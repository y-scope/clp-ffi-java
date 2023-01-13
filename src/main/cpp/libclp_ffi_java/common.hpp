#ifndef LIBCLP_FFI_JAVA_COMMON_HPP
#define LIBCLP_FFI_JAVA_COMMON_HPP

// C++ standard libraries
#include <type_traits>

// JNI
#include <jni.h>

namespace libclp_ffi_java {
    // Constants
    constexpr size_t cJSizeMax = (1ULL << (sizeof(jsize) * 8 - 1)) - 1;

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

#endif // LIBCLP_FFI_JAVA_COMMON_HPP
