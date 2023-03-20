#ifndef LIBCLP_FFI_JAVA_JAVAPRIMITIVEARRAYELEMENTSDELETER_HPP
#define LIBCLP_FFI_JAVA_JAVAPRIMITIVEARRAYELEMENTSDELETER_HPP

// JNI
#include <jni.h>

namespace libclp_ffi_java {
    /**
     * A deleter for the native side of a Java primitive array, obtained through
     * JNIEnv::Get<Primitive>ArrayElements
     * @tparam JavaArrayType The Java array type (e.g., jbooleanArray)
     * @tparam NativeArrayElementType The type of elements of the native array
     */
    template<typename JavaArrayType, typename NativeArrayElementType>
    class JavaPrimitiveArrayElementsDeleter {
    public:
        /**
         * @param jni_env
         * @param Java_array The Java primitive array corresponding to the
         * native array that will be deleted with this deleter
         * @param release_mode The JNI primitive array release mode (see the
         * docs for JNIEnv::Release<Primitive>ArrayElements)
         */
        JavaPrimitiveArrayElementsDeleter (
                JNIEnv* jni_env, JavaArrayType Java_array, jint release_mode
        ) : m_jni_env(jni_env), m_Java_array(Java_array), m_release_mode(release_mode) {}

        void operator() (NativeArrayElementType* ptr);

    private:
        JNIEnv* m_jni_env;
        JavaArrayType m_Java_array;
        jint m_release_mode;
    };
}

#endif // LIBCLP_FFI_JAVA_JAVAPRIMITIVEARRAYELEMENTSDELETER_HPP
