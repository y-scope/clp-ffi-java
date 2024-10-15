package com.yscope.clp.compressorfrontend;

import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;

public class FlattenedByteArrayFactory {
    private static final ThreadLocal<ByteArrayOutputStream> reusableByteArrayOutputStream = ThreadLocal.withInitial(ByteArrayOutputStream::new);

    /**
     * Constructs a FlattenedByteArray by concatenating the given byte arrays together.
     * @param byteArrays
     * @return The flattened array.
     */
    public static FlattenedByteArray fromByteArrays(byte[][] byteArrays) {
        if (null == byteArrays || 0 == byteArrays.length) {
            return EmptyArrayUtils.EMPTY_FLATTENED_BYTE_ARRAY;
        }

        // Compute flattenedElemsEndOffsets
        int flattenedElemsLength = 0;
        int[] flattenedElemsEndOffsets = new int[byteArrays.length];
        for (int i = 0; i < byteArrays.length; i++) {
            flattenedElemsLength += byteArrays[i].length;
            flattenedElemsEndOffsets[i] = flattenedElemsLength;
        }

        // Compute flattenedElems
        byte[] flattenedElems = new byte[flattenedElemsLength];
        for (int i = 0, flattenedElemsOffset = 0; i < byteArrays.length; i++) {
            byte[] byteArray = byteArrays[i];
            System.arraycopy(byteArray, 0, flattenedElems, flattenedElemsOffset, byteArray.length);
            flattenedElemsOffset += byteArray.length;
        }

        return new FlattenedByteArray(flattenedElems, flattenedElemsEndOffsets);
    }

    /**
     * Constructs a FlattenedByteArray by converting the given strings into byte arrays and then
     * concatenating them together.
     * @param strings
     * @return The flattened array.
     */
    public static FlattenedByteArray fromStrings(String[] strings) {
        if (null == strings || 0 == strings.length) {
            return EmptyArrayUtils.EMPTY_FLATTENED_BYTE_ARRAY;
        }

        ByteArrayOutputStream outputStream = reusableByteArrayOutputStream.get();
        outputStream.reset();

        int[] elemEndOffsets = new int[strings.length];
        for (int i = 0; i < strings.length; ++i) {
            byte[] bytes = strings[i].getBytes(StandardCharsets.UTF_8);
            outputStream.write(bytes, 0, bytes.length);
            elemEndOffsets[i] = outputStream.size();
        }

        return new FlattenedByteArray(outputStream.toByteArray(), elemEndOffsets);
    }
}
