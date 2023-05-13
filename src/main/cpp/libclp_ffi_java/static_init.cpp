#include "static_init.hpp"

// JNI
#include <jni.h>

// Project headers
#include "common.hpp"
#include "JavaException.hpp"

using libclp_ffi_java::Java_EncodedMessage;
using libclp_ffi_java::Java_EightByteClpEncodedSubQuery;
using libclp_ffi_java::Java_EightByteClpEncodedSubQuery_init;
using libclp_ffi_java::Java_EncodedMessage_logtype;
using libclp_ffi_java::Java_EncodedMessage_dictVarBounds;
using libclp_ffi_java::Java_EncodedMessage_encodedVars;

// Globals with external linkage
namespace libclp_ffi_java {
    jclass Java_EightByteClpEncodedSubQuery = nullptr;
    jmethodID Java_EightByteClpEncodedSubQuery_init;
    jclass Java_EncodedMessage = nullptr;
    jfieldID Java_EncodedMessage_logtype;
    jfieldID Java_EncodedMessage_dictVarBounds;
    jfieldID Java_EncodedMessage_encodedVars;
}

// Constants
static constexpr jint cRequiredJNIVersion = JNI_VERSION_1_6;

// Local function prototypes
/**
 * Caches the field IDs for the Java EncodedMessage class
 * @param jni_env
 * @return true on success, false otherwise
 */
static bool cache_java_encoded_message_field_ids (JNIEnv* jni_env);

static bool cache_java_encoded_message_field_ids (JNIEnv* jni_env) {
    // NOTE: GetFieldID already throws Java exceptions, so we don't need to

    // Get Java class fields and methods
    Java_EightByteClpEncodedSubQuery_init = jni_env->GetMethodID(Java_EightByteClpEncodedSubQuery,
                                                                 "<init>", "([B[BZ[I[J[B[I)V");
    if (nullptr == Java_EightByteClpEncodedSubQuery_init) {
        return false;
    }
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

JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* vm, void*) {
    JNIEnv* jni_env;
    // Based on the JDK 11 JNI docs, JNI 1.6 should be sufficient for all
    // JNI methods except those related to modules
    if (vm->GetEnv(reinterpret_cast<void**>(&jni_env), cRequiredJNIVersion) != JNI_OK) {
        return JNI_ERR;
    }

    // Cache JNI objects
    try {
        Java_EightByteClpEncodedSubQuery = libclp_ffi_java::get_class_global_ref(
                jni_env, "com/yscope/clp/compressorfrontend/EightByteClpEncodedSubquery");
        Java_EncodedMessage = libclp_ffi_java::get_class_global_ref(
                jni_env, "com/yscope/clp/compressorfrontend/EncodedMessage");
    } catch (libclp_ffi_java::JavaException& e) {
        return JNI_ERR;
    }
    if (cache_java_encoded_message_field_ids(jni_env) == false) {
        jni_env->DeleteGlobalRef(Java_EightByteClpEncodedSubQuery);
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

    jni_env->DeleteGlobalRef(Java_EightByteClpEncodedSubQuery);
    Java_EightByteClpEncodedSubQuery = nullptr;
    jni_env->DeleteGlobalRef(Java_EncodedMessage);
    Java_EncodedMessage = nullptr;
}
