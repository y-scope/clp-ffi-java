package com.yscope.clp.compressorfrontend;

import java.io.ByteArrayOutputStream;
import org.jetbrains.annotations.NotNull;

import static org.apache.commons.lang3.ArrayUtils.EMPTY_LONG_ARRAY;


/**
 * This class extends EncodedMessage to provide a more CLP encoded data in a datastructures that minimizes
 * serialization and deserialization overhead during CLP message decoding.
 */
public class DecodingOptimizedEncodedMessage extends EncodedMessage {
  protected final ByteArrayOutputStream flattenedDictionaryVarsBytesOutputStream = new ByteArrayOutputStream();
  protected FlattenedByteArray dictionaryVars;

  @Override
  public void setMessage(byte @NotNull [] message) {
    super.setMessage(message);
    dictionaryVars = null;
    flattenedDictionaryVarsBytesOutputStream.reset();
  }

  /**
   * Retrieves the raw message as a byte array
   * @return a byte array representing the logtype
   */
  public byte @NotNull [] getRawMessage() {
    return message;
  }

  /**
   * Retrieves the log type of the encoded message as a byte array
   * @return a byte array representing the logtype
   */
  public byte @NotNull [] getLogType() {
    return logtype;
  }

  public @NotNull FlattenedByteArray getFlattenedDictionaryVars() {
    // Return a cached object when possible
    return (null != dictionaryVars) ? dictionaryVars : (dictionaryVars = computeDictionaryVars());
  }

  /**
   * Generates a new {@link FlattenedByteArray} object containing the dictionary variables
   * extracted from the current message. The dictionary variables are stored in a flattened
   * byte array, with their corresponding end offsets tracked in the returned object.
   *
   * <p>If no dictionary variables exist (i.e., the {@code dictionaryVarBounds} array is
   * {@code null} or empty), an empty {@link FlattenedByteArray} instance is returned.</p>
   *
   * @return a {@link FlattenedByteArray} object containing the dictionary variables,
   *         or an empty instance if none are present
   */
  private @NotNull FlattenedByteArray computeDictionaryVars() {
    // If there's no dictionary variables, return empty flattened byte array
    if (null == dictionaryVarBounds || dictionaryVarBounds.length == 0) {
      return FlattenedByteArray.EMPTY_FLATTENED_BYTE_ARRAY;
    }

    int numVars = dictionaryVarBounds.length / 2;
    int[] flattenedDictionaryVarEndOffsets = new int[numVars];
    for (int i = 0, j = 0; i < numVars; ++i) {
      int beginOffset = dictionaryVarBounds[j++];
      int endOffset = dictionaryVarBounds[j++];
      flattenedDictionaryVarsBytesOutputStream.write(message, beginOffset, endOffset - beginOffset);
      flattenedDictionaryVarEndOffsets[i] = flattenedDictionaryVarsBytesOutputStream.size();
    }

    return new FlattenedByteArray(flattenedDictionaryVarsBytesOutputStream.toByteArray(),
        flattenedDictionaryVarEndOffsets);
  }

  /**
   * Retrieves the encoded vars of the encoded message as a long array
   * @return a byte array representing the logtype
   */
  @Override
  public long @NotNull [] getEncodedVars() {
    return (null != encodedVars) ? encodedVars : EMPTY_LONG_ARRAY;
  }
}
