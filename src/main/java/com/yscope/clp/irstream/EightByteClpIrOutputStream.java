package com.yscope.clp.irstream;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

/**
 * CLP IR output stream using the eight-byte encoding
 */
public class EightByteClpIrOutputStream extends AbstractClpIrOutputStream {
  /**
   * @see AbstractClpIrOutputStream#AbstractClpIrOutputStream
   */
  public EightByteClpIrOutputStream (String timestampPattern, String timeZoneId,
                                     OutputStream outputStream)
  {
    super(timestampPattern, timeZoneId, outputStream);
  }

  @Override
  protected byte[] encodePreamble (long firstMessageTimestamp) throws IOException {
    byte[] timestampPatternBytes = timestampPattern.getBytes(StandardCharsets.UTF_8);
    byte[] timestampPatternSyntaxBytes = "java::SimpleDateFormat".getBytes(StandardCharsets.UTF_8);
    byte[] timeZoneIdBytes = timeZoneId.getBytes(StandardCharsets.UTF_8);
    byte[] encoded = encodePreambleNative(nativeStateAddress, timestampPatternBytes,
                                          timestampPatternBytes.length,
                                          timestampPatternSyntaxBytes,
                                          timestampPatternSyntaxBytes.length, timeZoneIdBytes,
                                          timeZoneIdBytes.length);
    if (null == encoded) {
      throw new IOException("Encoding failed.");
    }
    return encoded;
  }

  @Override
  protected byte[] encodeLogEvent (long timestamp, ByteBuffer message) throws IOException {
    byte[] messageBytes = message.array();
    int messageLength = message.limit();
    byte[] encoded = encodeLogEventNative(nativeStateAddress, timestamp, messageBytes,
                                          messageLength);
    if (null == encoded) {
      throw new IOException("Encoding failed.");
    }
    return encoded;
  }

  /**
   * Encodes the IR stream's preamble
   * @param nativeStateAddress
   * @param timestampPattern
   * @param timestampPatternLength
   * @param timestampPatternSyntax
   * @param timestampPatternSyntaxLength
   * @param timeZoneId
   * @param timeZoneIdLength
   * @return The encoded preamble
   * @throws IOException on encode failure
   */
  private native byte[] encodePreambleNative (
      long nativeStateAddress,
      byte[] timestampPattern,
      int timestampPatternLength,
      byte[] timestampPatternSyntax,
      int timestampPatternSyntaxLength,
      byte[] timeZoneId,
      int timeZoneIdLength
  ) throws IOException;

  /**
   * Encodes the log event
   * @param nativeStateAddress
   * @param timestamp Timestamp as milliseconds from the Unix epoch
   * @param message
   * @param messageLength
   * @return The encoded log event
   * @throws IOException on encode failure
   */
  private native byte[] encodeLogEventNative (
      long nativeStateAddress,
      long timestamp,
      byte[] message,
      int messageLength
  ) throws IOException;
}
