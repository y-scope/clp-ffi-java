// C++ standard libraries
#include <string_view>

// CLP
#include "../submodules/clp/components/core/src/ffi/ir_stream/encoding_methods.hpp"
#include "../submodules/clp/components/core/src/type_utils.hpp"

// JNI
#include <com_yscope_clp_irstream_EightByteClpIrOutputStream.h>

// Project headers
#include "ClpIrOutputStreamState.hpp"
#include "common.hpp"
#include "ir_stream/common.hpp"
#include "JavaException.hpp"

using ffi::ir_stream::eight_byte_encoding::encode_message;
using ffi::ir_stream::eight_byte_encoding::encode_preamble;
using libclp_ffi_java::cJSizeMax;
using libclp_ffi_java::ClpIrOutputStreamState;
using libclp_ffi_java::ir_stream::encode_log_event;
using libclp_ffi_java::JavaIOException;
using libclp_ffi_java::JavaRuntimeException;
using libclp_ffi_java::size_checked_pointer_cast;
using std::string_view;

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_irstream_EightByteClpIrOutputStream_encodePreambleNative (
        JNIEnv* jni_env,
        jobject,
        jlong stream_state_address,
        jbyteArray Java_timestampPattern,
        jint timestamp_pattern_length,
        jbyteArray Java_timestampPatternSyntax,
        jint timestamp_pattern_syntax_length,
        jbyteArray Java_timeZoneId,
        jint time_zone_id_length
) {
    return libclp_ffi_java::ir_stream::encode_preamble<ffi::eight_byte_encoded_variable_t>(
            jni_env,
            stream_state_address,
            Java_timestampPattern,
            timestamp_pattern_length,
            Java_timestampPatternSyntax,
            timestamp_pattern_syntax_length,
            Java_timeZoneId,
            time_zone_id_length,
            0
    );
}

JNIEXPORT jbyteArray JNICALL
Java_com_yscope_clp_irstream_EightByteClpIrOutputStream_encodeLogEventNative (
        JNIEnv* jni_env,
        jobject,
        jlong stream_state_address,
        jlong timestamp,
        jbyteArray Java_message,
        jint message_length
) {
    return encode_log_event<ffi::eight_byte_encoded_variable_t>(
            jni_env, stream_state_address, timestamp, Java_message, message_length);
}
