#ifndef LIBCLP_FFI_JAVA_COMMON_HPP
#define LIBCLP_FFI_JAVA_COMMON_HPP

// JNI
#include <jni.h>

namespace libclp_ffi_java {
    /**
     * Validates that the given version names for the variables schema and
     * variable encoding methods match the versions currently used by this
     * library.
     * @param jni_env
     * @param Java_variablesSchemaVersion
     * @param Java_variableEncodingMethodsVersion
     */
    void validate_variable_handling_rule_versions (
            JNIEnv* jni_env,
            jbyteArray Java_variablesSchemaVersion,
            jbyteArray Java_variableEncodingMethodsVersion
    );
}

#endif // LIBCLP_FFI_JAVA_COMMON_HPP
