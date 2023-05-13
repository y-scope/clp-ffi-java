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
   * Constructs an object for decoding CLP-encoded log messages.
   * @param variablesSchemaVersion The version of the variables schema used to
   *                               parse the log messages.
   * @param variableEncodingMethodsVersion The version of variable encoding
   *                                       methods used to encode the log
   *                                       messages.
   * @throws UnsupportedOperationException if either version was unknown or
   * unsupported.
   */
  public MessageDecoder(
      String variablesSchemaVersion,
      String variableEncodingMethodsVersion
  ) throws UnsupportedOperationException {
    setVariableHandlingRuleVersions(variablesSchemaVersion.getBytes(StandardCharsets.ISO_8859_1),
        variableEncodingMethodsVersion.getBytes(StandardCharsets.ISO_8859_1));
  }

  private native void setVariableHandlingRuleVersions (
      byte[] variablesSchemaVersion,
      byte[] variableEncodingMethodsVersion
  ) throws UnsupportedOperationException;

  /**
   * Decodes the message with the given logtype and variables
   * @param logtype
   * @param dictionaryVars
   * @param encodedVars
   * @return The decoded message
   * @throws IOException if decoding fails
   */
  public String decodeMessage(
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
  private native byte[] decodeMessageNative(byte[] logtype, byte[] allDictionaryVars,
      int[] dictionaryVarEndOffsets, long[] encodedVars) throws IOException;

  /**
   * Checks whether any encoded integer variable matches the given wildcard
   * query
   * @param wildcardQuery
   * @param logtype
   * @param encodedVars
   * @return true if a match was found, false otherwise
   */
  public boolean wildcardQueryMatchesAnyIntVar(
      String wildcardQuery,
      String logtype,
      long[] encodedVars
  ) throws IOException {
    return wildcardQueryMatchesAnyIntVarNative(wildcardQuery.getBytes(StandardCharsets.ISO_8859_1),
        logtype.getBytes(StandardCharsets.ISO_8859_1), encodedVars);
  }

  private native boolean wildcardQueryMatchesAnyIntVarNative(
      byte[] wildcardQuery,
      byte[] logtype,
      long[] encodedVars
  ) throws IOException;

  /**
   * Checks whether any encoded float variable matches the given wildcard query
   * @param wildcardQuery
   * @param logtype
   * @param encodedVars
   * @return true if a match was found, false otherwise
   */
  public boolean wildcardQueryMatchesAnyFloatVar(
      String wildcardQuery,
      String logtype,
      long[] encodedVars
  ) throws IOException {
    return wildcardQueryMatchesAnyFloatVarNative(
        wildcardQuery.getBytes(StandardCharsets.ISO_8859_1),
        logtype.getBytes(StandardCharsets.ISO_8859_1), encodedVars);
  }

  private native boolean wildcardQueryMatchesAnyFloatVarNative(
      byte[] wildcardQuery,
      byte[] logtype,
      long[] encodedVars
  ) throws IOException;

  /**
   * For each log message in a batch of messages, checks whether the given
   * wildcard queries for variables match the message's encoded variables.
   * <br>
   * For example, consider a user's wildcard query, "*123*456*789*". One
   * potential CLP subquery might have this original wildcard query decomposed
   * into three wildcard queries for three encoded variables: ["*123*", "*456*",
   * "*789*"]. We refer to these decomposed wildcard queries as variable
   * wildcard-queries. Consider matching these variable wildcard-queries against
   * a message with these encoded variables: [123, 0, 456, 0, 789]. This method
   * should return true since each of the variable wildcard-queries matches a
   * unique encoded variable, in sequence.
   * <br>
   * Specifically, let {w in W} be the set of variable wildcard-queries and
   * {e in E} be the set of encoded variables. This method will return true if
   * and only if:
   * (1) Each unique `w` matches a unique `e`.
   * (2) When (1) is true, the order of elements in both W and E is unchanged.
   * <br>
   * NOTE 1: This is just one part of CLP's query processing algorithm. For more
   * details, see
   * https://github.com/y-scope/clp/blob/main/components/core/src/ffi/search/README.md
   * <br>
   * NOTE 2: Instead of taking an array of objects, this method takes arrays of
   * object-members (the result of serializing the objects) since this is
   * currently called from contexts where the objects will have already been
   * serialized.
   * @param logtypes The logtypes of the messages
   * @param encodedVarArrays Rows of encoded variables corresponding to the
   * messages
   * @param wildcardVarPlaceholders An array of variable placeholders, one for
   * each variable (or potential variable) containing a wildcard, indicating how
   * the variable query should be interpreted.
   * @param serializedVarWildcardQueries The wildcard queries to compare with
   * the encoded variables. Callers must ensure each wildcard query contains no
   * redundant wildcards (e.g. "**") nor unnecessary escape characters (e.g.
   * "\"). The queries are serialized into a single array with each query
   * following the previous query, and each query's end index specified in
   * {@code varWildcardQueryEndIndexes}.
   * @param varWildcardQueryEndIndexes The end index of each wildcard query in
   * {@code serializedVarWildcardQueries}
   * @param matchResults Returns the match result per message
   */
  public void batchEncodedVarsWildcardMatch (
      byte[][] logtypes,
      long[][] encodedVarArrays,
      byte[] wildcardVarPlaceholders,
      byte[] serializedVarWildcardQueries,
      int[] varWildcardQueryEndIndexes,
      int[] matchResults
  ) throws IOException {
    // Validate the array lengths
    if (logtypes.length != matchResults.length) {
      throw new IllegalArgumentException("Number of logtypes given doesn't match size of output "
                                             + "array.");
    }
    if (encodedVarArrays.length != matchResults.length) {
      throw new IllegalArgumentException("Number of encoded variable arrays given doesn't match "
                                             + "size of output array.");
    }

    // Validate the end indexes
    int lastIdx = 0;
    for (int i : varWildcardQueryEndIndexes) {
      if (i < lastIdx) {
        throw new IllegalArgumentException("Invalid wildcard variable query end index: i (" + i
                                               + ") < lastIdx (" + lastIdx + ")");
      }
      if (i > serializedVarWildcardQueries.length) {
        throw new IllegalArgumentException("Invalid wildcard variable query end index: i (" + i
                                               + ") > serializedVarWildcardQueries.length ("
                                               + lastIdx + ")");
      }
      lastIdx = i;
    }

    batchEncodedVarsWildcardMatchNative(
        logtypes.length,
        logtypes,
        encodedVarArrays,
        wildcardVarPlaceholders,
        wildcardVarPlaceholders.length,
        serializedVarWildcardQueries,
        serializedVarWildcardQueries.length,
        varWildcardQueryEndIndexes,
        varWildcardQueryEndIndexes.length,
        matchResults);
  }

  /**
   * Same as {@link MessageDecoder#batchEncodedVarsWildcardMatch} except array
   * lengths are specified separately to avoid unnecessary calls into the JVM
   * from the native code.
   * @param numMessages
   * @param logtypes
   * @param encodedVarArrays
   * @param wildcardVarPlaceholders
   * @param wildcardVarPlaceholdersLen
   * @param serializedVarWildcardQueries
   * @param serializedVarWildcardQueriesLen
   * @param varWildcardQueryEndIndexes
   * @param varWildcardQueryEndIndexesLen
   * @param matchResults
   */
  private native void batchEncodedVarsWildcardMatchNative (
      int numMessages,
      byte[][] logtypes,
      long[][] encodedVarArrays,
      byte[] wildcardVarPlaceholders,
      int wildcardVarPlaceholdersLen,
      byte[] serializedVarWildcardQueries,
      int serializedVarWildcardQueriesLen,
      int[] varWildcardQueryEndIndexes,
      int varWildcardQueryEndIndexesLen,
      int[] matchResults
  ) throws IOException;
}
