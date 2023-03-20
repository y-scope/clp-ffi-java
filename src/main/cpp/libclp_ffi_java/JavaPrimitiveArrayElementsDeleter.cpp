#include "JavaPrimitiveArrayElementsDeleter.hpp"

namespace libclp_ffi_java {
    template<>
    void JavaPrimitiveArrayElementsDeleter<jbooleanArray, jboolean>::operator() (jboolean* ptr) {
        m_jni_env->ReleaseBooleanArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jbyteArray, jbyte>::operator() (jbyte* ptr) {
        m_jni_env->ReleaseByteArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jcharArray, jchar>::operator() (jchar* ptr) {
        m_jni_env->ReleaseCharArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jshortArray, jshort>::operator() (jshort* ptr) {
        m_jni_env->ReleaseShortArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jintArray, jint>::operator() (jint* ptr) {
        m_jni_env->ReleaseIntArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jlongArray, jlong>::operator() (jlong* ptr) {
        m_jni_env->ReleaseLongArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jfloatArray, jfloat>::operator() (jfloat* ptr) {
        m_jni_env->ReleaseFloatArrayElements(m_Java_array, ptr, m_release_mode);
    }

    template<>
    void JavaPrimitiveArrayElementsDeleter<jdoubleArray, jdouble>::operator() (jdouble* ptr) {
        m_jni_env->ReleaseDoubleArrayElements(m_Java_array, ptr, m_release_mode);
    }
}
