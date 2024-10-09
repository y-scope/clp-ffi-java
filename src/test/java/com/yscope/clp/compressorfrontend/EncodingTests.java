package com.yscope.clp.compressorfrontend;

import com.yscope.clp.compressorfrontend.AbstractClpEncodedSubquery.VariableWildcardQuery;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class EncodingTests {
  @Test
  void testEncodingMessages () {
    try {
      MessageEncoder messageEncoder =
          new MessageEncoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                             BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
      EncodedMessage encodedMessage = new EncodedMessage();
      MessageDecoder messageDecoder =
          new MessageDecoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                             BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
      String message;

      // Test encoding and decoding
      message = "Static text only";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      message = "dictVar1Only";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      message = "1.1";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      message = "One dictVar1";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      message = "1 encoded var";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      message = "Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3";
      testEncodeAndDecode(message, messageEncoder, messageDecoder, encodedMessage);

      // Test searching encoded variables
      String logtypeAsString = encodedMessage.getLogTypeAsString();
      assertTrue(messageDecoder.wildcardQueryMatchesAnyIntVar("1*3", logtypeAsString,
          encodedMessage.getEncodedVars()));
      assertFalse(messageDecoder.wildcardQueryMatchesAnyIntVar("4*7", logtypeAsString,
          encodedMessage.getEncodedVars()));
      assertTrue(messageDecoder.wildcardQueryMatchesAnyFloatVar("4*7", logtypeAsString,
          encodedMessage.getEncodedVars()));
      assertFalse(messageDecoder.wildcardQueryMatchesAnyFloatVar("1*3", logtypeAsString,
          encodedMessage.getEncodedVars()));

      // Test decoding invalid encoded messages
      assertThrows(IOException.class, () -> {
        messageDecoder.decodeMessage(
            encodedMessage.getLogTypeAsString(), null,
            encodedMessage.getEncodedVars());
      });
      assertThrows(IOException.class, () -> {
        messageDecoder.decodeMessage(
            encodedMessage.getLogTypeAsString(), encodedMessage.getDictionaryVarsAsStrings(),
            null);
      });
    } catch (IOException e) {
      fail(e.getMessage());
    }
  }

  private void testEncodeAndDecode (String originalMessage, MessageEncoder messageEncoder, MessageDecoder messageDecoder,
      EncodedMessage encodedMessage)
      throws IOException {
    messageEncoder.encodeMessage(originalMessage, encodedMessage);

    // Test decoding from strings
    String decodedMessage = messageDecoder.decodeMessage(
        encodedMessage.getLogTypeAsString(),
        encodedMessage.getDictionaryVarsAsStrings(),
        encodedMessage.getEncodedVars()
    );
    assertEquals(originalMessage, decodedMessage);

    // Test decoding from byte arrays
    decodedMessage = messageDecoder.decodeMessage(
        encodedMessage.getLogtype(),
        encodedMessage.getDictionaryVarsAsFlattenedByteArray(),
        encodedMessage.getEncodedVars()
    );
    assertEquals(originalMessage, decodedMessage);
  }

  @Test
  void testEncodingQueries () {
    EightByteClpWildcardQueryEncoder encoder = new EightByteClpWildcardQueryEncoder(
        BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
        BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1
    );

    // Validate encoding a message as a query
    String query = "Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3";
    EightByteClpEncodedSubquery[] subqueries = encoder.encode(query);
    assertEquals(1, subqueries.length);
    EightByteClpEncodedSubquery subquery = subqueries[0];
    assertTrue(subquery.containsVariables());
    assertFalse(subquery.logtypeQueryContainsWildcards());
    assertFalse(subquery.getDictVarWildcardQueries().iterator().hasNext());
    assertFalse(subquery.getEncodedVarWildcardQueries().iterator().hasNext());

    // Validate that the query can be decoded correctly
    MessageDecoder messageDecoder =
        new MessageDecoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
                           BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
    String decodedQuery;
    try {
      String logtypeQuery = subquery.getLogtypeQueryAsString();
      String[] dictVars = new String[subquery.getNumDictVars()];
      int i = 0;
      for (ByteSegment s : subquery.getDictVars()) {
        dictVars[i++] = s.toString();
      }
      long[] encodedVars = subquery.getEncodedVars();
      decodedQuery = messageDecoder.decodeMessage(logtypeQuery, dictVars, encodedVars);
    } catch (Exception e) {
      fail("Failed to decode encoded query", e);
      // We'll never execute this return, but it's necessary for static analysis
      // to pass
      return;
    }
    assertEquals(query, decodedQuery);

    // Validate encoding a query with wildcards
    String wildcardQuery = "*123*";
    subqueries = encoder.encode(wildcardQuery);
    // Based on our current built-in schemas, this should generate at least two
    // subqueries: one for a dictionary variable and one for a non-dictionary
    // variable
    assertTrue(subqueries.length > 1);

    for (EightByteClpEncodedSubquery s : subqueries) {
      assertTrue(s.containsVariables());
      assertTrue(s.logtypeQueryContainsWildcards());
      // Each subquery should have at least one wildcard variable, but not
      // both a dictionary variable and an encoded variable
      assertTrue((s.getNumDictVarWildcardQueries() > 0)
                     ^ (s.getNumEncodedVarWildcardQueries() > 0));
    }

    // Validate error for an empty query
    assertThrows(IllegalArgumentException.class, () -> encoder.encode(""));

    // TODO Add some more tests
  }

  @Test
  void testBatchEncodedVarsWildcardMatchNative () {
    MessageEncoder messageEncoder = new MessageEncoder(
        BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
        BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
    EncodedMessage encodedMessage = new EncodedMessage();

    // Encode some messages
    final int numMessages = 5;
    byte[][] logtypes = new byte[numMessages][];
    long[][] encodedVarArrays = new long[numMessages][];
    try {
      int msgIdx = 0;
      messageEncoder.encodeMessage("Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3",
                                   encodedMessage);
      logtypes[msgIdx] = encodedMessage.getLogtype();
      encodedVarArrays[msgIdx] = encodedMessage.getEncodedVars();
      ++msgIdx;

      messageEncoder.encodeMessage("Message with only static text.", encodedMessage);
      logtypes[msgIdx] = encodedMessage.getLogtype();
      encodedVarArrays[msgIdx] = encodedMessage.getEncodedVars();
      ++msgIdx;

      messageEncoder.encodeMessage("Message with 1 + 1 encoded variables.", encodedMessage);
      logtypes[msgIdx] = encodedMessage.getLogtype();
      encodedVarArrays[msgIdx] = encodedMessage.getEncodedVars();
      ++msgIdx;

      messageEncoder.encodeMessage("Message with dictVar1 and dictVar2.", encodedMessage);
      logtypes[msgIdx] = encodedMessage.getLogtype();
      encodedVarArrays[msgIdx] = encodedMessage.getEncodedVars();
      ++msgIdx;

      // Invalid logtype
      logtypes[msgIdx] = null;
      encodedVarArrays[msgIdx] = encodedMessage.getEncodedVars();
      ++msgIdx;
    } catch (IOException e) {
      fail("Failed to encode messages", e);
    }

    // Encode a query
    EightByteClpWildcardQueryEncoder queryEncoder = new EightByteClpWildcardQueryEncoder(
        BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
        BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1
    );
    EightByteClpEncodedSubquery[] subqueries = queryEncoder.encode("*1* *987*");

    // Match the messages against the subqueries
    int[] matchingRows = new int[numMessages];
    try {
      MessageDecoder messageDecoder = new MessageDecoder(
          BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
          BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);

      for (EightByteClpEncodedSubquery subquery : subqueries) {
        // Prepare arguments for batchEncodedVarsWildcardMatch
        int numEncodedVarWildcardQueries = subquery.getNumEncodedVarWildcardQueries();
        byte[] encodedVarWildcardTypes = new byte[numEncodedVarWildcardQueries];
        int[] encodedVarWildcardQueryEndIndexes = new int[numEncodedVarWildcardQueries];
        ByteArrayOutputStream serializedEncodedVarWildcardQueries = new ByteArrayOutputStream();
        int wildcardEncodedVarIdx = 0;
        for (VariableWildcardQuery q : subquery.getEncodedVarWildcardQueries()) {
          encodedVarWildcardTypes[wildcardEncodedVarIdx] = q.getType();
          serializedEncodedVarWildcardQueries.write(q.getQuery().toByteArray());
          encodedVarWildcardQueryEndIndexes[wildcardEncodedVarIdx] =
              serializedEncodedVarWildcardQueries.size();

          ++wildcardEncodedVarIdx;
        }
        int[] matchResults = new int[logtypes.length];

        messageDecoder.batchEncodedVarsWildcardMatch(
            logtypes,
            encodedVarArrays,
            encodedVarWildcardTypes,
            serializedEncodedVarWildcardQueries.toByteArray(),
            encodedVarWildcardQueryEndIndexes,
            matchResults);
        for (int i = 0; i < matchingRows.length; ++i) {
          matchingRows[i] += matchResults[i];
        }
      }
    } catch (Exception e) {
      fail(e.getMessage(), e);
    }

    int rowIdx = 0;
    // First row should've matched at least one subquery
    assertTrue(matchingRows[rowIdx++] > 0);
    // Second row shouldn't match any subquery
    assertEquals(0, matchingRows[rowIdx++]);
    // Third row should've matched at least one subquery
    assertTrue(matchingRows[rowIdx++] > 0);
    // Fourth row shouldn't match any subquery
    assertEquals(0, matchingRows[rowIdx++]);
    // Fifth row is invalid and shouldn't match any subquery
    assertEquals(0, matchingRows[rowIdx++]);
  }
}
