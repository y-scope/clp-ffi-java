package com.yscope.clp.compressorfrontend;

import java.io.*;
import java.nio.charset.StandardCharsets;

/**
 * Class to encode log messages
 */
public class MessageEncoder {
  static {
    NativeLibraryLoader.load();
    init();
  }

  /**
   * Initializes the native side of the class
   */
  private static native void init();

  /**
   * Constructs an object for encoding log messages using CLP.
   * @param variablesSchemaVersion The version of the variables schema to use to
   *                               parse the log messages.
   * @param variableEncodingMethodsVersion The version of variable encoding
   *                                       methods to use to encode the log
   *                                       messages.
   * @throws UnsupportedOperationException if either version was unknown or
   * unsupported.
   */
  public MessageEncoder(
      String variablesSchemaVersion,
      String variableEncodingMethodsVersion
  ) throws UnsupportedOperationException {
    setVariableHandlingRuleVersions(variablesSchemaVersion.getBytes(StandardCharsets.ISO_8859_1),
        variableEncodingMethodsVersion.getBytes(StandardCharsets.ISO_8859_1));
  }

  private native void setVariableHandlingRuleVersions (
      byte[] variablesSchemaVersion,
      byte[] variableEncodingMethodsVersion
  ) throws UnsupportedOperationException;

  /**
   * Encodes the given log message
   * @param message
   * @param encodedMessage
   * @throws IOException if encoding fails
   */
  public void encodeMessage(String message, EncodedMessage encodedMessage) throws IOException {
    byte[] messageAsBytes = message.getBytes(StandardCharsets.UTF_8);
    encodedMessage.setMessage(messageAsBytes);
    encodeMessageNative(messageAsBytes, encodedMessage);
  }

  private native void encodeMessageNative(byte[] message, EncodedMessage encodedMessage)
      throws IOException;
}
