#include <iostream>

#include "../include/morphology.h"

#ifdef USE_CUDA
// dichiarazione delle implementazioni CUDA
Image morphological_operation_cuda_global(const Image&, const StructuringElement&, Operation);
Image morphological_operation_cuda_constant(const Image&, const StructuringElement&, Operation);
Image morphological_operation_cuda_shared(const Image&, const StructuringElement&, Operation);
#endif

Image morphological_operation_cuda(const Image& img, const StructuringElement& se, Operation operation, CudaMemoryType mem_type) {
    #ifdef USE_CUDA
    switch(mem_type) {
        case GLOBAL:
            std::cout << "Using GLOBAL memory CUDA\n";
            return morphological_operation_cuda_global(img, se, operation);
        case CONSTANT:
            std::cout << "Using CONSTANT memory CUDA\n";
            return morphological_operation_cuda_constant(img, se, operation);
        case SHARED:
            std::cout << "Using SHARED memory CUDA\n";
            return morphological_operation_cuda_shared(img, se, operation);
        default:
            throw std::invalid_argument("Unknown CUDA memory type");
    }
    #else
    // fallback trasparente su versione sequenziale
    std::cout << "CUDA not found, defaulting to sequential implementation.\n";
    return morphological_operation_seq(img, se, operation);
    #endif
}