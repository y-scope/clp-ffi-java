package com.yscope.clp.compressorfrontend;

/**
 * An iterable of byte segments
 */
public class ByteSegments implements Iterable<ByteSegment> {
  protected final int[] segmentBounds;
  private final byte[] parent;

  /**
   * @param parent The byte array backing all the iterable segments
   * @param segmentBounds The bounds of all segments, laid out as follows:
   * [s1.beginIndex, s1.endIndex, s2.beginIndex, s2.endIndex, ...].
   */
  public ByteSegments (byte[] parent, int[] segmentBounds) {
    this.parent = parent;
    this.segmentBounds = segmentBounds;
  }

  @Override
  public Iterator iterator () {
    return new Iterator();
  }

  public class Iterator implements java.util.Iterator<ByteSegment> {
    protected int boundsIdx = 0;

    @Override
    public boolean hasNext() {
      return boundsIdx < segmentBounds.length;
    }

    @Override
    public ByteSegment next() {
      int beginIdx = segmentBounds[boundsIdx++];
      int endIdx = segmentBounds[boundsIdx++];
      return new ByteSegment(parent, beginIdx, endIdx);
    }
  }
}
