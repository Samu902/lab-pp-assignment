# Morphological Image Processing in C++ OpenMP/CUDA

## Description
This project focuses on **mathematical morphology-based image processing**. Instead of convolution kernels, it uses **structuring elements (SE)** to apply morphological operations on digital images.

The main operations include:

- **Erosion**
- **Dilation**
- **Opening**
- **Closing**
- **Gradient**

These operations are useful for:

- Noise removal
- Image segmentation
- Edge extraction

The project provides three implementations:

- **Sequential C++**
- **Parallel CPU using OpenMP**
- **Parallel GPU using CUDA**

The goal is to compare computational performance and analyze the benefits of parallelism on different architectures.

---

## Objectives

- Implement morphological operations on images
- Compare sequential and parallel approaches
- Analyze CPU vs GPU performance
- Study scalability with OpenMP and CUDA
- Evaluate the impact of structuring element size

---

## Morphological Operations

### Primitive Operations
- **Erosion**: output = minimum of pixels covered by SE
- **Dilation**: output = maximum of pixels covered by SE

### Derived Operations
- **Opening** = Erosion, then Dilation
- **Closing** = Dilation, then Erosion
- **Morphological Gradient** = Dilation - Erosion

---

## Project Structure

```
lab-pp-assignment/
│
├─ src/
│   ├─ main.cpp                     ← l'entry point dell'applicazione
│   ├─ image_loader.cpp             ← gestione delle immagini
│   ├─ morphology_utils.cpp         ← funzioni di utilità morphology
│   ├─ morphology_seq.cpp           ← implementazione logica sequenziale
│   ├─ morphology_omp.cpp           ← implementazione logica OpenMP
│   ├─ morphology_cuda.cpp          ← implementazione logica CUDA (controllo compatibilità)
│   ├─ morphology_cuda_global.cu    ← implementazione logica CUDA global memory
│   ├─ morphology_cuda_constant.cu  ← implementazione logica CUDA constant memory
│   └─ morphology_cuda_shared.cu    ← implementazione logica CUDA shared memory
│
├─ include/
│   ├─ image_loader.h               ← header gestione immagini
│   └─ morphology.h                 ← header implementazione logica
│
├─ images/                          ← immagini di input ed output
│   ├─ in/
│   └─ out/
│
├─ run_all_tests.sh                 ← script per eseguire tutti i casi di test
│
├─ CMakeLists.txt                   ← file di configurazione di build
│
└─ README.md
````

---

## Implementation Details

- **Images**: 2D grayscale matrices (as a linear std::vector)
- **Structuring Elements**: 2D binary matrices (e.g. 3x3, 5x5, 7x7 - as a linear std::vector)

### Erosion

```cpp
for each pixel:
    output = minimum of neighbors defined by SE
```

### Dilation

```cpp
for each pixel:
    output = maximum of neighbors defined by SE
```

### Other operations

Other operations are composite of these first two.

---

## Parallelization

### OpenMP (CPU)

```cpp
#pragma omp parallel for
for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
        // morphological operation
    }
}
```

### CUDA (GPU)

* Map 1 thread per pixel
* Optimize usage of memory: from global to constant and shared memory

---

## Benchmark

Test on:

* Images of different sizes (256x256, 512×512, 1024×1024)
* Structuring element sizes (3×3, 5x5, 7×7)
* Different morphological operations
* Different computational versions and memory types (when applicable, e.g. with CUDA)
* Multiple iterations on same parameters
* Skip initial iterations to prevent cold start effects

Measure:

* Min execution time
* Max execution time
* Avg execution time

---

## Analysis

* GPU shows higher speedup for large SEs
* OpenMP is efficient for medium-sized images
* CUDA overhead can reduce benefits for small images

---

## Technologies Used

* **C++ (C++ 20 or newer)**
* **OpenMP** for CPU parallelization
* **CUDA** for GPU acceleration

---

## Requirements

* C++ 20 compatible compiler
* OpenMP support
* NVIDIA CUDA Toolkit
* CUDA-capable GPU for GPU execution

---

## Project Context

This project was developed as a **laboratory project** for the course **Parallel Computing**, held by **Professor Marco Bertini** at the **University of Florence**.

---

## Notes

* Performance results vary depending on hardware configuration
* CUDA implementation requires a compatible NVIDIA GPU
