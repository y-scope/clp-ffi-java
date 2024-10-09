package com.yscope.clp.compressorfrontend;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Objects;
import org.apache.commons.lang3.ArrayUtils;
import org.jetbrains.annotations.NotNull;

import static org.apache.commons.lang3.ArrayUtils.EMPTY_BYTE_ARRAY;
import static org.apache.commons.lang3.ArrayUtils.EMPTY_INT_ARRAY;


public class FlattenedByteArray implements Iterable<byte[]> {
  public static final FlattenedByteArray EMPTY_FLATTENED_BYTE_ARRAY =
      new FlattenedByteArray(EMPTY_BYTE_ARRAY, EMPTY_INT_ARRAY);
  private final byte[] flattenedElems;
  private final int[] elemEndOffsets;

  public FlattenedByteArray(byte[] flattenedElems, int[] elemEndOffsets) {
    this.flattenedElems = Objects.requireNonNullElse(flattenedElems, EMPTY_BYTE_ARRAY);
    this.elemEndOffsets = Objects.requireNonNullElse(elemEndOffsets, EMPTY_INT_ARRAY);
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
   * Provides an iterator over the individual elements in the flattened byte array.
   * Note: This method creates a new object for each iteration, which may impact performance
   * in tight loops or performance-critical sections. For high-performance scenarios,
   * consider using direct access methods like getFlattenedElems() and getElemEndOffsets().
   *
   * @return An iterator over the byte arrays representing each element
   */
  public @NotNull Iterator<byte[]> iterator() {
    return new ByteArrayIterator();
  }

  // Inner class that implements the Iterator for the flattened byte array
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

      // Extract the subarray from flattenedElems
      return ArrayUtils.subarray(flattenedElems, startOffset, endOffset);
    }

    @Override
    public void remove() {
      throw new UnsupportedOperationException("Remove operation is not supported.");
    }
  }
}
