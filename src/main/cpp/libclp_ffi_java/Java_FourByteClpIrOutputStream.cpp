// CLP
#include "../submodules/clp/components/core/src/ffi/ir_stream/encoding_methods.hpp"

// JNI
#include <com_yscope_clp_irstream_FourByteClpIrOutputStream.h>

// Project headers
#include "GeneralException.hpp"
#include "ir_stream/common.hpp"

using libclp_ffi_java::ir_stream::encode_log_event;
using libclp_ffi_java::ir_stream::encode_preamble;

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_irstream_FourByteClpIrOutputStream_encodePreambleNative (
        JNIEnv* jni_env,
        jobject,
        jlong stream_state_address,
        jbyteArray Java_timestampPattern,
        jint timestamp_pattern_length,
        jbyteArray Java_timestampPatternSyntax,
        jint timestamp_pattern_syntax_length,
        jbyteArray Java_timeZoneId,
        jint time_zone_id_length,
        jlong reference_timestamp
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    return encode_preamble<ffi::four_byte_encoded_variable_t>(
            jni_env,
            stream_state_address,
            Java_timestampPattern,
            timestamp_pattern_length,
            Java_timestampPatternSyntax,
            timestamp_pattern_syntax_length,
            Java_timeZoneId,
            time_zone_id_length,
            reference_timestamp
    );
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(nullptr)
}

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_irstream_FourByteClpIrOutputStream_encodeLogEventNative (
        JNIEnv* jni_env,
        jobject,
        jlong stream_state_address,
        jlong timestamp_delta,
        jbyteArray Java_message,
        jint message_length
) {
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN()
    // NOTE: Although encode_log_event takes `timestamp` as a parameter rather
    // than `timestamp_delta`, they are the same type and encode_log_event just
    // passes it through
    return encode_log_event<ffi::four_byte_encoded_variable_t>(
            jni_env, stream_state_address, timestamp_delta, Java_message, message_length);
    LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(nullptr)
}
