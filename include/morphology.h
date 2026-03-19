#pragma once
#include "image_loader.h"
#include <string>

enum CudaMemoryType { GLOBAL, SHARED, TEXTURE };

// --- Funzione sequenziale ---
Image morphological_operation_seq(const Image& img, const StructuringElement& se, const std::string& operation);

// --- Funzione OpenMP ---
Image morphological_operation_omp(const Image& img, const StructuringElement& se, const std::string& operation);

// --- Funzione CUDA ---
Image morphological_operation_cuda(const Image& img, const StructuringElement& se, const std::string& operation, CudaMemoryType mem_type);