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
  private final byte[] flattenedElems;
  private final int[] elemEndOffsets;

  public FlattenedByteArray(byte[] flattenedElems, int[] elemEndOffsets) {
    this.flattenedElems = (null == flattenedElems)
        ? EmptyArrayUtils.EMPTY_BYTE_ARRAY : flattenedElems;
    this.elemEndOffsets = (null == elemEndOffsets)
        ? EmptyArrayUtils.EMPTY_INT_ARRAY : elemEndOffsets;
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
    private int currentElemIdx = 0;

    @Override
    public boolean hasNext() {
      return currentElemIdx < elemEndOffsets.length;
    }

    @Override
    public byte[] next() {
      if (false == hasNext()) {
        throw new NoSuchElementException("No more elements in the byte array.");
      }

      int beginOffset = (0 == currentElemIdx) ? 0 : elemEndOffsets[currentElemIdx - 1];
      int endOffset = elemEndOffsets[currentElemIdx];
      currentElemIdx++;

      return Arrays.copyOfRange(flattenedElems, beginOffset, endOffset);
    }
  }
}
