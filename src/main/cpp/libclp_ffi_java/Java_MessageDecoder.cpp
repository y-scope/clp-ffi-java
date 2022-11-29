// C++ standard libraries
#include <string>

// JNI
#include <com_yscope_clp_compressorfrontend_MessageDecoder.h>

// Project headers
#include "../submodules/clp/components/core/src/string_utils.hpp"
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "JavaException.hpp"

using ffi::decode_message;
using libclp_ffi_java::JavaIOException;
using ffi::VariablePlaceholder;
using ffi::wildcard_query_matches_any_encoded_var;
using std::string_view;

/**
 * Wrapper around wildcard_query_matches_any_encoded_var which
 * @tparam var_placeholder
 * @param jni_env
 * @param Java_wildcard_query
 * @param Java_logtype
 * @param Java_encoded_vars
 * @return
 */
template <VariablePlaceholder var_placeholder>
bool jni_wildcard_query_matches_any_encoded_var (JNIEnv* jni_env, jbyteArray Java_wildcard_query,
                                                 jbyteArray Java_logtype,
                                                 jlongArray Java_encoded_vars);

template <VariablePlaceholder var_placeholder>
bool jni_wildcard_query_matches_any_encoded_var (JNIEnv* jni_env, jbyteArray Java_wildcard_query,
                                                 jbyteArray Java_logtype,
                                                 jlongArray Java_encoded_vars)
{
    // Get logtype
    auto logtype_bytes = jni_env->GetByteArrayElements(Java_logtype, nullptr);
    if (nullptr == logtype_bytes) {
        return false;
    }
    auto logtype_length = jni_env->GetArrayLength(Java_logtype);
    string_view logtype(reinterpret_cast<const char*>(logtype_bytes), logtype_length);

    // Get wildcard query
    auto wildcard_query_bytes = jni_env->GetByteArrayElements(Java_wildcard_query, nullptr);
    if (nullptr == wildcard_query_bytes) {
        return false;
    }
    auto wildcard_query_length = jni_env->GetArrayLength(Java_wildcard_query);
    string_view wildcard_query(reinterpret_cast<const char*>(wildcard_query_bytes),
                               wildcard_query_length);

    // Get encoded variables
    auto encoded_vars = jni_env->GetLongArrayElements(Java_encoded_vars, nullptr);
    if (nullptr == encoded_vars) {
        return false;
    }
    auto encoded_vars_length = jni_env->GetArrayLength(Java_encoded_vars);

    try {
        return wildcard_query_matches_any_encoded_var<var_placeholder>(
                wildcard_query, logtype, encoded_vars, encoded_vars_length);
    } catch (const ffi::EncodingException& e) {
        JavaIOException::throw_in_java(jni_env, e.what());
        return false;
    }
}

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_decodeMessageNative (
        JNIEnv* jni_env,
        jclass,
        jbyteArray Java_logtype,
        jbyteArray Java_allDictionaryVars,
        jintArray Java_dictionaryVarEndOffsets,
        jlongArray Java_encodedVars
) {
    // Get logtype
    auto logtype_bytes = jni_env->GetByteArrayElements(Java_logtype, nullptr);
    if (nullptr == logtype_bytes) {
        return nullptr;
    }
    auto logtype_length = jni_env->GetArrayLength(Java_logtype);
    string_view logtype(reinterpret_cast<const char*>(logtype_bytes), logtype_length);

    // Get dictionary variables
    auto all_dictionary_vars_bytes =
            jni_env->GetByteArrayElements(Java_allDictionaryVars, nullptr);
    if (nullptr == all_dictionary_vars_bytes) {
        return nullptr;
    }
    auto all_dictionary_vars_length = jni_env->GetArrayLength(Java_allDictionaryVars);
    string_view all_dictionary_vars(reinterpret_cast<const char*>(all_dictionary_vars_bytes),
                                    all_dictionary_vars_length);

    auto dictionary_var_end_offsets = jni_env->GetIntArrayElements(Java_dictionaryVarEndOffsets,
                                                                   nullptr);
    if (nullptr == dictionary_var_end_offsets) {
        return nullptr;
    }
    auto dictionary_var_end_offsets_length = jni_env->GetArrayLength(Java_dictionaryVarEndOffsets);

    // Get encoded variables
    auto encoded_vars = jni_env->GetLongArrayElements(Java_encodedVars, nullptr);
    if (nullptr == encoded_vars) {
        return nullptr;
    }
    auto encoded_vars_length = jni_env->GetArrayLength(Java_encodedVars);

    try {
        auto message = decode_message(logtype, encoded_vars, encoded_vars_length,
                                      all_dictionary_vars, dictionary_var_end_offsets,
                                      dictionary_var_end_offsets_length);

        auto message_bytes = jni_env->NewByteArray((int)message.length());
        if (nullptr == message_bytes) {
            return nullptr;
        }
        jni_env->SetByteArrayRegion(message_bytes, 0, (int)message.length(),
                                    reinterpret_cast<jbyte*>(message.data()));
        return message_bytes;
    } catch (const ffi::EncodingException& e) {
        JavaIOException::throw_in_java(jni_env, e.what());
        return nullptr;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_wildcardQueryMatchesAnyFloatVarNative (
        JNIEnv* jni_env,
        jclass,
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
        jclass,
        jbyteArray Java_wildcard_query,
        jbyteArray Java_logtype,
        jlongArray Java_encoded_vars
) {
    return jni_wildcard_query_matches_any_encoded_var<VariablePlaceholder::Integer>(
            jni_env, Java_wildcard_query, Java_logtype, Java_encoded_vars);
}
