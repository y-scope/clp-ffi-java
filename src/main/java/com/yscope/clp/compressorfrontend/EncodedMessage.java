package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.Objects;
import java.util.stream.LongStream;

import org.jetbrains.annotations.NotNull;

public class EncodedMessage {
  private byte[] logtype;
  private long[] encodedVars;
  private int[] dictionaryVarBounds;

  private FlattenedByteArray flattenedDictVars;

  private byte[] message;

  /**
   * Sets the message to which this encoded message corresponds.
   * This allows the encoded message to store dictionary variables as bounds
   * within the original message rather than as new strings
   * @param message
   */
  public void setMessage(byte @NotNull [] message) {
    Objects.requireNonNull(message);

    this.message = message;

    // Clear the encoded values since they no longer match
    logtype = null;
    encodedVars = null;
    dictionaryVarBounds = null;
    flattenedDictVars = null;
  }

  public byte[] getLogtype() {
    return logtype;
  }

  /**
   * @return The logtype after converting it to a {@code String}.
   */
  public String getLogTypeAsString() {
    if (null == logtype) {
      return null;
    } else {
      return new String(logtype, StandardCharsets.ISO_8859_1);
    }
  }

  /**
   * @return The dictionary variables instantiated as {@code String}s.
   */
  public String[] getDictionaryVarsAsStrings() {
    if (null == dictionaryVarBounds) {
      return null;
    }

    int numVars = dictionaryVarBounds.length / 2;
    String[] dictVars = new String[numVars];
    for (int i = 0, j = 0; i < dictVars.length; ++i) {
      int beginOffset = dictionaryVarBounds[j++];
      int endOffset = dictionaryVarBounds[j++];
      dictVars[i] = new String(message, beginOffset,
          endOffset - beginOffset, StandardCharsets.ISO_8859_1);
    }
    return dictVars;
  }

  /**
   * @return The dictionary variables serialized into a flattened byte array.
   */
  public FlattenedByteArray getDictionaryVarsAsFlattenedByteArray() {
    if (null == dictionaryVarBounds) {
      return null;
    }

    if (null != flattenedDictVars) {
      return flattenedDictVars;
    }

    // Compute flattenedVarsEndOffsets
    int flattenedVarsLength = 0;
    int[] flattenedVarsEndOffsets = new int[dictionaryVarBounds.length / 2];
    for (
        int varBoundsIdx = 0, endOffsetsIdx = 0;
        varBoundsIdx < dictionaryVarBounds.length;
        endOffsetsIdx++
    ) {
      int varBeginOffset = dictionaryVarBounds[varBoundsIdx++];
      int varEndOffset = dictionaryVarBounds[varBoundsIdx++];
      flattenedVarsLength += varEndOffset - varBeginOffset;
      flattenedVarsEndOffsets[endOffsetsIdx] = flattenedVarsLength;
    }

    // Compute flattenedVars
    byte[] flattenedVars = new byte[flattenedVarsLength];
    for (int varBoundsIdx = 0, flattenedVarsOffset = 0; varBoundsIdx < dictionaryVarBounds.length;) {
      int varBeginOffset = dictionaryVarBounds[varBoundsIdx++];
      int varEndOffset = dictionaryVarBounds[varBoundsIdx++];
      int varLength = varEndOffset - varBeginOffset;
      System.arraycopy(
          message,
          varBeginOffset,
          flattenedVars,
          flattenedVarsOffset,
          varLength
      );
      flattenedVarsOffset += varLength;
    }

    flattenedDictVars = new FlattenedByteArray(flattenedVars, flattenedVarsEndOffsets);
    return flattenedDictVars;
  }

  public long[] getEncodedVars() {
    return encodedVars;
  }

  /**
   * @return The encoded variables boxed into {@code Long} objects.
   */
  public Long[] getEncodedVarsAsBoxedLongs() {
    if (null == encodedVars) {
      return null;
    } else {
      return LongStream.of(encodedVars).boxed().toArray(Long[]::new);
    }
  }

  public byte[] getMessage() {
    return message;
  }
}
