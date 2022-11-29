package com.yscope.clp.compressorfrontend;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/**
 * Class to decode or search within encoded messages
 */
public class MessageDecoder {
  static {
    NativeLibraryLoader.load();
  }

  /**
   * Decodes the message with the given logtype and variables
   * @param logtype
   * @param dictionaryVars
   * @param encodedVars
   * @return The decoded message
   * @throws IOException if decoding fails
   */
  public static String decodeMessage(
      String logtype,
      String[] dictionaryVars,
      long[] encodedVars
  ) throws IOException {
    // Flatten dictionaryVars (["var1", "var2", ...] -> "var1var2...")
    int[] dictionaryVarEndOffsets = new int[dictionaryVars.length];
    ByteArrayOutputStream dictionaryVarsOutputStream = new ByteArrayOutputStream();
    int lastDictionaryVarEndOffset = 0;
    for (int i = 0; i < dictionaryVars.length; ++i) {
      byte[] dictionaryVarBytes = dictionaryVars[i].getBytes(StandardCharsets.UTF_8);
      dictionaryVarsOutputStream.write(dictionaryVarBytes);
      lastDictionaryVarEndOffset += dictionaryVarBytes.length;
      dictionaryVarEndOffsets[i] = lastDictionaryVarEndOffset;
    }

    byte[] messageBytes = decodeMessageNative(logtype.getBytes(StandardCharsets.ISO_8859_1),
        dictionaryVarsOutputStream.toByteArray(), dictionaryVarEndOffsets, encodedVars);
    return new String(messageBytes, StandardCharsets.UTF_8);
  }

  /**
   * Decodes the message from the given logtype and variables
   * @param logtype The message's logtype
   * @param allDictionaryVars The message's dictionary variables, stored
   *                          back-to-back in a single byte-array
   * @param dictionaryVarEndOffsets The end-offset of each dictionary variable
   *                                in {@code allDictionaryVars}
   * @param encodedVars The message's encoded variables
   * @return The decoded message
   * @throws IOException if the delimiters in the logtype don't match the number
   * of variables.
   */
  private static native byte[] decodeMessageNative(byte[] logtype, byte[] allDictionaryVars,
      int[] dictionaryVarEndOffsets, long[] encodedVars) throws IOException;

  /**
   * Checks whether any encoded integer variable matches the given wildcard
   * query
   * @param wildcardQuery
   * @param logtype
   * @param encodedVars
   * @return true if a match was found, false otherwise
   */
  public static boolean wildcardQueryMatchesAnyIntVar(
      String wildcardQuery,
      String logtype,
      long[] encodedVars
  ) {
    return wildcardQueryMatchesAnyIntVarNative(wildcardQuery.getBytes(StandardCharsets.ISO_8859_1),
        logtype.getBytes(StandardCharsets.ISO_8859_1), encodedVars);
  }

  private static native boolean wildcardQueryMatchesAnyIntVarNative(
      byte[] wildcardQuery,
      byte[] logtype,
      long[] encodedVars
  );

  /**
   * Checks whether any encoded float variable matches the given wildcard query
   * @param wildcardQuery
   * @param logtype
   * @param encodedVars
   * @return true if a match was found, false otherwise
   */
  public static boolean wildcardQueryMatchesAnyFloatVar(
      String wildcardQuery,
      String logtype,
      long[] encodedVars
  ) throws IOException {
    return wildcardQueryMatchesAnyFloatVarNative(
        wildcardQuery.getBytes(StandardCharsets.ISO_8859_1),
        logtype.getBytes(StandardCharsets.ISO_8859_1), encodedVars);
  }

  private static native boolean wildcardQueryMatchesAnyFloatVarNative(
      byte[] wildcardQuery,
      byte[] logtype,
      long[] encodedVars
  ) throws IOException;
}
