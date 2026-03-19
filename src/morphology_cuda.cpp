#include <iostream>

#include "../include/morphology.h"

#ifdef USE_CUDA
// dichiarazione della vera implementazione CUDA
Image morphological_operation_cuda_impl(const Image&, const StructuringElement&, const std::string&, CudaMemoryType);
#endif

Image morphological_operation_cuda(const Image& img, const StructuringElement& se, const std::string& operation, CudaMemoryType mem_type) {
    #ifdef USE_CUDA
    return morphological_operation_cuda_impl(img, se, operation, mem_type);
    #else
    // fallback trasparente su versione sequenziale
    std::cout << "CUDA not found, defaulting to sequential implementation.\n";
    return morphological_operation_seq(img, se, operation);
    #endif
}