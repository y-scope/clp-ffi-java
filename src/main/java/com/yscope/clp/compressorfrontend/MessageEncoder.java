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
