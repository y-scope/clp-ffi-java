package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;

/**
 * Class to encode wildcard queries so that they can be used to search
 * CLP-encoded messages.
 */
public abstract class AbstractClpWildcardQueryEncoder {
  static {
    NativeLibraryLoader.load();
  }

  /**
   * @param variablesSchemaVersion The version of the variables schema to use to
   *                               parse the query.
   * @param variableEncodingMethodsVersion The version of variable encoding
   *                                       methods to use to encode the query.
   * @throws UnsupportedOperationException if either version was unknown or
   * unsupported.
   */
  public AbstractClpWildcardQueryEncoder (
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
}
