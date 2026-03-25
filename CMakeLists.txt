cmake_minimum_required(VERSION 3.18)
project(lab-pp-assignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMP
find_package(OpenMP REQUIRED)

# CUDA support optionally
include(CheckLanguage)
check_language(CUDA)

# CUDA detection
if(CMAKE_CUDA_COMPILER)
    message(STATUS "CUDA compiler found: ${CMAKE_CUDA_COMPILER}")
    # Enable CUDA language support
    enable_language(CUDA)
    # Optionally also locate toolkit for linking helpers
    find_package(CUDAToolkit)
    set(HAVE_CUDA TRUE)
else()
    message(STATUS "CUDA compiler not found; building CPU-only")
    set(HAVE_CUDA FALSE)
endif()

# Sources
set(SOURCES
    src/main.cpp
    src/image_loader.cpp
    src/morphology_utils.cpp
    src/morphology_seq.cpp
    src/morphology_omp.cpp
    src/morphology_cuda.cpp
)

# Target
add_executable(morphological-image-processing ${SOURCES})

# OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(morphological-image-processing PUBLIC OpenMP::OpenMP_CXX)
endif()

# CUDA sources and settings
if(CUDAToolkit_FOUND)
    message(STATUS "CUDA found: ${CUDAToolkit_VERSION}")
    target_sources(morphological-image-processing PRIVATE
        src/morphology_cuda_global.cu
        src/morphology_cuda_constant.cu
        src/morphology_cuda_shared.cu
    )
    target_compile_definitions(morphological-image-processing PRIVATE USE_CUDA)
    # Modern CUDA properties
    set_target_properties(morphological-image-processing PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "native"
    )
    # Optionally link CUDA toolkit (if needed)
    target_link_libraries(morphological-image-processing PRIVATE CUDA::cudart)
else()
    message(STATUS "CUDA NOT found, building CPU-only version")
endif()

# Binary executable output directory
set_target_properties(morphological-image-processing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ..
)