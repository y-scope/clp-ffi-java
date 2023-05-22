// JNI
#include <com_yscope_clp_compressorfrontend_AbstractClpWildcardQueryEncoder.h>

// Project headers
#include "common.hpp"

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_AbstractClpWildcardQueryEncoder_setVariableHandlingRuleVersions
(
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_variablesSchemaVersion, jint variables_schema_version_len,
        jbyteArray Java_variableEncodingMethodsVersion, jint variable_encoding_methods_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    libclp_ffi_java::validate_variable_handling_rule_versions(jni_env, Java_variablesSchemaVersion,
                                                              variables_schema_version_len,
                                                              Java_variableEncodingMethodsVersion,
                                                              variable_encoding_methods_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}
