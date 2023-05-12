#include "common.hpp"

// C++ standard libraries
#include <string_view>

// Project headers
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "JavaException.hpp"

using ffi::cVariableEncodingMethodsVersion;
using ffi::cVariablesSchemaVersion;
using std::string_view;

namespace libclp_ffi_java {
    void validate_variable_handling_rule_versions (
            JNIEnv* jni_env,
            jbyteArray Java_variablesSchemaVersion,
            jbyteArray Java_variableEncodingMethodsVersion
    ) {
        // Validate the schemas version
        auto schema_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variablesSchemaVersion, JNI_ABORT);
        auto schema_version_bytes_length = jni_env->GetArrayLength(Java_variablesSchemaVersion);
        string_view schema_version(size_checked_pointer_cast<char>(schema_version_bytes.get()),
                                   schema_version_bytes_length);
        if (cVariablesSchemaVersion != schema_version) {
            throw JavaUnsupportedOperationException(__FILENAME__, __LINE__, jni_env,
                                                    "Unsupported version for variables schema");
        }

        // Validate the encoding methods version
        auto encoding_methods_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variableEncodingMethodsVersion, JNI_ABORT);
        auto encoding_methods_version_length =
                jni_env->GetArrayLength(Java_variableEncodingMethodsVersion);
        string_view encoding_methods_version(
                size_checked_pointer_cast<char>(encoding_methods_version_bytes.get()),
                encoding_methods_version_length);
        if (cVariableEncodingMethodsVersion != encoding_methods_version) {
            throw JavaUnsupportedOperationException(
                    __FILENAME__, __LINE__, jni_env,
                    "Unsupported version for variable encoding methods");
        }
    }
}
