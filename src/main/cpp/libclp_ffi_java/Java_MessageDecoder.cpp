// NOTE: Throughout this file, size_checked_pointer_cast from
// jlong* -> encoded_variable_t* is necessary to resolve build errors on macOS.

// C++ standard libraries
#include <string>

// JNI
#include <com_yscope_clp_compressorfrontend_MessageDecoder.h>

// Project headers
#include "../submodules/clp/components/core/src/string_utils.hpp"
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "common.hpp"
#include "JavaException.hpp"

using ffi::decode_message;
using ffi::eight_byte_encoded_variable_t;
using ffi::VariablePlaceholder;
using ffi::wildcard_query_matches_any_encoded_var;
using libclp_ffi_java::cJSizeMax;
using libclp_ffi_java::get_java_primitive_array_elements;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;

// Local function prototypes
/**
 * See MessageDecoder::decodeMessageNative in Java
 * @param jni_env
 * @param Java_logtype
 * @param Java_allDictionaryVars
 * @param Java_dictionaryVarEndOffsets
 * @param Java_encodedVars
 * @return The decoded message
 */
static jbyteArray decode_message_native (JNIEnv* jni_env, jbyteArray Java_logtype,
                                         jbyteArray Java_allDictionaryVars,
                                         jintArray Java_dictionaryVarEndOffsets,
                                         jlongArray Java_encodedVars);

/**
 * Wrapper around wildcard_query_matches_any_encoded_var which
 * @tparam var_placeholder
 * @param jni_env
 * @param Java_wildcard_query
 * @param Java_logtype
 * @param Java_encoded_vars
 * @return true if a match was found, false otherwise
 */
template <VariablePlaceholder var_placeholder>
static bool jni_wildcard_query_matches_any_encoded_var (
        JNIEnv* jni_env, jbyteArray Java_wildcard_query, jbyteArray Java_logtype,
        jlongArray Java_encoded_vars);

template <VariablePlaceholder var_placeholder>
static bool jni_wildcard_query_matches_any_encoded_var (
        JNIEnv* jni_env, jbyteArray Java_wildcard_query, jbyteArray Java_logtype,
        jlongArray Java_encoded_vars)
{
    // Get logtype
    auto logtype_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_logtype, JNI_ABORT);
    auto logtype_length = jni_env->GetArrayLength(Java_logtype);
    string_view logtype(size_checked_pointer_cast<char>(logtype_bytes.get()), logtype_length);

    // Get wildcard query
    auto wildcard_query_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_wildcard_query, JNI_ABORT);
    auto wildcard_query_length = jni_env->GetArrayLength(Java_wildcard_query);
    string_view wildcard_query(size_checked_pointer_cast<char>(wildcard_query_bytes.get()),
                               wildcard_query_length);

    // Get encoded variables
    auto encoded_vars = get_java_primitive_array_elements<jlongArray, jlong>(
            jni_env, Java_encoded_vars, JNI_ABORT);
    auto encoded_vars_length = jni_env->GetArrayLength(Java_encoded_vars);

    try {
        return wildcard_query_matches_any_encoded_var<var_placeholder>(
                wildcard_query, logtype,
                size_checked_pointer_cast<eight_byte_encoded_variable_t>(encoded_vars.get()),
                encoded_vars_length
        );
    } catch (const ffi::EncodingException& e) {
        throw JavaIOException(__FILENAME__, __LINE__, jni_env, e.what());
    }
}

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_setVariableHandlingRuleVersions (
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

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_decodeMessageNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_logtype,
        jbyteArray Java_allDictionaryVars,
        jintArray Java_dictionaryVarEndOffsets,
        jlongArray Java_encodedVars
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return decode_message_native(jni_env, Java_logtype, Java_allDictionaryVars,
                                 Java_dictionaryVarEndOffsets, Java_encodedVars);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(nullptr)
}

JNIEXPORT jboolean JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_wildcardQueryMatchesAnyFloatVarNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_wildcard_query,
        jbyteArray Java_logtype,
        jlongArray Java_encoded_vars
) {
    return jni_wildcard_query_matches_any_encoded_var<VariablePlaceholder::Float>(
            jni_env, Java_wildcard_query, Java_logtype, Java_encoded_vars);
}

JNIEXPORT jboolean JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_wildcardQueryMatchesAnyIntVarNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_wildcard_query,
        jbyteArray Java_logtype,
        jlongArray Java_encoded_vars
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return jni_wildcard_query_matches_any_encoded_var<VariablePlaceholder::Integer>(
            jni_env, Java_wildcard_query, Java_logtype, Java_encoded_vars);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(false)
}

static jbyteArray decode_message_native (JNIEnv* jni_env, jbyteArray Java_logtype,
                                         jbyteArray Java_allDictionaryVars,
                                         jintArray Java_dictionaryVarEndOffsets,
                                         jlongArray Java_encodedVars)
{
    // Get logtype
    auto logtype_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(jni_env, Java_logtype,
                                                                              JNI_ABORT);
    auto logtype_length = jni_env->GetArrayLength(Java_logtype);
    string_view logtype(size_checked_pointer_cast<char>(logtype_bytes.get()), logtype_length);

    // Get dictionary variables
    auto all_dictionary_vars_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_allDictionaryVars, JNI_ABORT);
    auto all_dictionary_vars_length = jni_env->GetArrayLength(Java_allDictionaryVars);
    string_view all_dictionary_vars(
            size_checked_pointer_cast<char>(all_dictionary_vars_bytes.get()),
            all_dictionary_vars_length);

    auto dictionary_var_end_offsets = get_java_primitive_array_elements<jintArray, jint>(
            jni_env, Java_dictionaryVarEndOffsets, JNI_ABORT);
    auto dictionary_var_end_offsets_length = jni_env->GetArrayLength(Java_dictionaryVarEndOffsets);

    // Get encoded variables
    auto encoded_vars = get_java_primitive_array_elements<jlongArray, jlong>(
            jni_env, Java_encodedVars, JNI_ABORT);
    auto encoded_vars_length = jni_env->GetArrayLength(Java_encodedVars);

    try {
        auto message = decode_message(
                logtype,
                size_checked_pointer_cast<eight_byte_encoded_variable_t>(encoded_vars.get()),
                encoded_vars_length,
                all_dictionary_vars,
                dictionary_var_end_offsets.get(),
                dictionary_var_end_offsets_length
        );

        auto Java_message = libclp_ffi_java::new_java_primitive_array<jbyteArray, jbyte>(
                jni_env, size_checked_pointer_cast<jbyte>(message.data()), message.length());
        return Java_message;
    } catch (const ffi::EncodingException& e) {
        throw JavaIOException(__FILENAME__, __LINE__, jni_env, e.what());
    }
}
