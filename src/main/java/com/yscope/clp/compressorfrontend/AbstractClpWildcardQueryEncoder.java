package com.yscope.clp.compressorfrontend;

import java.nio.charset.StandardCharsets;
import java.util.Objects;

import org.jetbrains.annotations.NotNull;

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
      @NotNull String variablesSchemaVersion,
      @NotNull String variableEncodingMethodsVersion
  ) throws UnsupportedOperationException {
    Objects.requireNonNull(variablesSchemaVersion);
    Objects.requireNonNull(variableEncodingMethodsVersion);
    byte[] schemaVersionBytes = variablesSchemaVersion.getBytes(StandardCharsets.ISO_8859_1);
    byte[] encodingMethodsVersionBytes =
        variableEncodingMethodsVersion.getBytes(StandardCharsets.ISO_8859_1);
    setVariableHandlingRuleVersions(schemaVersionBytes, schemaVersionBytes.length,
                                    encodingMethodsVersionBytes,
                                    encodingMethodsVersionBytes.length);
  }

  private native void setVariableHandlingRuleVersions (
      byte[] variablesSchemaVersion, int variablesSchemaVersionLen,
      byte[] variableEncodingMethodsVersion, int variableEncodingMethodsVersionLen
  ) throws UnsupportedOperationException;
}
