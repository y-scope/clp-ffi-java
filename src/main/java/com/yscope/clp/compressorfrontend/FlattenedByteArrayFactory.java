package com.yscope.clp.compressorfrontend;

import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;

public class FlattenedByteArrayFactory {
    private static ThreadLocal<ByteArrayOutputStream> reusableByteArrayOutputStream;

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

        ByteArrayOutputStream outputStream;
        if (null == reusableByteArrayOutputStream) {
            outputStream = new ByteArrayOutputStream();
            reusableByteArrayOutputStream = ThreadLocal.withInitial(() -> outputStream);
        } else {
            outputStream = reusableByteArrayOutputStream.get();
            outputStream.reset();
        }

        int[] elemEndOffsets = new int[strings.length];
        for (int i = 0; i < strings.length; ++i) {
            byte[] bytes = strings[i].getBytes(StandardCharsets.UTF_8);
            outputStream.write(bytes, 0, bytes.length);
            elemEndOffsets[i] = outputStream.size();
        }

        return new FlattenedByteArray(outputStream.toByteArray(), elemEndOffsets);
    }
}
