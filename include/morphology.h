#pragma once
#include "image_loader.h"

enum Operation { EROSION, DILATION, OPENING, CLOSING, GRADIENT };
enum Approach { SEQUENTIAL, OPENMP, CUDA };
enum CudaMemoryType { GLOBAL, CONSTANT, SHARED, NOT_RELEVANT };

// --- Dichiarazioni funzioni helper per convertire enums a stringhe ---

std::string operation_name(const Operation operation);

std::string approach_name(const Approach approach);

std::string memory_type_name(const CudaMemoryType memory_type);

// --- Dichiarazioni funzioni morfologiche: sequenziale, OpenMP e CUDA ---

Image morphological_operation_seq(const Image& img, const StructuringElement& se, Operation operation);

Image morphological_operation_omp(const Image& img, const StructuringElement& se, Operation operation);

Image morphological_operation_cuda(const Image& img, const StructuringElement& se, Operation operation, CudaMemoryType mem_type);