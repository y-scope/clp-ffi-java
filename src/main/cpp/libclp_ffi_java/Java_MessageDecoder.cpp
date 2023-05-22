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
using ffi::wildcard_match_encoded_vars;
using ffi::wildcard_query_matches_any_encoded_var;
using libclp_ffi_java::cJSizeMax;
using libclp_ffi_java::get_java_primitive_array_elements;
using libclp_ffi_java::JavaExceptionOccurred;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::JavaRuntimeException;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;

// Local function prototypes
/**
 * See MessageDecoder::batchEncodedVarsWildcardMatchNative in Java
 * @param jni_env
 * @param num_messages
 * @param Java_logtypes
 * @param Java_encodedVarArrays
 * @param Java_wildcardVarPlaceholders
 * @param wildcard_var_placeholders_len
 * @param Java_serializedVarWildcardQueries
 * @param serialized_var_wildcard_queries_len
 * @param Java_varWildcardQueryEndIndexes
 * @param var_wildcard_query_end_indexes_len
 * @param Java_matchResults
 */
static void batch_encoded_vars_wildcard_match_native (
        JNIEnv* jni_env,
        jint num_messages,
        jobjectArray Java_logtypes,
        jobjectArray Java_encodedVarArrays,
        jbyteArray Java_wildcardVarPlaceholders,
        jint wildcard_var_placeholders_len,
        jbyteArray Java_serializedVarWildcardQueries,
        jint serialized_var_wildcard_queries_len,
        jintArray Java_varWildcardQueryEndIndexes,
        jint var_wildcard_query_end_indexes_len,
        jintArray Java_matchResults
);

/**
 * See MessageDecoder::decodeMessageNative in Java
 * @param jni_env
 * @param Java_logtype
 * @param logtype_len
 * @param Java_allDictionaryVars
 * @param all_dictionary_vars_len
 * @param Java_dictionaryVarEndOffsets
 * @param Java_encodedVars
 * @param encoded_vars_len
 * @return The decoded message
 */
static jbyteArray decode_message_native (JNIEnv* jni_env, jbyteArray Java_logtype,
                                         jint logtype_len, jbyteArray Java_allDictionaryVars,
                                         jint all_dictionary_vars_len,
                                         jintArray Java_dictionaryVarEndOffsets,
                                         jlongArray Java_encodedVars, jint encoded_vars_len);

/**
 * Wrapper around wildcard_query_matches_any_encoded_var which
 * @tparam var_placeholder
 * @param jni_env
 * @param Java_wildcardQuery
 * @param wildcard_query_len
 * @param Java_logtype
 * @param logtype_len
 * @param Java_encodedVars
 * @param encoded_vars_len
 * @return true if a match was found, false otherwise
 */
template <VariablePlaceholder var_placeholder>
static bool jni_wildcard_query_matches_any_encoded_var (
        JNIEnv* jni_env, jbyteArray Java_wildcardQuery, jint wildcard_query_len,
        jbyteArray Java_logtype, jint logtype_len, jlongArray Java_encodedVars,
        jint encoded_vars_len);

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_setVariableHandlingRuleVersions (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_variablesSchemaVersion,
        jint variables_schema_version_len,
        jbyteArray Java_variableEncodingMethodsVersion,
        jint variable_encoding_methods_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    libclp_ffi_java::validate_variable_handling_rule_versions(jni_env, Java_variablesSchemaVersion,
                                                              variables_schema_version_len,
                                                              Java_variableEncodingMethodsVersion,
                                                              variable_encoding_methods_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_decodeMessageNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_logtype,
        jint logtype_len,
        jbyteArray Java_allDictionaryVars,
        jint all_dictionary_vars_len,
        jintArray Java_dictionaryVarEndOffsets,
        jlongArray Java_encodedVars,
        jint encoded_vars_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return decode_message_native(jni_env, Java_logtype, logtype_len, Java_allDictionaryVars,
                                 all_dictionary_vars_len, Java_dictionaryVarEndOffsets,
                                 Java_encodedVars, encoded_vars_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(nullptr)
}

JNIEXPORT jboolean JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_wildcardQueryMatchesAnyFloatVarNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_wildcardQuery,
        jint wildcard_query_len,
        jbyteArray Java_logtype,
        jint logtype_len,
        jlongArray Java_encodedVars,
        jint encoded_vars_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return jni_wildcard_query_matches_any_encoded_var<VariablePlaceholder::Float>(
            jni_env, Java_wildcardQuery, wildcard_query_len, Java_logtype, logtype_len,
            Java_encodedVars, encoded_vars_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(false)
}

JNIEXPORT jboolean JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_wildcardQueryMatchesAnyIntVarNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_wildcardQuery,
        jint wildcard_query_len,
        jbyteArray Java_logtype,
        jint logtype_len,
        jlongArray Java_encodedVars,
        jint encoded_vars_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return jni_wildcard_query_matches_any_encoded_var<VariablePlaceholder::Integer>(
            jni_env, Java_wildcardQuery, wildcard_query_len, Java_logtype, logtype_len,
            Java_encodedVars, encoded_vars_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(false)
}

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_MessageDecoder_batchEncodedVarsWildcardMatchNative (
        JNIEnv* jni_env,
        jobject,
        jint num_messages,
        jobjectArray Java_logtypes,
        jobjectArray Java_encodedVarArrays,
        jbyteArray Java_wildcardVarPlaceholders,
        jint wildcard_var_placeholders_len,
        jbyteArray Java_serializedVarWildcardQueries,
        jint serialized_var_wildcard_queries_len,
        jintArray Java_varWildcardQueryEndIndexes,
        jint var_wildcard_query_end_indexes_len,
        jintArray Java_matchResults
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return batch_encoded_vars_wildcard_match_native(jni_env,
                                                    num_messages,
                                                    Java_logtypes,
                                                    Java_encodedVarArrays,
                                                    Java_wildcardVarPlaceholders,
                                                    wildcard_var_placeholders_len,
                                                    Java_serializedVarWildcardQueries,
                                                    serialized_var_wildcard_queries_len,
                                                    Java_varWildcardQueryEndIndexes,
                                                    var_wildcard_query_end_indexes_len,
                                                    Java_matchResults);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}

static void batch_encoded_vars_wildcard_match_native (
        JNIEnv* jni_env,
        jint num_messages,
        jobjectArray Java_logtypes,
        jobjectArray Java_encodedVarArrays,
        jbyteArray Java_wildcardVarPlaceholders,
        jint wildcard_var_placeholders_len,
        jbyteArray Java_serializedVarWildcardQueries,
        jint serialized_var_wildcard_queries_len,
        jintArray Java_varWildcardQueryEndIndexes,
        jint var_wildcard_query_end_indexes_len,
        jintArray Java_matchResults
) {
    // Get serialized variable types
    auto wildcard_var_placeholders = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_wildcardVarPlaceholders, JNI_ABORT);
    string_view wildcard_var_types{
            size_checked_pointer_cast<char>(wildcard_var_placeholders.get()),
            static_cast<size_t>(wildcard_var_placeholders_len)};

    // Get serialized variable queries
    auto serialized_var_wildcard_queries_bytes =
            get_java_primitive_array_elements<jbyteArray, jbyte>(
                    jni_env, Java_serializedVarWildcardQueries, JNI_ABORT);
    string_view serialized_var_wildcard_queries{
            size_checked_pointer_cast<char>(serialized_var_wildcard_queries_bytes.get()),
            static_cast<size_t>(serialized_var_wildcard_queries_len)};

    // Get variable query end indexes
    auto var_wildcard_query_end_indexes_uniq_ptr =
            get_java_primitive_array_elements<jintArray, jint>(
                    jni_env, Java_varWildcardQueryEndIndexes, JNI_ABORT);
    auto var_wildcard_query_end_indexes = var_wildcard_query_end_indexes_uniq_ptr.get();

    // Deserialize variable queries
    std::vector<string_view> var_wildcard_queries;
    size_t query_begin_idx = 0;
    for (int i = 0; i < var_wildcard_query_end_indexes_len; ++i) {
        auto end_idx = static_cast<size_t>(var_wildcard_query_end_indexes[i]);
        var_wildcard_queries.emplace_back(&serialized_var_wildcard_queries[query_begin_idx],
                                          end_idx - query_begin_idx);
        query_begin_idx = end_idx;
    }

    // Get match results array
    auto match_results_uniq_ptr =
            get_java_primitive_array_elements<jintArray, jint>(jni_env, Java_matchResults, 0);
    auto match_results = size_checked_pointer_cast<int>(match_results_uniq_ptr.get());

    // Do the matching
    // NOTE: The downcasts from jobject to jlongArray in the loop below are safe
    for (int i = 0; i < num_messages; ++i) {
        // Get encoded variables
        auto Java_encodedVars = static_cast<jlongArray>(
                jni_env->GetObjectArrayElement(Java_encodedVarArrays, i));
        auto exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "GetObjectArrayElement failed");
        }
        if (nullptr == Java_encodedVars) {
            // No encoded variables in message
            match_results[i] = 0;
            continue;
        }
        auto encoded_vars_len = jni_env->GetArrayLength(Java_encodedVars);
        auto encoded_vars = get_java_primitive_array_elements<jlongArray, jlong>(jni_env,
                                                                                 Java_encodedVars,
                                                                                 JNI_ABORT);

        // Get logtype
        auto Java_logtype = static_cast<jbyteArray>(
                jni_env->GetObjectArrayElement(Java_logtypes, i));
        exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "GetObjectArrayElement failed");
        }
        auto logtype_len = nullptr == Java_logtype ? 0 : jni_env->GetArrayLength(Java_logtype);
        auto logtype_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(jni_env,
                                                                                  Java_logtype,
                                                                                  JNI_ABORT);
        string_view logtype{size_checked_pointer_cast<char>(logtype_bytes.get()),
                            static_cast<size_t>(logtype_len)};

        try {
            match_results[i] = wildcard_match_encoded_vars(
                    logtype,
                    size_checked_pointer_cast<eight_byte_encoded_variable_t>(encoded_vars.get()),
                    encoded_vars_len,
                    wildcard_var_types,
                    var_wildcard_queries);
        } catch (const ffi::EncodingException& e) {
            throw JavaIOException(__FILENAME__, __LINE__, jni_env, e.what());
        }

        logtype_bytes.reset(nullptr);
        jni_env->DeleteLocalRef(Java_logtype);
        encoded_vars.reset(nullptr);
        jni_env->DeleteLocalRef(Java_encodedVars);
    }
}

static jbyteArray decode_message_native (JNIEnv* jni_env, jbyteArray Java_logtype,
                                         jint logtype_len, jbyteArray Java_allDictionaryVars,
                                         jint all_dictionary_vars_len,
                                         jintArray Java_dictionaryVarEndOffsets,
                                         jlongArray Java_encodedVars, jint encoded_vars_len)
{
    // Get logtype
    auto logtype_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(jni_env, Java_logtype,
                                                                              JNI_ABORT);
    string_view logtype{size_checked_pointer_cast<char>(logtype_bytes.get()),
                        static_cast<size_t>(logtype_len)};

    // Get dictionary variables
    auto all_dictionary_vars_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_allDictionaryVars, JNI_ABORT);
    string_view all_dictionary_vars{
            size_checked_pointer_cast<char>(all_dictionary_vars_bytes.get()),
            static_cast<size_t>(all_dictionary_vars_len)};

    auto dictionary_var_end_offsets = get_java_primitive_array_elements<jintArray, jint>(
            jni_env, Java_dictionaryVarEndOffsets, JNI_ABORT);

    // Get encoded variables
    auto encoded_vars = get_java_primitive_array_elements<jlongArray, jlong>(
            jni_env, Java_encodedVars, JNI_ABORT);

    try {
        auto message = decode_message(
                logtype,
                size_checked_pointer_cast<eight_byte_encoded_variable_t>(encoded_vars.get()),
                encoded_vars_len,
                all_dictionary_vars,
                dictionary_var_end_offsets.get(),
                all_dictionary_vars_len
        );

        auto Java_message = libclp_ffi_java::new_java_primitive_array<jbyteArray, jbyte>(
                jni_env, size_checked_pointer_cast<jbyte>(message.data()), message.length());
        return Java_message;
    } catch (const ffi::EncodingException& e) {
        throw JavaIOException(__FILENAME__, __LINE__, jni_env, e.what());
    }
}

template <VariablePlaceholder var_placeholder>
static bool jni_wildcard_query_matches_any_encoded_var (
        JNIEnv* jni_env, jbyteArray Java_wildcardQuery, jint wildcard_query_len,
        jbyteArray Java_logtype, jint logtype_len, jlongArray Java_encodedVars,
        jint encoded_vars_len)
{
    // Get logtype
    auto logtype_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_logtype, JNI_ABORT);
    string_view logtype{size_checked_pointer_cast<char>(logtype_bytes.get()),
                        static_cast<size_t>(logtype_len)};

    // Get wildcard query
    auto wildcard_query_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_wildcardQuery, JNI_ABORT);
    string_view wildcard_query{size_checked_pointer_cast<char>(wildcard_query_bytes.get()),
                               static_cast<size_t>(wildcard_query_len)};

    // Get encoded variables
    auto encoded_vars = get_java_primitive_array_elements<jlongArray, jlong>(
            jni_env, Java_encodedVars, JNI_ABORT);

    try {
        return wildcard_query_matches_any_encoded_var<var_placeholder>(
                wildcard_query, logtype,
                size_checked_pointer_cast<eight_byte_encoded_variable_t>(encoded_vars.get()),
                encoded_vars_len
        );
    } catch (const ffi::EncodingException& e) {
        throw JavaIOException(__FILENAME__, __LINE__, jni_env, e.what());
    }
}
