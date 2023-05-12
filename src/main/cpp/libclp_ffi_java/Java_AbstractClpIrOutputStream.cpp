// CLP
#include "../submodules/clp/components/core/src/ffi/ir_stream/protocol_constants.hpp"
#include "../submodules/clp/components/core/src/type_utils.hpp"

// JNI
#include <com_yscope_clp_irstream_AbstractClpIrOutputStream.h>

// Project headers
#include "ClpIrOutputStreamState.hpp"

using libclp_ffi_java::ClpIrOutputStreamState;

JNIEXPORT jlong JNICALL
Java_com_yscope_clp_irstream_AbstractClpIrOutputStream_createNativeState (JNIEnv*, jobject) {
    // NOTE: The use of uintptr_t means that if for some reason,
    // sizeof(jlong) < sizeof(void*) but sizeof(jlong) == sizeof(uintptr_t),
    // then this conversion will still work.
    return bit_cast<jlong>(reinterpret_cast<uintptr_t>(new(std::nothrow) ClpIrOutputStreamState()));
}

JNIEXPORT void JNICALL
Java_com_yscope_clp_irstream_AbstractClpIrOutputStream_destroyNativeState (
        JNIEnv*,
        jobject,
        jlong state_address
) {
    delete reinterpret_cast<ClpIrOutputStreamState*>(bit_cast<uintptr_t>(state_address));
}

JNIEXPORT jbyte JNICALL
Java_com_yscope_clp_irstream_AbstractClpIrOutputStream_getEofByte (JNIEnv*, jobject) {
    return ffi::ir_stream::cProtocol::Eof;
}
