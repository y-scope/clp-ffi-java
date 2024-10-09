package com.yscope.clp.compressorfrontend;

/**
 * Utilities to convert null arrays into empty arrays. These utilities allow callers to avoid
 * creating empty arrays unnecessarily, reducing unnecessary garbage collection (GC) pressure.
 */
public class EmptyArrayUtils {
  public static byte[] EMPTY_BYTE_ARRAY = new byte[0];
  public static int[] EMPTY_INT_ARRAY = new int[0];
  public static long[] EMPTY_LONG_ARRAY = new long[0];
  public static FlattenedByteArray EMPTY_FLATTENED_BYTE_ARRAY =
      new FlattenedByteArray(EMPTY_BYTE_ARRAY, EMPTY_INT_ARRAY);

  public static byte[] getNonNullByteArray(byte[] byteArray) {
    return (null == byteArray) ? EMPTY_BYTE_ARRAY : byteArray;
  }

  public static int[] getNonNullIntArray(int[] intArray) {
    return (null == intArray) ? EMPTY_INT_ARRAY : intArray;
  }

  public static long[] getNonNullLongArray(long[] longArray) {
    return (null == longArray) ? EMPTY_LONG_ARRAY : longArray;
  }

  public static FlattenedByteArray getNonNullFlattenedByteArray(
      FlattenedByteArray flattenedByteArray
  ) {
    return (null == flattenedByteArray) ? EMPTY_FLATTENED_BYTE_ARRAY : flattenedByteArray;
  }
}
