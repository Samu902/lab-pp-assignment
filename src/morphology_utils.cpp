#include "../include/morphology.h"
#include <iostream>

// --- Funzioni helper per convertire enums a stringhe ---

std::string operation_name(const Operation operation) {
    switch(operation) {
    case EROSION: return "erosion";
    case DILATION: return "dilation";
    case OPENING: return "opening";
    case CLOSING: return "closing";
    case GRADIENT: return "gradient";
    default:
        std::cerr << "Invalid choice, defaulting to Erosion.\n";
        return "erosion";
    }
}

std::string approach_name(const Approach approach) {
    switch(approach) {
    case SEQUENTIAL: return "sequential";
    case OPENMP: return "openmp";
    case CUDA: return "cuda";
    default:
        std::cerr << "Invalid choice, defaulting to Sequential.\n";
        return "sequential";
    }
}

std::string memory_type_name(const CudaMemoryType memory_type) {
    switch(memory_type) {
    case GLOBAL: return "global";
    case CONSTANT: return "constant";
    case SHARED: return "shared";
    case NOT_RELEVANT: return "not_relevant";
    default:
        std::cerr << "Invalid choice, defaulting to Not Relevant.\n";
        return "not_relevant";
    }
}