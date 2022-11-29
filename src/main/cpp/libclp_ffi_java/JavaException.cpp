#include "JavaException.hpp"

// Constants
static constexpr char cJavaClassNotFoundExceptionSignature[] = "java/lang/ClassNotFoundException";
static constexpr char cJavaIOExceptionSignature[] = "java/io/IOException";
static constexpr char cJavaRuntimeExceptionSignature[] = "java/lang/RuntimeException";

using std::string;

namespace libclp_ffi_java {
    JavaException::JavaException (ErrorCode error_code, const char* const filename,
                                  int line_number, JNIEnv* jni_env,
                                  const char* exception_class_signature,
                                  string message) :
            TraceableException(error_code, filename, line_number), m_message(std::move(message)) {
        JavaException::throw_in_java(jni_env, exception_class_signature, m_message);
    }

    void JavaException::throw_in_java (JNIEnv* jni_env, const char* exception_class_signature,
                                       const string& message) {
        jclass local_class = jni_env->FindClass(exception_class_signature);
        if (nullptr == local_class) {
            // Nothing we can do, so just return
            return;
        }
        // We ignore the return value since if throwing fails, there's nothing
        // we can do anyway
        jni_env->ThrowNew(local_class, message.c_str());
        jni_env->DeleteLocalRef(local_class);
    }

    JavaClassNotFoundException::JavaClassNotFoundException (
            const char* filename,
            int line_number,
            JNIEnv* jni_env,
            const char* class_name
    ) : JavaException(ErrorCode_Failure, filename, line_number, jni_env,
                      cJavaClassNotFoundExceptionSignature,
                      string("Couldn't find ") + class_name) {}

    void JavaClassNotFoundException::throw_in_java (JNIEnv* jni_env, const char* class_name) {
        JavaException::throw_in_java(jni_env, cJavaClassNotFoundExceptionSignature,
                                     string("Couldn't find ") + class_name);
    }

    JavaIOException::JavaIOException (const char* filename, int line_number,
                                      JNIEnv* jni_env, const string& message) :
            JavaException(ErrorCode_Failure, filename, line_number, jni_env,
                          cJavaIOExceptionSignature, message) {}

    void JavaIOException::throw_in_java (JNIEnv* jni_env, const string& message) {
        JavaException::throw_in_java(jni_env, cJavaIOExceptionSignature, message);
    }

    JavaRuntimeException::JavaRuntimeException (
            const char* filename,
            int line_number,
            JNIEnv* jni_env,
            const string& message
    ) : JavaException(ErrorCode_Failure, filename, line_number, jni_env,
                      cJavaRuntimeExceptionSignature, message) {}

    void JavaRuntimeException::throw_in_java (JNIEnv* jni_env, const string& message) {
        JavaException::throw_in_java(jni_env, cJavaRuntimeExceptionSignature, message);
    }
}
