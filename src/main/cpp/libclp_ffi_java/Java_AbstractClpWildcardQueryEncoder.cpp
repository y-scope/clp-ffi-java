// JNI
#include <com_yscope_clp_compressorfrontend_AbstractClpWildcardQueryEncoder.h>

// Project headers
#include "common.hpp"

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_AbstractClpWildcardQueryEncoder_setVariableHandlingRuleVersions
(
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_variablesSchemaVersion,
        jbyteArray Java_variableEncodingMethodsVersion
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    libclp_ffi_java::validate_variable_handling_rule_versions(jni_env, Java_variablesSchemaVersion,
                                                              Java_variableEncodingMethodsVersion);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}
