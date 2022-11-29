package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.stream.LongStream;

public class EncodedMessage {
  // These are public so that they can be accessed in native code without too
  // many method calls
  public byte[] logtype;
  public long[] encodedVars;
  public int[] dictionaryVarBounds;

  private byte[] message;

  /**
   * Sets the message to which this encoded message corresponds.
   * This allows the encoded message to store dictionary variables as bounds
   * within the original message rather than as new strings
   * @param message
   */
  public void setMessage(byte[] message) {
    this.message = message;

    // Clear the encoded values since they no longer match
    logtype = null;
    encodedVars = null;
    dictionaryVarBounds = null;
  }

  public String getLogTypeAsString() {
    if (null == logtype) {
      return null;
    } else {
      return new String(logtype, StandardCharsets.ISO_8859_1);
    }
  }

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

  public Long[] getEncodedVarsAsBoxedLongs() {
    if (null == encodedVars) {
      return null;
    } else {
      return LongStream.of(encodedVars).boxed().toArray(Long[]::new);
    }
  }
}
