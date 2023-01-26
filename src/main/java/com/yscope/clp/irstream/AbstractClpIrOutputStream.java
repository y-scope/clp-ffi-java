package com.yscope.clp.irstream;

import com.yscope.clp.compressorfrontend.NativeLibraryLoader;
import java.io.Flushable;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

/**
 * Base class for CLP IR output streams.
 */
public abstract class AbstractClpIrOutputStream implements AutoCloseable, Flushable {
  protected long nativeStateAddress;

  protected String timestampPattern;
  protected String timeZoneId;

  private final OutputStream outputStream;

  static {
    NativeLibraryLoader.load();
  }

  /**
   * Abstract constructor for output streams
   * @param timestampPattern Timestamp pattern used by log events in this stream
   * @param timeZoneId Time zone of log events in this stream
   * @param outputStream Stream which IR should be written to
   */
  public AbstractClpIrOutputStream (String timestampPattern, String timeZoneId,
                                    OutputStream outputStream)
  {
    this.nativeStateAddress = createNativeState();
    this.timestampPattern = timestampPattern;
    this.timeZoneId = timeZoneId;
    this.outputStream = outputStream;
  }

  /**
   * Closes the stream
   * @throws IOException on I/O error
   */
  @Override
  public void close () throws IOException {
    outputStream.write(getEofByte());
    outputStream.close();

    destroyNativeState(nativeStateAddress);
    nativeStateAddress = 0;
  }

  /**
   * Flushes the stream
   * @throws IOException on I/O error
   */
  @Override
  public void flush () throws IOException {
    outputStream.flush();
  }

  /**
   * Writes the given log event to the stream
   * @param timestamp Timestamp of the event as milliseconds since the Unix
   * epoch
   * @param message
   * @throws IOException on failure to encode the message or I/O error
   */
  public void writeLogEvent (long timestamp, ByteBuffer message) throws IOException {
    if (0 == nativeStateAddress) {
      throw new IOException("Stream closed.");
    }

    if (null != timestampPattern) {
      byte[] preamble = encodePreamble(timestamp);
      outputStream.write(preamble);
      // Clear these members now that they've been written to the preamble
      timestampPattern = null;
      timeZoneId = null;
    }

    byte[] encodedLogEvent = encodeLogEvent(timestamp, message);
    outputStream.write(encodedLogEvent);
  }

  /**
   * Encodes the preamble into the IR stream format
   * @param firstMessageTimestamp Timestamp of the first message in this stream
   * @return The encoded preamble
   * @throws IOException on encode failure
   */
  protected abstract byte[] encodePreamble (long firstMessageTimestamp) throws IOException;

  /**
   * Encodes the log event into the IR stream format
   * @param timestamp Timestamp of the event as milliseconds from the Unix epoch
   * @param message
   * @return The encoded message
   * @throws IOException on encode failure
   */
  protected abstract byte[] encodeLogEvent (long timestamp, ByteBuffer message) throws IOException;

  /**
   * Creates the native state necessary for this output stream
   * @return A native address for the state
   */
  private native long createNativeState ();

  /**
   * Destroys this output stream's native state
   * @param stateAddress The native address of the state
   */
  private native void destroyNativeState (long stateAddress);

  /**
   * @return The EOF byte for the IR stream
   */
  private native byte getEofByte ();
}
