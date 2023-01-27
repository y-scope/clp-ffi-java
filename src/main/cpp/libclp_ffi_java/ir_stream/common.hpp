#ifndef LIBCLP_FFI_JAVA_IR_STREAM_COMMON_HPP
#define LIBCLP_FFI_JAVA_IR_STREAM_COMMON_HPP

// C++ standard libraries
#include <cstdint>
#include <vector>

// JNI
#include <jni.h>

namespace libclp_ffi_java::ir_stream {
    /**
     * Encodes the preamble into the IR stream format determined by the given
     * encoded variable type.
     * NOTE: On failure, callers should return to Java without calling any other
     * JNI methods.
     * @tparam encoded_variable_t Type of the encoded variables used with this
     * IR stream
     * @param jni_env
     * @param stream_state_address
     * @param Java_timestampPattern
     * @param timestamp_pattern_length
     * @param Java_timestampPatternSyntax
     * @param timestamp_pattern_syntax_length
     * @param Java_timeZoneId
     * @param time_zone_id_length
     * @param reference_timestamp For the four-byte encoding, the timestamp
     * that the first log message's timestamp delta is calculated from. For the
     * eight-byte encoding, this is unused.
     * @return A reference to the encoded preamble on success, nullptr
     * otherwise.
     */
    template <typename encoded_variable_t>
    jbyteArray encode_preamble (
            JNIEnv* jni_env,
            jlong stream_state_address,
            jbyteArray Java_timestampPattern,
            jint timestamp_pattern_length,
            jbyteArray Java_timestampPatternSyntax,
            jint timestamp_pattern_syntax_length,
            jbyteArray Java_timeZoneId,
            jint time_zone_id_length,
            jlong reference_timestamp
    );

    /**
     * Encodes the log event into the IR stream format determined by the given
     * encoded variable type.
     * NOTE: On failure, callers should return to Java without calling any other
     * JNI methods.
     * @tparam encoded_variable_t Type of the encoded variables used with this
     * IR stream
     * @param jni_env
     * @param stream_state_address
     * @param timestamp_or_timestamp_delta
     * @param Java_message
     * @param message_length
     * @return A reference to the encoded log event on success, nullptr
     * otherwise.
     */
    template <typename encoded_variable_t>
    jbyteArray encode_log_event (JNIEnv* jni_env, jlong stream_state_address,
                                 jlong timestamp_or_timestamp_delta, jbyteArray Java_message,
                                 jint message_length);
}

#include "common.tpp"

#endif //LIBCLP_FFI_JAVA_IR_STREAM_COMMON_HPP
