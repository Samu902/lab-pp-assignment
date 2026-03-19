# Morphological Image Processing in C++ OpenMP/CUDA

## Description
This project focuses on **mathematical morphology-based image processing**. Instead of convolution kernels, it uses **structuring elements (SE)** to apply morphological operations on digital images.

The main operations include:

- **Erosion**
- **Dilation**
- **Opening**
- **Closing**
- **Morphological Gradient**

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
- **Opening** = Erosion → Dilation
- **Closing** = Dilation → Erosion
- **Morphological Gradient** = Dilation − Erosion

### Optional (Bonus)
- **Top-hat**
- **Black-hat**

---

## Project Structure

```
lab-pp-assignment/
│
├─ src/
│   ├─ main.cpp                     ← l'entry point dell'applicazione
│   ├─ image_loader.cpp             ← gestione delle immagini
│   ├─ morphology_seq.cpp           ← implementazione logica sequenziale
│   ├─ morphology_omp.cpp           ← implementazione logica OpenMP
│   ├─ morphology_cuda.cpp          ← implementazione logica CUDA (controllo compatibilità)
│   └─ morphology_cuda.cu           ← implementazione logica CUDA
│
├─ include/
│   ├─ image_loader.h               ← header gestione immagini
│   └─ morphology.h                 ← header implementazione logica
│
├─ images/                          ← immagini di input ed output
│   ├─ in/
│   └─ out/
│
├─ results/                         ← salvataggio delle misurazioni sperimentali
│
├─ CMakeLists.txt                   ← file di build
│
└─ README.md
````

---

## Implementation Details

- **Images**: 2D grayscale matrices
- **Structuring Elements**: 2D binary matrices (e.g., 3x3, 5x5)

Example SE:
```cpp
int se[3][3] = {
  {0, 1, 0},
  {1, 1, 1},
  {0, 1, 0}
};
````

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
* Use shared memory for optimization

---

## Benchmark

Test on:

* Images of different sizes: 512×512, 1024×1024, 4K
* Structuring element sizes: 3×3, 7×7, 15×15

Measure:

* Execution time
* Speedup: `Speedup = T_sequential / T_parallel`

---

## Analysis

* GPU shows higher speedup for large SEs
* OpenMP is efficient for medium-sized images
* CUDA overhead can reduce benefits for small images

---

## Technologies Used

* **C++ (C++17 or newer)**
* **OpenMP** for CPU parallelization
* **CUDA** for GPU acceleration

---

## Requirements

* C++17-compatible compiler
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
