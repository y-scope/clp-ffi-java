#ifndef LIBCLP_FFI_JAVA_CLPIROUTPUTSTREAMSTATE_HPP
#define LIBCLP_FFI_JAVA_CLPIROUTPUTSTREAMSTATE_HPP

// C++ standard libraries
#include <cstdint>
#include <string>
#include <vector>

namespace libclp_ffi_java {
    /**
     * Struct containing buffers that are reused across multiple IR output
     * stream calls (to avoid reallocations).
     */
    struct ClpIrOutputStreamState {
    public:
        std::string logtype;
        std::vector<int8_t> ir_buffer;
    };
}

#endif //LIBCLP_FFI_JAVA_CLPIROUTPUTSTREAMSTATE_HPP
