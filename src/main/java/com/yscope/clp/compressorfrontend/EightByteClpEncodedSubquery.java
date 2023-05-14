package com.yscope.clp.compressorfrontend;

/**
 * A CLP-encoded subquery using the eight-byte encoding for variables.
 */
public class EightByteClpEncodedSubquery extends AbstractClpEncodedSubquery {
  private final long[] encodedVars;

  public EightByteClpEncodedSubquery (byte[] query, byte[] logtypeQuery,
                                      boolean logtypeQueryContainsWildcards, int[] dictVarBounds,
                                      long[] encodedVars, byte[] wildcardVarTypes,
                                      int[] wildcardVarBounds)
  {
    super(query, logtypeQuery, logtypeQueryContainsWildcards, dictVarBounds, wildcardVarTypes,
          wildcardVarBounds);
    this.encodedVars = encodedVars;
  }

  @Override
  public boolean containsVariables () {
    return super.containsVariables() || encodedVars.length > 0;
  }

  public long[] getEncodedVars () {
    return encodedVars;
  }
}
