cmake_minimum_required(VERSION 3.5.1)
project(CLP_FFI_JAVA LANGUAGES CXX C)

# Set default build type
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Release")
    message(STATUS "No build type specified. Setting to '${default_build_type}'.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Ensure we're compiling for a little-endian machine since we depend on the byte
# ordering
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if (IS_BIG_ENDIAN)
    message(FATAL_ERROR "Big-endian machines are not supported")
endif()

# Set length of source path for the __FILENAME__ macro
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

find_package(JNI REQUIRED)

add_library(clp-ffi-java SHARED
        src/main/cpp/libclp_ffi_java/ClpIrOutputStreamState.hpp
        src/main/cpp/libclp_ffi_java/common.cpp
        src/main/cpp/libclp_ffi_java/common.hpp
        src/main/cpp/libclp_ffi_java/common.tpp
        src/main/cpp/libclp_ffi_java/GeneralException.hpp
        src/main/cpp/libclp_ffi_java/ir_stream/common.hpp
        src/main/cpp/libclp_ffi_java/ir_stream/common.tpp
        src/main/cpp/libclp_ffi_java/Java_AbstractClpIrOutputStream.cpp
        src/main/cpp/libclp_ffi_java/Java_EightByteClpIrOutputStream.cpp
        src/main/cpp/libclp_ffi_java/Java_FourByteClpIrOutputStream.cpp
        src/main/cpp/libclp_ffi_java/Java_MessageDecoder.cpp
        src/main/cpp/libclp_ffi_java/Java_MessageEncoder.cpp
        src/main/cpp/libclp_ffi_java/JavaException.cpp
        src/main/cpp/libclp_ffi_java/JavaException.hpp
        src/main/cpp/libclp_ffi_java/JavaPrimitiveArrayElementsDeleter.cpp
        src/main/cpp/libclp_ffi_java/JavaPrimitiveArrayElementsDeleter.hpp
        src/main/cpp/submodules/clp/components/core/src/ffi/encoding_methods.cpp
        src/main/cpp/submodules/clp/components/core/src/ffi/encoding_methods.hpp
        src/main/cpp/submodules/clp/components/core/src/ffi/encoding_methods.tpp
        src/main/cpp/submodules/clp/components/core/src/ffi/ir_stream/encoding_methods.cpp
        src/main/cpp/submodules/clp/components/core/src/ffi/ir_stream/encoding_methods.hpp
        src/main/cpp/submodules/clp/components/core/src/ffi/ir_stream/protocol_constants.hpp
        src/main/cpp/submodules/clp/components/core/src/string_utils.cpp
        src/main/cpp/submodules/clp/components/core/src/string_utils.hpp
        src/main/cpp/submodules/clp/components/core/src/string_utils.tpp
        target/include/com_yscope_clp_compressorfrontend_MessageDecoder.h
        target/include/com_yscope_clp_compressorfrontend_MessageEncoder.h
        target/include/com_yscope_clp_irstream_AbstractClpIrOutputStream.h
        target/include/com_yscope_clp_irstream_EightByteClpIrOutputStream.h
        target/include/com_yscope_clp_irstream_FourByteClpIrOutputStream.h
        )
target_compile_features(clp-ffi-java
        PRIVATE cxx_std_17
        )
target_include_directories(clp-ffi-java PRIVATE
        ${JNI_INCLUDE_DIRS}
        target/include
        )
target_compile_options(clp-ffi-java PRIVATE
        "-D_REENTRANT"  # Necessary since the JVM is multithreaded
        )
target_link_libraries(clp-ffi-java PRIVATE
        # Static link to improve compatibility on other Linux versions
        -static-libstdc++
        )
