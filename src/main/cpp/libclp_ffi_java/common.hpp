#ifndef LIBCLP_FFI_JAVA_COMMON_HPP
#define LIBCLP_FFI_JAVA_COMMON_HPP

// C++ standard libraries
#include <memory>
#include <type_traits>

// JNI
#include <jni.h>

// Project headers
#include "JavaPrimitiveArrayElementsDeleter.hpp"

namespace libclp_ffi_java {
    // Constants
    constexpr size_t cJSizeMax = (1ULL << (sizeof(jsize) * 8 - 1)) - 1;

    /**
     * Gets a native version of a Java array of primitives as a unique pointer
     * with a custom deleter so that we don't need to manually release the array
     * @tparam JavaArrayType The Java array type (e.g., jbooleanArray)
     * @tparam NativeArrayElementType The type of elements of the native array
     * @param jni_env
     * @param Java_array The Java primitive array
     * @param release_mode The JNI primitive array release mode (see the
     * docs for JNIEnv::Release<Primitive>ArrayElements)
     * @return A unique pointer to the native version of the array
     * @throw libclp_ffi_java::JavaExceptionOccurred if we couldn't get the
     * array due to a Java exception.
     * @throw libclp_ffi_java::GeneralException if we couldn't get the array
     * for another reason.
     */
    template <typename JavaArrayType, typename NativeArrayElementType>
    std::unique_ptr<
            NativeArrayElementType,
            JavaPrimitiveArrayElementsDeleter<JavaArrayType, NativeArrayElementType>
    > get_java_primitive_array_elements (JNIEnv* jni_env, JavaArrayType Java_array,
                                         jint release_mode);

    /**
     * Creates a new Java primitive array and initializes it with the contents
     * of the given buffer.
     * @tparam JavaArrayType The Java array's type (e.g., jbooleanArray)
     * @tparam NativeArrayElementType The type of elements of the native array
     * @param jni_env
     * @param buf The elements to initialize the Java array with
     * @param buf_len The number of elements
     * @return The Java array
     * @throw libclp_ffi_java::JavaUnsupportedOperationException if \p buf_len
     * can't fit in a Java array.
     * @throw libclp_ffi_java::JavaExceptionOccurredOperationException if the
     * array couldn't be allocated or copied into due to a Java exception.
     * @throw libclp_ffi_java::GeneralException if the array couldn't be
     * allocated or copied into for another reason.
     */
    template <typename JavaArrayType, typename NativeArrayElementType>
    JavaArrayType new_java_primitive_array (JNIEnv* jni_env, const NativeArrayElementType* buf,
                                            size_t buf_len);

    /**
     * Cast between pointers after ensuring the source and destination types are
     * the same size
     * @tparam Destination The destination type
     * @tparam Source The source type
     * @param src The source pointer
     * @return The casted pointer
     */
    template <typename Destination, class Source>
    std::enable_if_t<sizeof(Destination) == sizeof(Source), Destination*>
            size_checked_pointer_cast (Source* src)
    {
        return reinterpret_cast<Destination*>(src);
    }

    /**
     * Validates that the given version names for the variables schema and
     * variable encoding methods match the versions currently used by this
     * library.
     * @param jni_env
     * @param Java_variablesSchemaVersion
     * @param Java_variableEncodingMethodsVersion
     */
    void validate_variable_handling_rule_versions (
            JNIEnv* jni_env,
            jbyteArray Java_variablesSchemaVersion,
            jbyteArray Java_variableEncodingMethodsVersion
    );
}

#include "common.tpp"

#endif // LIBCLP_FFI_JAVA_COMMON_HPP
