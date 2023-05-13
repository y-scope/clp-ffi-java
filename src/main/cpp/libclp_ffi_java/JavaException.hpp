#ifndef LIBCLP_FFI_JAVA_JAVAEXCEPTION_HPP
#define LIBCLP_FFI_JAVA_JAVAEXCEPTION_HPP

// C++ standard libraries
#include <string>

// JNI
#include <jni.h>

// Project headers
#include "../submodules/clp/components/core/src/TraceableException.hpp"

namespace libclp_ffi_java {
    /**
     * An exception class that also throws an exception in the JVM. So we get
     * the benefit of the C++ exception flow without having to do extra work to
     * make sure the exception is reflected in the JVM.
     */
    class JavaException : public TraceableException {
    public:
        // Constructors
        /**
         * Constructs a JavaException
         * @param error_code
         * @param filename
         * @param line_number
         * @param jni_env
         * @param exception_class_signature Signature of the Java exception
         * class to throw in the JVM
         * @param message
         */
        JavaException (ErrorCode error_code, const char* filename, int line_number,
                       JNIEnv* jni_env, const char* exception_class_signature,
                       std::string message);

        // Disable copy and move constructor/assignment since we don't want the
        // caller throwing multiple exceptions in the JVM unintentionally
        JavaException (const JavaException&) = delete;

        JavaException& operator= (const JavaException&) = delete;

        // Methods
        /**
         * Throws the exception in the JVM without needing to throw this
         * exception in C++
         * @param jni_env
         * @param exception_class_signature Signature of the Java exception
         * class to throw in the JVM
         * @param message
         */
        static void throw_in_java (JNIEnv* jni_env, const char* exception_class_signature,
                                   const std::string& message);

        [[nodiscard]] const char* what () const noexcept override {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class JavaClassNotFoundException : public JavaException {
    public:
        // Constructors
        /**
         * Constructs a JavaClassNotFoundException
         * @param filename
         * @param line_number
         * @param jni_env
         * @param class_name The name of the missing Java class
         */
        JavaClassNotFoundException (const char* filename, int line_number, JNIEnv* jni_env,
                                    const char* class_name);

        // Methods
        /**
         * Throws the exception in the JVM without needing to throw this
         * exception in C++
         * @param jni_env
         * @param class_name The name of the missing Java class
         */
        static void throw_in_java (JNIEnv* jni_env, const char* class_name);
    };

    class JavaIllegalArgumentException : public JavaException {
    public:
        // Constructors
        JavaIllegalArgumentException (const char* filename, int line_number, JNIEnv* jni_env,
                                      const std::string& message);

        // Methods
        static void throw_in_java (JNIEnv* jni_env, const std::string& message);
    };

    class JavaIOException : public JavaException {
    public:
        // Constructors
        JavaIOException (const char* filename, int line_number, JNIEnv* jni_env,
                         const std::string& message);

        // Methods
        static void throw_in_java (JNIEnv* jni_env, const std::string& message);
    };

    class JavaRuntimeException : public JavaException {
    public:
        // Constructors
        JavaRuntimeException (const char* filename, int line_number, JNIEnv* jni_env,
                              const std::string& message);

        // Methods
        static void throw_in_java (JNIEnv* jni_env, const std::string& message);
    };

    class JavaUnsupportedOperationException : public JavaException {
    public:
        // Constructors
        JavaUnsupportedOperationException (const char* filename, int line_number, JNIEnv* jni_env,
                                           const std::string& message);

        // Methods
        static void throw_in_java (JNIEnv* jni_env, const std::string& message);
    };
}

#endif // LIBCLP_FFI_JAVA_JAVAEXCEPTION_HPP
