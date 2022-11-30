// NOTE: Throughout this file, the reinterpret_cast from jbyte* (signed char*)
// to char* should be safe since we are comparing the values against ASCII
// rather than doing arithmetic.

// C++ standard libraries
#include <algorithm>
#include <cctype>
#include <climits>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// JNI
#include <com_yscope_clp_compressorfrontend_MessageEncoder.h>

// Project headers
#include "../submodules/clp/components/core/src/Defs.h"
#include "../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "JavaException.hpp"

using ffi::encode_message;
using libclp_ffi_java::JavaClassNotFoundException;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::JavaRuntimeException;
using std::string_view;
using std::string;
using std::unique_ptr;
using std::vector;

jclass Java_EncodedMessage;
jfieldID Java_EncodedMessage_logtype;
jfieldID Java_EncodedMessage_dictVarBounds;
jfieldID Java_EncodedMessage_encodedVars;

unique_ptr<string> logtype;
unique_ptr<vector<encoded_variable_t>> encoded_vars;
unique_ptr<vector<int32_t>> dictionary_var_bounds;

/**
 * Gets a Java global reference (persists between JNI calls) to the Java class
 * with the given signature
 * @param jni_env
 * @param class_signature
 * @return The reference
 */
static jclass get_class_global_ref (JNIEnv* jni_env, const char* class_signature);

static jclass get_class_global_ref (JNIEnv* jni_env, const char* class_signature) {
    auto local_class_ref = jni_env->FindClass(class_signature);
    if (nullptr == local_class_ref) {
        throw JavaClassNotFoundException(__FILENAME__, __LINE__, jni_env, class_signature);
    }
    auto global_class_ref = static_cast<jclass>(jni_env->NewGlobalRef(local_class_ref));
    if (nullptr == global_class_ref) {
        jni_env->DeleteLocalRef(local_class_ref);
        throw JavaRuntimeException(__FILENAME__, __LINE__, jni_env, "NewGlobalRef failed");
    }
    jni_env->DeleteLocalRef(local_class_ref);

    return global_class_ref;
}

JNIEXPORT void JNICALL Java_com_yscope_clp_compressorfrontend_MessageEncoder_init (
        JNIEnv* jni_env,
        jclass
) {
    // Get Java classes
    try {
        Java_EncodedMessage =
                get_class_global_ref(jni_env, "com/yscope/clp/compressorfrontend/EncodedMessage");
    } catch (const libclp_ffi_java::JavaException& e) {
        return;
    }

    // Get Java class fields
    // NOTE: GetFieldID already throws Java exceptions, so we don't need to
    Java_EncodedMessage_logtype = jni_env->GetFieldID(Java_EncodedMessage, "logtype", "[B");
    if (nullptr == Java_EncodedMessage_logtype) {
        return;
    }
    Java_EncodedMessage_encodedVars =
            jni_env->GetFieldID(Java_EncodedMessage, "encodedVars", "[J");
    if (nullptr == Java_EncodedMessage_encodedVars) {
        return;
    }
    Java_EncodedMessage_dictVarBounds =
            jni_env->GetFieldID(Java_EncodedMessage, "dictionaryVarBounds", "[I");
    if (nullptr == Java_EncodedMessage_dictVarBounds) {
        return;
    }

    // Initialize globals
    logtype = std::make_unique<string>();
    encoded_vars = std::make_unique<vector<encoded_variable_t>>();
    dictionary_var_bounds = std::make_unique<vector<int32_t>>();
}

JNIEXPORT void JNICALL Java_com_yscope_clp_compressorfrontend_MessageEncoder_encodeMessageNative (
        JNIEnv* jni_env,
        jobject,
        jbyteArray Java_message,
        jobject Java_encodedMessage
) {
    // Get the message
    auto message_bytes = jni_env->GetByteArrayElements(Java_message, nullptr);
    if (nullptr == message_bytes) {
        return;
    }
    auto message_length = jni_env->GetArrayLength(Java_message);
    string_view message(reinterpret_cast<const char*>(message_bytes), message_length);

    if (false == encode_message(message, *logtype, *encoded_vars, *dictionary_var_bounds)) {
        JavaIOException::throw_in_java(jni_env, "message contains variable placeholders");
        return;
    }

    // Set encodedMessage.logtype
    auto Java_logtype = jni_env->NewByteArray(logtype->length());
    if (nullptr == Java_logtype) {
        return;
    }
    jni_env->SetByteArrayRegion(Java_logtype, 0, logtype->length(),
                                reinterpret_cast<signed char*>(logtype->data()));
    auto exception = jni_env->ExceptionOccurred();
    if (nullptr != exception) {
        return;
    }
    jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_logtype, Java_logtype);
    logtype->clear();

    // Set encodedMessage.dictionaryVarBounds
    if (false == dictionary_var_bounds->empty()) {
        auto java_dict_var_bounds = jni_env->NewIntArray(dictionary_var_bounds->size());
        jni_env->SetIntArrayRegion(java_dict_var_bounds, 0, dictionary_var_bounds->size(),
                                   dictionary_var_bounds->data());
        jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_dictVarBounds,
                                java_dict_var_bounds);
        dictionary_var_bounds->clear();
    }

    // Set encodedMessage.encodedVars
    if (false == encoded_vars->empty()) {
        auto java_encoded_variables = jni_env->NewLongArray(encoded_vars->size());
        jni_env->SetLongArrayRegion(java_encoded_variables, 0, encoded_vars->size(),
                                    bit_cast<const jlong*>(encoded_vars->data()));
        jni_env->SetObjectField(Java_encodedMessage, Java_EncodedMessage_encodedVars,
                                java_encoded_variables);
        encoded_vars->clear();
    }
}
