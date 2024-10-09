package com.yscope.clp.compressorfrontend;

import java.util.Arrays;
import java.util.Iterator;
import java.util.NoSuchElementException;
import org.jetbrains.annotations.NotNull;

/**
 * A byte array that's the result of flattening a {@code byte[][]} by concatenating each
 * {@code byte[]} together and recording its end offset.
 */
public class FlattenedByteArray implements Iterable<byte[]> {
  public static final FlattenedByteArray EMPTY_FLATTENED_BYTE_ARRAY =
      new FlattenedByteArray(null, null);
  private final byte[] flattenedElems;
  private final int[] elemEndOffsets;

  public FlattenedByteArray(byte[] flattenedElems, int[] elemEndOffsets) {
    this.flattenedElems = null == flattenedElems ? new byte[0] : flattenedElems;
    this.elemEndOffsets = null == elemEndOffsets ? new int[0] : elemEndOffsets;
  }

  public byte @NotNull [] getFlattenedElems() {
    return flattenedElems;
  }

  public int @NotNull [] getElemEndOffsets() {
    return elemEndOffsets;
  }

  public int size() {
    return elemEndOffsets.length;
  }

  /**
   * NOTE: This method creates a new object for each iteration, which may impact performance
   * in tight loops or performance-critical sections. For high-performance scenarios,
   * consider using direct access methods like {@code getFlattenedElems()} and
   * {@code getElemEndOffsets()}.
   *
   * @return An iterator over each element in the flattened array.
   */
  public @NotNull Iterator<byte[]> iterator() {
    return new ByteArrayIterator();
  }

  private class ByteArrayIterator implements Iterator<byte[]> {
    private int currentIndex = 0;  // To track the current element being iterated

    @Override
    public boolean hasNext() {
      return currentIndex < elemEndOffsets.length;
    }

    @Override
    public byte[] next() {
      if (!hasNext()) {
        throw new NoSuchElementException("No more elements in the byte array.");
      }

      int startOffset = (currentIndex == 0) ? 0 : elemEndOffsets[currentIndex - 1];
      int endOffset = elemEndOffsets[currentIndex];
      currentIndex++;

      return Arrays.copyOfRange(flattenedElems, startOffset, endOffset);
    }

    @Override
    public void remove() {
      throw new UnsupportedOperationException("Remove operation is not supported.");
    }
  }
}
