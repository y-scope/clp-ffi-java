package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.stream.LongStream;
import org.jetbrains.annotations.NotNull;


/**
 * A minimal base class for specialized encoded message types, such as {@code DecodingOptimizedEncodedMessage}.
 *
 * <p>This class serves as the data boundary between Java and native (JNI) code, holding the raw message bytes
 * and its essential components, including the log type, encoded variables, and dictionary variable bounds.</p>
 */
public class EncodedMessage {
  // These are protected so that they can be accessed in native code without too
  // many method calls
  protected byte[] logtype;
  protected long[] encodedVars;
  protected int[] dictionaryVarBounds;

  protected byte[] message;

  /**
   * Sets the message to which this encoded message corresponds.
   * This allows the encoded message to store dictionary variables as bounds
   * within the original message rather than as new strings
   * @param message
   */
  public void setMessage(byte @NotNull [] message) {
    this.message = message;

    // Clear the encoded values since they no longer match
    logtype = null;
    encodedVars = null;
    dictionaryVarBounds = null;
  }

  /** Convenient but inefficient function to get the log type as a string */
  public String getLogTypeAsString() {
    if (null == logtype) {
      return null;
    } else {
      return new String(logtype, StandardCharsets.ISO_8859_1);
    }
  }

  /** Convenient but inefficient function to get the dictionary vars as a byteArray of string */
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

  /** Convenient but inefficient function to get the encoded vars as a byteArray of Long */
  public Long[] getEncodedVarsAsBoxedLongs() {
    if (null == encodedVars) {
      return null;
    } else {
      return LongStream.of(encodedVars).boxed().toArray(Long[]::new);
    }
  }

  public long[] getEncodedVars() {
    return encodedVars;
  }
}
