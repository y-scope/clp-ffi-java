#ifndef LIBCLP_FFI_JAVA_COMMON_TPP
#define LIBCLP_FFI_JAVA_COMMON_TPP

// C++ standard libraries
#include <functional>

// Project headers
#include "JavaException.hpp"
#include "GeneralException.hpp"

namespace libclp_ffi_java {
    // Local function prototypes
    /**
     * Copies \p buf_len elements of \p buf to the Java array \p Java_array,
     * starting at \p begin_pos. Callers must ensure \p begin_pos and \p
     * buf_len can fit in sizeof(jsize).
     * @tparam JavaArrayType The Java array's type (e.g., jbooleanArray)
     * @tparam NativeArrayElementType The type of elements of the native array
     * @param jni_env
     * @param Java_array The Java primitive array
     * @param begin_pos The position in \p Java_array to start writing from
     * @param buf The elements to copy
     * @param buf_len The number of elements to copy
     * @throw libclp_ffi_java::JavaExceptionOccurred if the array couldn't be
     * copied into.
     */
    template <typename JavaArrayType, typename NativeArrayElementType>
    static void copy_primitive_array_to_java_unsafe (JNIEnv* jni_env, JavaArrayType Java_array,
                                                     size_t begin_pos,
                                                     const NativeArrayElementType* buf,
                                                     size_t buf_len);

    template <typename JavaArrayType, typename NativeArrayElementType>
    std::unique_ptr<
            NativeArrayElementType,
            JavaPrimitiveArrayElementsDeleter<JavaArrayType, NativeArrayElementType>
    > get_java_primitive_array_elements (JNIEnv* jni_env, JavaArrayType Java_array,
                                         jint release_mode)
    {
        std::function<NativeArrayElementType*(JNIEnv*, JavaArrayType, jboolean*)> get_array;
        if constexpr (std::is_same_v<JavaArrayType, jbooleanArray>) {
            get_array = &JNIEnv::GetBooleanArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jbyteArray>) {
            get_array = &JNIEnv::GetByteArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jcharArray>) {
            get_array = &JNIEnv::GetCharArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jshortArray>) {
            get_array = &JNIEnv::GetShortArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jintArray>) {
            get_array = &JNIEnv::GetIntArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jlongArray>) {
            get_array = &JNIEnv::GetLongArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jfloatArray>) {
            get_array = &JNIEnv::GetFloatArrayElements;
        } else if constexpr (std::is_same_v<JavaArrayType, jdoubleArray>) {
            get_array = &JNIEnv::GetDoubleArrayElements;
        }

        using DeleterType =
                JavaPrimitiveArrayElementsDeleter<JavaArrayType, NativeArrayElementType>;
        auto array = std::unique_ptr<NativeArrayElementType, DeleterType>{
                get_array(jni_env, Java_array, nullptr),
                DeleterType{jni_env, Java_array, release_mode}};
        auto exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "Get<Primitive>ArrayElements failed");
        }
        if (nullptr == array) {
            throw GeneralException(ErrorCode_Failure, __FILENAME__, __LINE__,
                                   "Get<Primitive>ArrayElements failed");
        }
        return array;
    }

    template <typename JavaArrayType, typename NativeArrayElementType>
    JavaArrayType new_java_primitive_array (JNIEnv* jni_env, const NativeArrayElementType* buf,
                                            size_t buf_len)
    {
        if (buf_len > cJSizeMax) {
            throw JavaUnsupportedOperationException(__FILENAME__, __LINE__, jni_env,
                                                    "Data can't fit in a Java array");
        }
        std::function<JavaArrayType(JNIEnv*, jsize)> new_array;
        if constexpr (std::is_same_v<JavaArrayType, jbooleanArray>) {
            new_array = &JNIEnv::NewBooleanArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jbyteArray>) {
            new_array = &JNIEnv::NewByteArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jcharArray>) {
            new_array = &JNIEnv::NewCharArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jshortArray>) {
            new_array = &JNIEnv::NewShortArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jintArray>) {
            new_array = &JNIEnv::NewIntArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jlongArray>) {
            new_array = &JNIEnv::NewLongArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jfloatArray>) {
            new_array = &JNIEnv::NewFloatArray;
        } else if constexpr (std::is_same_v<JavaArrayType, jdoubleArray>) {
            new_array = &JNIEnv::NewDoubleArray;
        }
        auto Java_array = new_array(jni_env, static_cast<jsize>(buf_len));
        auto exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "New<Primitive>Array failed");
        }
        if (nullptr == Java_array) {
            throw GeneralException(ErrorCode_Failure, __FILENAME__, __LINE__,
                                   "New<Primitive>Array failed");
        }
        copy_primitive_array_to_java_unsafe(jni_env, Java_array, 0, buf, buf_len);
        return Java_array;
    }

    template <typename JavaArrayType, typename NativeArrayElementType>
    static void copy_primitive_array_to_java_unsafe (JNIEnv* jni_env, JavaArrayType Java_array,
                                                     size_t begin_pos,
                                                     const NativeArrayElementType* buf,
                                                     size_t buf_len)
    {
        std::function<void(JNIEnv*, JavaArrayType, jsize, jsize,
                           const NativeArrayElementType*)> set_array_region;
        if constexpr (std::is_same_v<JavaArrayType, jbooleanArray>) {
            set_array_region = &JNIEnv::SetBooleanArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jbyteArray>) {
            set_array_region = &JNIEnv::SetByteArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jcharArray>) {
            set_array_region = &JNIEnv::SetCharArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jshortArray>) {
            set_array_region = &JNIEnv::SetShortArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jintArray>) {
            set_array_region = &JNIEnv::SetIntArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jlongArray>) {
            set_array_region = &JNIEnv::SetLongArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jfloatArray>) {
            set_array_region = &JNIEnv::SetFloatArrayRegion;
        } else if constexpr (std::is_same_v<JavaArrayType, jdoubleArray>) {
            set_array_region = &JNIEnv::SetDoubleArrayRegion;
        }
        set_array_region(jni_env, Java_array, static_cast<jsize>(begin_pos),
                         static_cast<jsize>(buf_len), buf);
        auto exception = jni_env->ExceptionOccurred();
        if (nullptr != exception) {
            throw JavaExceptionOccurred(ErrorCode_Failure, __FILENAME__, __LINE__,
                                        "Set<Primitive>>ArrayRegion failed");
        }
    }
}

#endif // LIBCLP_FFI_JAVA_COMMON_TPP
