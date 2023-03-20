#ifndef LIBCLP_FFI_JAVA_IR_STREAM_COMMON_TPP
#define LIBCLP_FFI_JAVA_IR_STREAM_COMMON_TPP

// C++ standard libraries
#include <cstdint>
#include <vector>
#include <type_traits>

// JNI
#include <jni.h>

// Project headers
#include "../../submodules/clp/components/core/src/type_utils.hpp"
#include "../../submodules/clp/components/core/src/ffi/encoding_methods.hpp"
#include "../../submodules/clp/components/core/src/ffi/ir_stream/encoding_methods.hpp"
#include "../ClpIrOutputStreamState.hpp"
#include "../common.hpp"
#include "../JavaException.hpp"
#include "../JavaPrimitiveArrayElementsDeleter.hpp"

namespace libclp_ffi_java::ir_stream {
    // Local function prototypes
    /**
     * Copies the given IR buffer to Java and returns a reference to it.
     * NOTE: On failure, callers should return to Java without calling any other
     * JNI methods.
     * @param ir_buffer
     * @param jni_env
     * @return Reference to the Java buffer on success, nullptr otherwise.
     */
    static jbyteArray copy_ir_buffer_to_java (const std::vector<int8_t>& ir_buffer,
                                              JNIEnv* jni_env);

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
    ) {
        if (timestamp_pattern_length < 0 || timestamp_pattern_syntax_length < 0 ||
            time_zone_id_length < 0)
        {
            JavaRuntimeException::throw_in_java(jni_env,
                                                "[native] Byte array lengths cannot be negative.");
            return nullptr;
        }

        // Get the timestamp pattern
        auto timestamp_pattern_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_timestampPattern, JNI_ABORT);
        if (nullptr == timestamp_pattern_bytes) {
            return nullptr;
        }
        std::string_view timestamp_pattern{
                size_checked_pointer_cast<char>(timestamp_pattern_bytes.get()),
                static_cast<size_t>(timestamp_pattern_length)
        };

        // Get the timestamp pattern syntax
        auto timestamp_pattern_syntax_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_timestampPatternSyntax, JNI_ABORT);
        if (nullptr == timestamp_pattern_syntax_bytes) {
            return nullptr;
        }
        std::string_view timestamp_pattern_syntax{
                size_checked_pointer_cast<char>(timestamp_pattern_syntax_bytes.get()),
                static_cast<size_t>(timestamp_pattern_syntax_length)
        };

        // Get the time zone ID
        auto time_zone_id_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_timeZoneId, JNI_ABORT);
        if (nullptr == time_zone_id_bytes) {
            return nullptr;
        }
        std::string_view time_zone_id{size_checked_pointer_cast<char>(time_zone_id_bytes.get()),
                                 static_cast<size_t>(time_zone_id_length)};

        auto stream_state = reinterpret_cast<ClpIrOutputStreamState*>(
                bit_cast<uintptr_t>(stream_state_address));
        auto& ir_buffer = stream_state->ir_buffer;
        ir_buffer.clear();
        bool encoding_successful;
        static_assert(std::is_same_v<encoded_variable_t, ffi::eight_byte_encoded_variable_t> ||
                      std::is_same_v<encoded_variable_t, ffi::four_byte_encoded_variable_t>);
        if constexpr (std::is_same_v<encoded_variable_t, ffi::eight_byte_encoded_variable_t>) {
            encoding_successful = ffi::ir_stream::eight_byte_encoding::encode_preamble(
                    timestamp_pattern, timestamp_pattern_syntax, time_zone_id, ir_buffer
            );
        } else {  // std::is_same_v<encoded_variable_t, ffi::four_byte_encoded_variable_t>
            encoding_successful = ffi::ir_stream::four_byte_encoding::encode_preamble(
                    timestamp_pattern,
                    timestamp_pattern_syntax,
                    time_zone_id,
                    reference_timestamp,
                    ir_buffer
            );
        }
        if (false == encoding_successful)
        {
            JavaIOException::throw_in_java(jni_env, "Failed to encode preamble.");
            return nullptr;
        }

        return libclp_ffi_java::ir_stream::copy_ir_buffer_to_java(ir_buffer, jni_env);
    }

    template <typename encoded_variable_t>
    jbyteArray encode_log_event (JNIEnv* jni_env, jlong stream_state_address,
                                 jlong timestamp_or_timestamp_delta, jbyteArray Java_message,
                                 jint message_length)
    {
        if (message_length < 0) {
            JavaRuntimeException::throw_in_java(jni_env,
                                                "[native] Byte array lengths cannot be negative.");
            return nullptr;
        }

        // Get the message
        auto message_bytes = get_java_primitive_array_elements<jbyteArray, jbyte>(
                jni_env, Java_message, JNI_ABORT);
        if (nullptr == message_bytes) {
            JavaIOException::throw_in_java(jni_env, "[native] Failed to get message.");
            return nullptr;
        }
        std::string_view message{size_checked_pointer_cast<char>(message_bytes.get()),
                            static_cast<size_t>(message_length)};

        auto stream_state = reinterpret_cast<ClpIrOutputStreamState*>(
                bit_cast<uintptr_t>(stream_state_address));
        auto& logtype = stream_state->logtype;
        auto& ir_buffer = stream_state->ir_buffer;
        ir_buffer.clear();
        bool encoding_successful;
        static_assert(std::is_same_v<encoded_variable_t, ffi::eight_byte_encoded_variable_t> ||
                      std::is_same_v<encoded_variable_t, ffi::four_byte_encoded_variable_t>);
        if constexpr (std::is_same_v<encoded_variable_t, ffi::eight_byte_encoded_variable_t>) {
            encoding_successful = ffi::ir_stream::eight_byte_encoding::encode_message(
                    timestamp_or_timestamp_delta, message, logtype, ir_buffer
            );
        } else {  // std::is_same_v<encoded_variable_t, ffi::four_byte_encoded_variable_t>
            encoding_successful = ffi::ir_stream::four_byte_encoding::encode_message(
                    timestamp_or_timestamp_delta, message, logtype, ir_buffer
            );
        }
        if (false == encoding_successful) {
            JavaIOException::throw_in_java(jni_env, "Failed to encode message.");
            return nullptr;
        }

        return copy_ir_buffer_to_java(ir_buffer, jni_env);
    }

    static jbyteArray copy_ir_buffer_to_java (const std::vector<int8_t>& ir_buffer,
                                              JNIEnv* jni_env)
    {
        if (ir_buffer.size() > cJSizeMax) {
            JavaIOException::throw_in_java(jni_env, "Encoded IR data too long for byte array.");
            return nullptr;
        }
        auto Java_ir_buffer = jni_env->NewByteArray(static_cast<jsize>(ir_buffer.size()));
        if (nullptr == Java_ir_buffer) {
            JavaIOException::throw_in_java(jni_env, "[native] Failed to allocate return buffer.");
            return nullptr;
        }
        jni_env->SetByteArrayRegion(Java_ir_buffer, 0, static_cast<jsize>(ir_buffer.size()),
                                    size_checked_pointer_cast<const jbyte>(ir_buffer.data()));
        auto exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            JavaIOException::throw_in_java(jni_env,
                                           "[native] Failed to set return buffer content.");
            return nullptr;
        }
        return Java_ir_buffer;
    }
}

#endif //LIBCLP_FFI_JAVA_IR_STREAM_COMMON_TPP
