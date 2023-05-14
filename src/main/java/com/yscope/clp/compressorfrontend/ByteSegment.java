package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

/**
 * A view of part of a byte array, similar to {@link javax.swing.text.Segment}
 */
public class ByteSegment {
  private final int beginIndex;
  private final int endIndex;
  private final byte[] parent;

  /**
   * @param parent The byte array backing this segment
   * @param beginIndex The index of the first byte in this segment
   * @param endIndex The index of the byte after the last byte in this segment
   */
  ByteSegment (byte[] parent, int beginIndex, int endIndex) {
    this.parent = parent;
    this.beginIndex = beginIndex;
    this.endIndex = endIndex;
  }

  public int getBeginIndex() {
    return beginIndex;
  }

  public int getEndIndex() {
    return endIndex;
  }

  @Override
  public String toString () {
    return new String(parent, beginIndex, endIndex - beginIndex,
        StandardCharsets.ISO_8859_1);
  }

  public byte[] toByteArray () {
    return Arrays.copyOfRange(parent, beginIndex, endIndex);
  }
}
