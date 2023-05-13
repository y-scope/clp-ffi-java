#ifndef LIBCLP_FFI_JAVA_OPERATIONEXCEPTION_HPP
#define LIBCLP_FFI_JAVA_OPERATIONEXCEPTION_HPP

// C++ standard libraries
#include <string>

// Project headers
#include "../submodules/clp/components/core/src/TraceableException.hpp"
#include "JavaException.hpp"

namespace libclp_ffi_java {
    class GeneralException : public TraceableException {
    public:
        /**
         * @param error_code
         * @param filename
         * @param line_number
         * @param message
         */
        GeneralException (
                ErrorCode error_code,
                const char* filename,
                int line_number,
                std::string message
        ) : TraceableException(error_code, filename, line_number), m_message(std::move(message)) {}

        [[nodiscard]] const char* what () const noexcept override {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    /**
     * An exception after a Java exception has already occurred. This is useful
     * so that we can differentiate between exceptions where we should propagate
     * the error to Java and exceptions where we should *not* because an
     * exception is already pending in Java.
     */
    class JavaExceptionOccurred : public GeneralException {
    public:
        /**
         * @param error_code
         * @param filename
         * @param line_number
         * @param message
         */
        JavaExceptionOccurred (
                ErrorCode error_code,
                const char* filename,
                int line_number,
                std::string message
        ) : GeneralException(error_code, filename, line_number, std::move(message)) {}
    };
} // libclp_ffi_java

/**
 * Begins a try block
 * @param ignore Parameter so the macro call looks like a function invocation.
 */
#define LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_BEGIN(ignore) try {
/**
 * Ends a try block and handles all exceptions. If an exception has already
 * been thrown in Java, this block will simply return. If not, an exception
 * will be thrown in Java before returning.
 * @param ERROR_RETURN_VAL The value to return on error
 */
#define LIBCLP_FFI_JAVA_EXCEPTION_CATCHALL_END(ERROR_RETURN_VAL) \
        } catch (libclp_ffi_java::JavaException& e) { \
            /* Corresponding exception already thrown in Java */ \
            return ERROR_RETURN_VAL; \
        } catch (libclp_ffi_java::JavaExceptionOccurred& e) { \
            /* An exception was already thrown in Java, so we shouldn't */ \
            /* throw another */ \
            return ERROR_RETURN_VAL; \
        } catch (std::exception& e) { \
            libclp_ffi_java::JavaRuntimeException::throw_in_java(jni_env, e.what()); \
            return ERROR_RETURN_VAL; \
        } catch (...) { \
            libclp_ffi_java::JavaRuntimeException::throw_in_java(jni_env, \
                                                                 "[native] Unexpected exception"); \
            return ERROR_RETURN_VAL; \
        }
#endif //LIBCLP_FFI_JAVA_OPERATIONEXCEPTION_HPP
