#pragma once
#include "image_loader.h"

enum Operation { EROSION, DILATION, OPENING, CLOSING, GRADIENT };
enum CudaMemoryType { GLOBAL, CONSTANT, SHARED };

// --- Funzione sequenziale ---
Image morphological_operation_seq(const Image& img, const StructuringElement& se, Operation operation);

// --- Funzione OpenMP ---
Image morphological_operation_omp(const Image& img, const StructuringElement& se, Operation operation);

// --- Funzione CUDA ---
Image morphological_operation_cuda(const Image& img, const StructuringElement& se, Operation operation, CudaMemoryType mem_type);