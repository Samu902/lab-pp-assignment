cmake_minimum_required(VERSION 3.18)
project(lab-pp-assignment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMP
find_package(OpenMP REQUIRED)

# Check if CUDA is available
include(CheckLanguage)
check_language(CUDA)

if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(HAVE_CUDA TRUE)
    message(STATUS "CUDA found")
else()
    set(HAVE_CUDA FALSE)
    message(STATUS "CUDA NOT found, building CPU-only version")
endif()

# Sources
set(SOURCES
    src/main.cpp
    src/image_loader.cpp
    src/morphology_seq.cpp
    src/morphology_omp.cpp
    src/morphology_cuda.cpp
)

if(HAVE_CUDA)
    list(APPEND SOURCES src/morphology_cuda.cu)
endif()

# Target
add_executable(MyProject ${SOURCES})

# OpenMP
target_link_libraries(MyProject PUBLIC OpenMP::OpenMP_CXX)

# CUDA settings (only if available)
if(HAVE_CUDA)
    set_target_properties(MyProject PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
    )
    target_compile_definitions(MyProject PRIVATE
        $<$<BOOL:${HAVE_CUDA}>:USE_CUDA>
    )
    # scegli architettura o usa native (CMake recente)
    set(CMAKE_CUDA_ARCHITECTURES native)
endif()