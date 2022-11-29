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
      MessageEncoder messageEncoder = new MessageEncoder();

      // Test encoding and decoding
      messageEncoder.encodeMessage(message, encodedMessage);
      String logtypeAsString = new String(encodedMessage.logtype, StandardCharsets.ISO_8859_1);
      String decodedMessage = MessageDecoder.decodeMessage(
          logtypeAsString, encodedMessage.getDictionaryVarsAsStrings(),
          encodedMessage.encodedVars);
      assertEquals(message, decodedMessage);

      // Test searching encoded variables
      assertTrue(MessageDecoder.wildcardQueryMatchesAnyIntVar("1*3", logtypeAsString,
          encodedMessage.encodedVars));
      assertFalse(MessageDecoder.wildcardQueryMatchesAnyIntVar("4*7", logtypeAsString,
          encodedMessage.encodedVars));
      assertTrue(MessageDecoder.wildcardQueryMatchesAnyFloatVar("4*7", logtypeAsString,
          encodedMessage.encodedVars));
      assertFalse(MessageDecoder.wildcardQueryMatchesAnyFloatVar("1*3", logtypeAsString,
          encodedMessage.encodedVars));
    } catch (IOException e) {
      fail(e.getMessage());
    }
  }
}
