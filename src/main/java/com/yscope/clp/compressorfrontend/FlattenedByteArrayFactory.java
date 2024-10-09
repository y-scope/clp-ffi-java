package com.yscope.clp.compressorfrontend;

import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;

public class FlattenedByteArrayFactory {
    private static ByteArrayOutputStream reusableByteArrayOutputStream;

    /**
     * Constructs a FlattenedByteArray by converting the given strings into byte arrays and then
     * concatenating them together.
     * @param strings
     * @return The flattened array.
     */
    public static FlattenedByteArray fromStrings(String[] strings) {
        if (null == strings || 0 == strings.length) {
            return FlattenedByteArray.EMPTY_FLATTENED_BYTE_ARRAY;
        }

        if (null == reusableByteArrayOutputStream) {
            reusableByteArrayOutputStream = new ByteArrayOutputStream();
        } else {
            reusableByteArrayOutputStream.reset();
        }

        int[] elemEndOffsets = new int[strings.length];
        for (int i = 0; i < strings.length; ++i) {
            byte[] bytes = strings[i].getBytes(StandardCharsets.UTF_8);
            reusableByteArrayOutputStream.write(bytes, 0, bytes.length);
            elemEndOffsets[i] = reusableByteArrayOutputStream.size();
        }

        return new FlattenedByteArray(reusableByteArrayOutputStream.toByteArray(), elemEndOffsets);
    }
}
