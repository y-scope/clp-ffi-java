package com.yscope.clp.compressorfrontend;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.Objects;

import org.jetbrains.annotations.NotNull;

/**
 * Class to encode log messages
 */
public class MessageEncoder {
  static {
    NativeLibraryLoader.load();
  }

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
      @NotNull String variablesSchemaVersion,
      @NotNull String variableEncodingMethodsVersion
  ) throws UnsupportedOperationException {
    Objects.requireNonNull(variablesSchemaVersion);
    Objects.requireNonNull(variableEncodingMethodsVersion);
    byte[] schemaVersionBytes = variablesSchemaVersion.getBytes(StandardCharsets.ISO_8859_1);
    byte[] encodingMethodsVersionBytes =
        variableEncodingMethodsVersion.getBytes(StandardCharsets.ISO_8859_1);
    setVariableHandlingRuleVersions(schemaVersionBytes, schemaVersionBytes.length,
                                    encodingMethodsVersionBytes,
                                    encodingMethodsVersionBytes.length);
  }

  private native void setVariableHandlingRuleVersions (
      byte[] variablesSchemaVersion,
      int variablesSchemaVersionLen,
      byte[] variableEncodingMethodsVersion,
      int variableEncodingMethodsVersionLen
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
