cmake_minimum_required(VERSION 3.18)
project(lab-pp-assignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
    src/morphology_seq.cpp
    src/morphology_omp.cpp
    src/morphology_cuda.cpp
)

# Target
add_executable(MyProject ${SOURCES})

# OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(MyProject PUBLIC OpenMP::OpenMP_CXX)
endif()

# CUDA sources and settings
if(CUDAToolkit_FOUND)
    message(STATUS "CUDA found: ${CUDAToolkit_VERSION}")
    target_sources(MyProject PRIVATE src/morphology_cuda.cu)
    target_compile_definitions(MyProject PRIVATE USE_CUDA)
    # Modern CUDA properties
    set_target_properties(MyProject PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES "native"
    )
    # Optionally link CUDA toolkit (if needed)
    target_link_libraries(MyProject PRIVATE CUDA::cudart)
else()
    message(STATUS "CUDA NOT found, building CPU-only version")
endif()