package com.yscope.clp.irstream;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

/**
 * CLP IR output stream using the four-byte encoding
 */
public class FourByteClpIrOutputStream extends AbstractClpIrOutputStream {
  private long previousMessageTimestamp;

  /**
   * @see AbstractClpIrOutputStream#AbstractClpIrOutputStream
   */
  public FourByteClpIrOutputStream (String timestampPattern, String timeZoneId,
                                    OutputStream outputStream)
  {
    super(timestampPattern, timeZoneId, outputStream);
    previousMessageTimestamp = 0;
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
                                          timeZoneIdBytes.length, firstMessageTimestamp);
    if (null == encoded) {
      throw new IOException("Encoding failed.");
    }

    previousMessageTimestamp = firstMessageTimestamp;

    return encoded;
  }

  @Override
  protected byte[] encodeLogEvent (long timestamp, ByteBuffer message) throws IOException {
    byte[] encoded = encodeLogEventNative(nativeStateAddress, timestamp - previousMessageTimestamp,
                                          message.array(), message.limit());
    if (null == encoded) {
      throw new IOException("Encoding failed.");
    }

    previousMessageTimestamp = timestamp;

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
   * @param referenceTimestamp The timestamp that the first log message's
   * timestamp delta is calculated from
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
      int timeZoneIdLength,
      long referenceTimestamp
  ) throws IOException;

  /**
   * Encodes the log event
   * @param nativeStateAddress
   * @param timestampDelta Milliseconds since the previous message's timestamp
   * @param message
   * @param messageLength
   * @return The encoded log event
   * @throws IOException on encode failure
   */
  private native byte[] encodeLogEventNative (
      long nativeStateAddress,
      long timestampDelta,
      byte[] message,
      int messageLength
  ) throws IOException;
}
