#ifndef LIBCLP_FFI_JAVA_STATIC_INIT_HPP
#define LIBCLP_FFI_JAVA_STATIC_INIT_HPP

#include <jni.h>

namespace libclp_ffi_java {
    extern jclass Java_EightByteClpEncodedSubQuery;
    extern jmethodID Java_EightByteClpEncodedSubQuery_init;
    extern jclass Java_EncodedMessage;
    extern jfieldID Java_EncodedMessage_logtype;
    extern jfieldID Java_EncodedMessage_dictVarBounds;
    extern jfieldID Java_EncodedMessage_encodedVars;
}

#endif //LIBCLP_FFI_JAVA_STATIC_INIT_HPP
