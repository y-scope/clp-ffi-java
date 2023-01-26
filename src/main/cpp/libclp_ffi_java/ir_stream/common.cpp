#include "common.hpp"

// Project headers
#include "../common.hpp"
#include "../JavaException.hpp"

namespace libclp_ffi_java::ir_stream {
    jbyteArray copy_ir_buffer_to_java (JNIEnv* jni_env, const std::vector<int8_t>& ir_buffer) {
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
