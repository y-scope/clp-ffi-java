// NOTE: Throughout this file...
// - size_checked_pointer_cast from jbyte* (signed char*) to char* should be
//   safe since we are comparing the values against ASCII rather than doing
//   arithmetic.
// - size_checked_pointer_cast from encoded_variable_t* -> jlong* is
//   necessary to resolve build errors on macOS.

// C++ standard libraries
#include <memory>
#include <string>
#include <vector>

// CLP
#include "../submodules/clp/components/core/src/Defs.h"
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"

// JNI
#include <com_yscope_clp_compressorfrontend_MessageEncoder.h>

// Project headers
#include "common.hpp"
#include "GeneralException.hpp"
#include "JavaException.hpp"
#include "static_init.hpp"

using ffi::encode_message;
using libclp_ffi_java::get_java_primitive_array_elements;
using libclp_ffi_java::Java_EncodedMessage;
using libclp_ffi_java::Java_EncodedMessage_dictVarBounds;
using libclp_ffi_java::Java_EncodedMessage_encodedVars;
using libclp_ffi_java::Java_EncodedMessage_logtype;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::new_java_primitive_array;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;
using std::string;
using std::vector;

// Local function prototypes
/**
 * See MessageEncoder::encodeMessageNative in Java
 * @param jni_env
 * @param Java_message
 * @param Java_encodedMessage
 */
static void encode_message_native (JNIEnv* jni_env, jbyteArray Java_message,
                                   jobject Java_encodedMessage);

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_MessageEncoder_setVariableHandlingRuleVersions (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_variablesSchemaVersion, jint variables_schema_version_len,
        jbyteArray Java_variableEncodingMethodsVersion, jint variable_encoding_methods_len
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    libclp_ffi_java::validate_variable_handling_rule_versions(jni_env, Java_variablesSchemaVersion,
                                                              variables_schema_version_len,
                                                              Java_variableEncodingMethodsVersion,
                                                              variable_encoding_methods_len);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}

JNIEXPORT void JNICALL Java_com_yscope_clp_compressorfrontend_MessageEncoder_encodeMessageNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_message,
        jobject Java_encodedMessage
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    encode_message_native(jni_env, Java_message, Java_encodedMessage);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END()
}

static void encode_message_native (JNIEnv* jni_env, jbyteArray Java_message,
                                   jobject Java_encodedMessage)
{
    // Get the message
    auto message_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
            jni_env, Java_message, JNI_ABORT);
    auto message_length = jni_env->GetArrayLength(Java_message);
    string_view message(size_checked_pointer_cast<char>(message_bytes.get()), message_length);

    string logtype;
    vector<encoded_variable_t> encoded_vars;
    vector<int32_t> dictionary_var_bounds;
    if (false == encode_message(message, logtype, encoded_vars, dictionary_var_bounds)) {
        throw JavaIOException(__FILENAME__, __LINE__, jni_env,
                              "message contains variable placeholders");
    }

    // Set encodedMessage.logtype
    auto Java_logtype = new_java_primitive_array<jbyteArray, jbyte>(
            jni_env, size_checked_pointer_cast<jbyte>(logtype.data()), logtype.length());
    jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_logtype, Java_logtype);

    // Set encodedMessage.dictionaryVarBounds
    if (false == dictionary_var_bounds.empty()) {
        auto Java_dictVarBounds = new_java_primitive_array<jintArray, jint>(
                jni_env, size_checked_pointer_cast<jint>(dictionary_var_bounds.data()),
                dictionary_var_bounds.size());
        jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_dictVarBounds,
                                Java_dictVarBounds);
    }

    // Set encodedMessage.encodedVars
    if (false == encoded_vars.empty()) {
        auto Java_encodedVars = new_java_primitive_array<jlongArray, jlong>(
                jni_env, size_checked_pointer_cast<jlong>(encoded_vars.data()),
                encoded_vars.size());
        jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_encodedVars,
                                Java_encodedVars);
    }
}
