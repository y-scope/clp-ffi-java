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
#include "JavaException.hpp"

using ffi::encode_message;
using libclp_ffi_java::get_java_primitive_array_elements;
using libclp_ffi_java::JavaClassNotFoundException;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::JavaRuntimeException;
using libclp_ffi_java::new_java_primitive_array;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;
using std::string;
using std::vector;

// Constants
constexpr jint cRequiredJNIVersion = JNI_VERSION_1_6;

// Globals
static jclass Java_EncodedMessage = nullptr;
static jfieldID Java_EncodedMessage_logtype;
static jfieldID Java_EncodedMessage_dictVarBounds;
static jfieldID Java_EncodedMessage_encodedVars;

// Local function prototypes
/**
 * Caches the field IDs for the Java EncodedMessage class
 * @param jni_env
 * @return true on success, false otherwise
 */
static bool cache_java_encoded_message_field_ids (JNIEnv* jni_env);
/**
 * Gets a Java global reference (persists between JNI calls) to the Java class
 * with the given signature
 * @param jni_env
 * @param class_signature
 * @return The reference
 */
static jclass get_class_global_ref (JNIEnv* jni_env, const char* class_signature);

/**
 * See MessageEncoder::encodeMessageNative in Java
 * @param jni_env
 * @param Java_message
 * @param Java_encodedMessage
 */
static void encode_message_native (JNIEnv* jni_env, jbyteArray Java_message,
                                   jobject Java_encodedMessage);

static bool cache_java_encoded_message_field_ids (JNIEnv* jni_env) {
    // Get Java class fields
    // NOTE: GetFieldID already throws Java exceptions, so we don't need to
    Java_EncodedMessage_logtype = jni_env->GetFieldID(Java_EncodedMessage, "logtype", "[B");
    if (nullptr == Java_EncodedMessage_logtype) {
        return false;
    }
    Java_EncodedMessage_encodedVars =
            jni_env->GetFieldID(Java_EncodedMessage, "encodedVars", "[J");
    if (nullptr == Java_EncodedMessage_encodedVars) {
        return false;
    }
    Java_EncodedMessage_dictVarBounds =
            jni_env->GetFieldID(Java_EncodedMessage, "dictionaryVarBounds", "[I");
    if (nullptr == Java_EncodedMessage_dictVarBounds) {
        return false;
    }

    return true;
}

static jclass get_class_global_ref (JNIEnv* jni_env, const char* class_signature) {
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

JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void*) {
    JNIEnv* jni_env;
    // Based on the JDK 11 JNI docs, JNI 1.6 should be sufficient for all
    // JNI methods except those related to modules
    if (vm->GetEnv(reinterpret_cast<void**>(&jni_env), cRequiredJNIVersion) != JNI_OK) {
        return JNI_ERR;
    }

    // Cache JNI objects
    try {
        Java_EncodedMessage = get_class_global_ref(jni_env,
                "com/yscope/clp/compressorfrontend/EncodedMessage");
    } catch (libclp_ffi_java::JavaException& e) {
        return JNI_ERR;
    }
    if (cache_java_encoded_message_field_ids(jni_env) == false) {
        jni_env->DeleteGlobalRef(Java_EncodedMessage);
        Java_EncodedMessage = nullptr;
        return JNI_ERR;
    }

    return cRequiredJNIVersion;
}

JNIEXPORT void JNICALL JNI_OnUnload (JavaVM* vm, void*) {
    JNIEnv* jni_env;
    if (vm->GetEnv(reinterpret_cast<void**>(&jni_env), cRequiredJNIVersion) != JNI_OK) {
        // Unexpected error, but nothing we can do
        return;
    }

    jni_env->DeleteGlobalRef(Java_EncodedMessage);
    Java_EncodedMessage = nullptr;
}

JNIEXPORT void JNICALL
Java_com_yscope_clp_compressorfrontend_MessageEncoder_setVariableHandlingRuleVersions (
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

void encode_message_native (JNIEnv* jni_env, jbyteArray Java_message,
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
