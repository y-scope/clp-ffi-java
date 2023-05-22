#include "common.hpp"

// C++ standard libraries
#include <string_view>

// Project headers
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "../submodules/clp/components/core/src/type_utils.hpp"
#include "JavaException.hpp"

using ffi::cVariableEncodingMethodsVersion;
using ffi::cVariablesSchemaVersion;
using std::string_view;

namespace libclp_ffi_java {
    jclass get_class_global_ref (JNIEnv* jni_env, const char* class_signature) {
        auto local_class_ref = jni_env->FindClass(class_signature);
        if (nullptr == local_class_ref) {
            throw JavaClassNotFoundException(__FILENAME__, __LINE__, jni_env, class_signature);
        }
        auto global_class_ref = reinterpret_cast<jclass>(jni_env->NewGlobalRef(local_class_ref));
        if (nullptr == global_class_ref) {
            jni_env->DeleteLocalRef(local_class_ref);
            throw JavaRuntimeException(__FILENAME__, __LINE__, jni_env, "NewGlobalRef failed");
        }
        jni_env->DeleteLocalRef(local_class_ref);

        return global_class_ref;
    }

    void validate_variable_handling_rule_versions (
            JNIEnv* jni_env,
            jbyteArray Java_variablesSchemaVersion,
            jint variables_schema_version_len,
            jbyteArray Java_variableEncodingMethodsVersion,
            jint variable_encoding_methods_len
    ) {
        // Validate the schemas version
        auto schema_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variablesSchemaVersion, JNI_ABORT);
        string_view schema_version(size_checked_pointer_cast<char>(schema_version_bytes.get()),
                                   variables_schema_version_len);
        if (cVariablesSchemaVersion != schema_version) {
            throw JavaUnsupportedOperationException(__FILENAME__, __LINE__, jni_env,
                                                    "Unsupported version for variables schema");
        }

        // Validate the encoding methods version
        auto encoding_methods_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variableEncodingMethodsVersion, JNI_ABORT);
        string_view encoding_methods_version(
                size_checked_pointer_cast<char>(encoding_methods_version_bytes.get()),
                variable_encoding_methods_len);
        if (cVariableEncodingMethodsVersion != encoding_methods_version) {
            throw JavaUnsupportedOperationException(__FILENAME__, __LINE__, jni_env,
                                                    "Unsupported version for variable encoding"
                                                    " methods");
        }
    }
}
