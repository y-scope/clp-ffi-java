// C++ standard libraries
#include <string_view>
#include <variant>
#include <vector>

// JNI
#include <com_yscope_clp_compressorfrontend_EightByteClpWildcardQueryEncoder.h>

// Project headers
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "../submodules/clp/components/core/src/ffi/search/ExactVariableToken.hpp"
#include "../submodules/clp/components/core/src/ffi/search/query_methods.hpp"
#include "../submodules/clp/components/core/src/ffi/search/QueryMethodFailed.hpp"
#include "../submodules/clp/components/core/src/ffi/search/WildcardToken.hpp"
#include "../submodules/clp/components/core/src/string_utils.hpp"
#include "common.hpp"
#include "GeneralException.hpp"
#include "JavaException.hpp"
#include "static_init.hpp"

using ffi::eight_byte_encoded_variable_t;
using ffi::search::ExactVariableToken;
using ffi::search::Subquery;
using ffi::search::WildcardToken;
using libclp_ffi_java::GeneralException;
using libclp_ffi_java::get_java_primitive_array_elements;
using libclp_ffi_java::Java_EightByteClpEncodedSubQuery_init;
using libclp_ffi_java::Java_EightByteClpEncodedSubQuery;
using libclp_ffi_java::JavaExceptionOccurred;
using libclp_ffi_java::JavaIllegalArgumentException;
using libclp_ffi_java::JavaRuntimeException;
using libclp_ffi_java::JavaUnsupportedOperationException;
using libclp_ffi_java::new_java_primitive_array;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;
using std::string;
using std::variant;
using std::vector;

using EightByteExactVariableToken = ExactVariableToken<eight_byte_encoded_variable_t>;
using EightByteWildcardToken = WildcardToken<eight_byte_encoded_variable_t>;

// Local function prototypes
/**
 * See EightByteClpWildcardQueryEncoder::encodeNative in Java
 * @param jni_env
 * @param Java_wildcardQuery
 * @param wildcard_query_length
 * @return The encoded subqueries
 */
static jobjectArray encode_native (JNIEnv* jni_env, jbyteArray Java_wildcardQuery,
                                   jint wildcard_query_length);

JNIEXPORT jobjectArray JNICALL
Java_com_yscope_clp_compressorfrontend_EightByteClpWildcardQueryEncoder_encodeNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_wildcardQuery,
        jint wildcard_query_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return encode_native(jni_env, Java_wildcardQuery, wildcard_query_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(nullptr)
}

static jobjectArray encode_native (
        JNIEnv* jni_env,
        jbyteArray Java_wildcardQuery,
        jint wildcard_query_length
) {
    // Get the message
    auto wildcard_query_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_wildcardQuery, JNI_ABORT);
    string_view wildcard_query{size_checked_pointer_cast<char>(wildcard_query_bytes.get()),
                               static_cast<size_t>(wildcard_query_length)};

    // Sanitize the wildcard query
    auto clean_wildcard_query = clean_up_wildcard_search_string(wildcard_query);
    auto Java_cleanWildcardQuery = new_java_primitive_array<jbyteArray>(
            jni_env, size_checked_pointer_cast<jbyte>(clean_wildcard_query.data()),
            clean_wildcard_query.size());

    // Generate the subqueries
    vector<Subquery<eight_byte_encoded_variable_t>> subqueries;
    try {
        ffi::search::generate_subqueries(clean_wildcard_query, subqueries);
    } catch (const ffi::search::QueryMethodFailed& e) {
        throw JavaIllegalArgumentException(__FILENAME__, __LINE__, jni_env, e.what());
    }
    if (subqueries.size() > libclp_ffi_java::cJSizeMax) {
        throw JavaUnsupportedOperationException(__FILENAME__, __LINE__, jni_env,
                                                "Subqueries can't fit in a Java array");
    }
    auto Java_subqueries = jni_env->NewObjectArray(static_cast<jsize>(subqueries.size()),
                                                   Java_EightByteClpEncodedSubQuery, nullptr);
    auto exception = jni_env->ExceptionOccurred();
    if (nullptr != exception) {
        throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                    "[native] Failed to allocate array in JVM for subqueries");
    }
    if (nullptr == Java_subqueries) {
        throw GeneralException(ErrorCode_Failure, __FILENAME__, __LINE__,
                               "[native] Failed to allocate array in JVM for subqueries");
    }

    vector<int32_t> dict_var_bounds;
    vector<eight_byte_encoded_variable_t> encoded_vars;
    vector<int8_t> wildcard_var_placeholders;
    vector<int32_t> wildcard_var_bounds;
    for (size_t i = 0; i < subqueries.size(); ++i) {
        const auto& sub_query = subqueries[i];
        const auto& logtype_query = sub_query.get_logtype_query();
        const auto& query_vars = sub_query.get_query_vars();

        // NOTE: Although the original query length is a jsize, the logtype
        // length may be longer if the variable placeholders take up more space
        // than the variables they replace. This doesn't happen at the time of
        // writing, but it may in the future.
        auto Java_logtypeQuery = new_java_primitive_array<jbyteArray, jbyte>(
                jni_env, size_checked_pointer_cast<const jbyte>(logtype_query.data()),
                logtype_query.length());

        dict_var_bounds.clear();
        encoded_vars.clear();
        wildcard_var_placeholders.clear();
        wildcard_var_bounds.clear();
        for (const auto& query_var : query_vars) {
            auto success = std::visit(overloaded{
                    [&] (const EightByteExactVariableToken& var) {
                        if (ffi::VariablePlaceholder::Dictionary == var.get_placeholder()) {
                            // These static casts are safe since the original query
                            // length is a jsize
                            dict_var_bounds.push_back(
                                    static_cast<int32_t>(var.get_begin_pos()));
                            dict_var_bounds.push_back(
                                    static_cast<int32_t>(var.get_end_pos()));
                        } else {
                            encoded_vars.push_back(var.get_encoded_value());
                        }
                        return true;
                    },
                    [&] (const EightByteWildcardToken& var) {
                        switch (var.get_current_interpretation()) {
                            case ffi::search::TokenType::IntegerVariable:
                                wildcard_var_placeholders.push_back(
                                        enum_to_underlying_type(
                                                ffi::VariablePlaceholder::Integer));
                                break;
                            case ffi::search::TokenType::FloatVariable:
                                wildcard_var_placeholders.push_back(
                                        enum_to_underlying_type(
                                                ffi::VariablePlaceholder::Float));
                                break;
                            case ffi::search::TokenType::DictionaryVariable:
                                wildcard_var_placeholders.push_back(
                                        enum_to_underlying_type(
                                                ffi::VariablePlaceholder::Dictionary));
                                break;
                            default:
                                // This should never happen
                                return false;
                        }
                        // These static casts are safe since the original query
                        // length is a jsize
                        wildcard_var_bounds.push_back(
                                static_cast<int32_t>(var.get_begin_pos()));
                        wildcard_var_bounds.push_back(static_cast<int32_t>(var.get_end_pos()));
                        return true;
                    }
            }, query_var);
            if (false == success) {
                throw JavaRuntimeException(__FILENAME__, __LINE__, jni_env,
                        "[native] Unexpected exception when serializing subqueries.");
            }
        }

        auto Java_dictVarBounds = new_java_primitive_array<jintArray, jint>(
                jni_env, size_checked_pointer_cast<const jint>(dict_var_bounds.data()),
                dict_var_bounds.size());

        auto Java_encodedVars = new_java_primitive_array<jlongArray, jlong>(
                jni_env, size_checked_pointer_cast<const jlong>(encoded_vars.data()),
                encoded_vars.size());

        auto Java_wildcardVarPlaceholders = new_java_primitive_array<jbyteArray, jbyte>(
                jni_env,
                size_checked_pointer_cast<const jbyte>(wildcard_var_placeholders.data()),
                wildcard_var_placeholders.size());

        auto Java_varWildcardQueryBounds = new_java_primitive_array<jintArray, jint>(
                jni_env, size_checked_pointer_cast<const jint>(wildcard_var_bounds.data()),
                wildcard_var_bounds.size());

        auto Java_subquery = jni_env->NewObject(
                Java_EightByteClpEncodedSubQuery, Java_EightByteClpEncodedSubQuery_init,
                Java_cleanWildcardQuery, Java_logtypeQuery,
                sub_query.logtype_query_contains_wildcards(), Java_dictVarBounds,
                Java_encodedVars, Java_wildcardVarPlaceholders, Java_varWildcardQueryBounds);
        exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "[native] Failed to create subquery in JVM");
        }
        if (nullptr == Java_subquery) {
            throw GeneralException(ErrorCode_Failure, __FILENAME__, __LINE__,
                                   "[native] Failed to create subquery in JVM");
        }
        jni_env->SetObjectArrayElement(Java_subqueries, static_cast<jsize>(i), Java_subquery);
        exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "Failed to assign subquery to array in JVM");
        }

        // Delete local references so we don't overwhelm the JVM
        jni_env->DeleteLocalRef(Java_logtypeQuery);
        jni_env->DeleteLocalRef(Java_dictVarBounds);
        jni_env->DeleteLocalRef(Java_encodedVars);
        jni_env->DeleteLocalRef(Java_wildcardVarPlaceholders);
        jni_env->DeleteLocalRef(Java_varWildcardQueryBounds);
        jni_env->DeleteLocalRef(Java_subquery);
    }

    return Java_subqueries;
}
