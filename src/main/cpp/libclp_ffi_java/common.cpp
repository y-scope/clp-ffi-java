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
    template<>
    std::unique_ptr<
            jboolean,
            JavaPrimitiveArrayElementsDeleter<jbooleanArray, jboolean>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jbooleanArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jbooleanArray, jboolean>;
        return std::unique_ptr<jboolean, DeleterType>{
                jni_env->GetBooleanArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jbyte,
            JavaPrimitiveArrayElementsDeleter<jbyteArray, jbyte>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jbyteArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jbyteArray, jbyte>;
        return std::unique_ptr<jbyte, DeleterType>{
                jni_env->GetByteArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jchar,
            JavaPrimitiveArrayElementsDeleter<jcharArray, jchar>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jcharArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jcharArray, jchar>;
        return std::unique_ptr<jchar, DeleterType>{
                jni_env->GetCharArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jshort,
            JavaPrimitiveArrayElementsDeleter<jshortArray, jshort>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jshortArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jshortArray, jshort>;
        return std::unique_ptr<jshort, DeleterType>{
                jni_env->GetShortArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jint,
            JavaPrimitiveArrayElementsDeleter<jintArray, jint>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jintArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jintArray, jint>;
        return std::unique_ptr<jint, DeleterType>{
                jni_env->GetIntArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jlong,
            JavaPrimitiveArrayElementsDeleter<jlongArray, jlong>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jlongArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jlongArray, jlong>;
        return std::unique_ptr<jlong, DeleterType>{
                jni_env->GetLongArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jfloat,
            JavaPrimitiveArrayElementsDeleter<jfloatArray, jfloat>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jfloatArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jfloatArray, jfloat>;
        return std::unique_ptr<jfloat, DeleterType>{
                jni_env->GetFloatArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    template<>
    std::unique_ptr<
            jdouble,
            JavaPrimitiveArrayElementsDeleter<jdoubleArray, jdouble>
    > get_java_primitive_array_elements (JNIEnv* jni_env, jdoubleArray Java_array,
                                         jint release_mode)
    {
        using DeleterType = JavaPrimitiveArrayElementsDeleter<jdoubleArray, jdouble>;
        return std::unique_ptr<jdouble, DeleterType>{
                jni_env->GetDoubleArrayElements(Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}
        };
    }

    void validate_variable_handling_rule_versions (
            JNIEnv* jni_env,
            jbyteArray Java_variablesSchemaVersion,
            jbyteArray Java_variableEncodingMethodsVersion
    ) {
        // Validate the schemas version
        auto schema_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variablesSchemaVersion, JNI_ABORT);
        if (nullptr == schema_version_bytes) {
            return;
        }
        auto schema_version_bytes_length = jni_env->GetArrayLength(Java_variablesSchemaVersion);
        string_view schema_version(size_checked_pointer_cast<char>(schema_version_bytes.get()),
                                   schema_version_bytes_length);
        if (cVariablesSchemaVersion != schema_version) {
            JavaUnsupportedOperationException::throw_in_java(jni_env,
                    "Unsupported version for variables schema");
        }

        // Validate the encoding methods version
        auto encoding_methods_version_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_variableEncodingMethodsVersion, JNI_ABORT);
        if (nullptr == encoding_methods_version_bytes) {
            return;
        }
        auto encoding_methods_version_length =
                jni_env->GetArrayLength(Java_variableEncodingMethodsVersion);
        string_view encoding_methods_version(
                size_checked_pointer_cast<char>(encoding_methods_version_bytes.get()),
                encoding_methods_version_length);
        if (cVariableEncodingMethodsVersion != encoding_methods_version) {
            JavaUnsupportedOperationException::throw_in_java(jni_env,
                    "Unsupported version for variable encoding methods");
        }
    }
}
