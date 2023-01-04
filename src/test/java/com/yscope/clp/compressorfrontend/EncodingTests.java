package com.yscope.clp.compressorfrontend;

import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

import static org.junit.jupiter.api.Assertions.*;

public class EncodingTests {
  @Test
  void testEncodings() {
    try {
      String message = "Static text, dictVar1, 123, 456.7, dictVar2, 987, 654.3";
      EncodedMessage encodedMessage = new EncodedMessage();
      MessageEncoder messageEncoder =
          new MessageEncoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
              BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);

      // Test encoding and decoding
      messageEncoder.encodeMessage(message, encodedMessage);
      String logtypeAsString = new String(encodedMessage.logtype, StandardCharsets.ISO_8859_1);
      MessageDecoder messageDecoder =
          new MessageDecoder(BuiltInVariableHandlingRuleVersions.VariablesSchemaV2,
              BuiltInVariableHandlingRuleVersions.VariableEncodingMethodsV1);
      String decodedMessage = messageDecoder.decodeMessage(
          logtypeAsString, encodedMessage.getDictionaryVarsAsStrings(),
          encodedMessage.encodedVars);
      assertEquals(message, decodedMessage);

      // Test searching encoded variables
      assertTrue(messageDecoder.wildcardQueryMatchesAnyIntVar("1*3", logtypeAsString,
          encodedMessage.encodedVars));
      assertFalse(messageDecoder.wildcardQueryMatchesAnyIntVar("4*7", logtypeAsString,
          encodedMessage.encodedVars));
      assertTrue(messageDecoder.wildcardQueryMatchesAnyFloatVar("4*7", logtypeAsString,
          encodedMessage.encodedVars));
      assertFalse(messageDecoder.wildcardQueryMatchesAnyFloatVar("1*3", logtypeAsString,
          encodedMessage.encodedVars));
    } catch (IOException e) {
      fail(e.getMessage());
    }
  }
}
