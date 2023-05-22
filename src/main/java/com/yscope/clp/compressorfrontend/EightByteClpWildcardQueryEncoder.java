package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.Objects;

import org.jetbrains.annotations.NotNull;

/**
 * Derived version of {@link AbstractClpWildcardQueryEncoder} specifically using
 * the eight-byte encoding for variables.
 */
public class EightByteClpWildcardQueryEncoder extends AbstractClpWildcardQueryEncoder {
  /**
   * @see AbstractClpWildcardQueryEncoder#AbstractClpWildcardQueryEncoder
   */
  public EightByteClpWildcardQueryEncoder (
      @NotNull String variablesSchemaVersion,
      @NotNull String variableEncodingMethodsVersion
  ) throws UnsupportedOperationException {
    super(variablesSchemaVersion, variableEncodingMethodsVersion);
  }

  /**
   * Encodes the given wildcard query into an array of subqueries that can be
   * run on encoded messages. The array of subqueries is a logical disjunction
   * (i.e., all subqueries OR-ed together).
   * @param wildcardQuery The wildcard query using two wildcards:
   * <ul>
   *   <li>'?' matching any single character</li>
   *   <li>'*' matching 0 or more characters</li>
   * </ul>
   * @return The subqueries
   */
  public EightByteClpEncodedSubquery[] encode (
      @NotNull String wildcardQuery
  ) throws IllegalArgumentException {
    Objects.requireNonNull(wildcardQuery);
    byte[] wildcardQueryBytes = wildcardQuery.getBytes(StandardCharsets.ISO_8859_1);
    return encodeNative(wildcardQueryBytes, wildcardQueryBytes.length);
  }

  /**
   * Same as {@link EightByteClpWildcardQueryEncoder#encode} except
   * {@code wildcardQuery} is a byte array.
   * @param wildcardQuery
   * @param wildcardQueryLength
   * @return The subqueries
   */
  private native EightByteClpEncodedSubquery[] encodeNative (
      byte[] wildcardQuery,
      int wildcardQueryLength
  ) throws IllegalArgumentException;
}
