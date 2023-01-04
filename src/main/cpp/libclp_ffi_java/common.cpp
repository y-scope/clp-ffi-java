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
        auto schema_version_bytes = jni_env->GetByteArrayElements(Java_variablesSchemaVersion,
                                                                  nullptr);
        if (nullptr == schema_version_bytes) {
            return;
        }
        auto schema_version_bytes_length = jni_env->GetArrayLength(Java_variablesSchemaVersion);
        string_view schema_version(reinterpret_cast<const char*>(schema_version_bytes),
                                   schema_version_bytes_length);
        if (cVariablesSchemaVersion != schema_version) {
            JavaUnsupportedOperationException::throw_in_java(jni_env,
                    "Unsupported version for variables schema");
        }

        // Validate the encoding methods version
        auto encoding_methods_version_bytes =
                jni_env->GetByteArrayElements(Java_variableEncodingMethodsVersion, nullptr);
        if (nullptr == encoding_methods_version_bytes) {
            return;
        }
        auto encoding_methods_version_length =
                jni_env->GetArrayLength(Java_variableEncodingMethodsVersion);
        string_view encoding_methods_version(
                reinterpret_cast<const char*>(encoding_methods_version_bytes),
                encoding_methods_version_length);
        if (cVariableEncodingMethodsVersion != encoding_methods_version) {
            JavaUnsupportedOperationException::throw_in_java(jni_env,
                    "Unsupported version for variable encoding methods");
        }
    }
}
