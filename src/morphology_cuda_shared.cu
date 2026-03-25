#include "../include/morphology.h"
#include <cuda_runtime.h>
#include <iostream>
#include <stdexcept>

#define BLOCK_SIZE 16  // dimensione block per CUDA

// ------------------------
// Kernel con shared memory
// ------------------------
__global__ void erosion_shared_kernel(const uint8_t* input, uint8_t* output, int width, int height, const uint8_t* se, int se_size)
{
    extern __shared__ uint8_t shared_mem[];
    uint8_t* s_se = shared_mem; // shared memory SE
    uint8_t* s_img = shared_mem + se_size * se_size; // shared memory tile immagine

    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    // Copia SE dalla global alla shared memory (solo i primi se_size*se_size thread)
    if(tid < se_size * se_size){
        s_se[tid] = se[tid];
    }
    __syncthreads();

    int offset = se_size / 2;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x * blockDim.x;
    int by = blockIdx.y * blockDim.y;

    // Coordinate global del thread
    int x = bx + tx;
    int y = by + ty;

    // Dimensione tile shared (includendo halo)
    int tile_width = BLOCK_SIZE + 2 * offset;
    int tile_height = BLOCK_SIZE + 2 * offset;

    // Coordinate locali nella shared memory
    int local_x = tx + offset;
    int local_y = ty + offset;

    // Copia la tile nell’array shared, con halo
    for (int dy = ty; dy < tile_height; dy += BLOCK_SIZE) {
        for (int dx = tx; dx < tile_width; dx += BLOCK_SIZE) {
            int gx = bx + dx - offset;
            int gy = by + dy - offset;

            if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
                s_img[dy * tile_width + dx] = input[gy * width + gx];
            } else {
                s_img[dy * tile_width + dx] = 255; // valore neutro per erosion
            }
        }
    }
    __syncthreads();

    if(x >= width || y >= height) return;

    uint8_t min_val = 255;
    for (int j = 0; j < se_size; j++) {
        for (int i = 0; i < se_size; i++) {
            if (s_se[j * se_size + i] == 0) continue;

            int sx = local_x + i - offset;
            int sy = local_y + j - offset;

            min_val = min(min_val, s_img[sy * tile_width + sx]);
        }
    }
    output[y * width + x] = min_val;
}

__global__ void dilation_shared_kernel(const uint8_t* input, uint8_t* output, int width, int height, const uint8_t* se, int se_size)
{
    extern __shared__ uint8_t shared_mem[];
    uint8_t* s_se = shared_mem;
    uint8_t* s_img = shared_mem + se_size * se_size;

    int tid = threadIdx.y * blockDim.x + threadIdx.x;
    if (tid < se_size * se_size) {
        s_se[tid] = se[tid];
    }
    __syncthreads();

    int offset = se_size / 2;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = blockIdx.x * blockDim.x;
    int by = blockIdx.y * blockDim.y;

    int x = bx + tx;
    int y = by + ty;

    int tile_width = BLOCK_SIZE + 2 * offset;
    int tile_height = BLOCK_SIZE + 2 * offset;

    int local_x = tx + offset;
    int local_y = ty + offset;

    for (int dy = ty; dy < tile_height; dy += BLOCK_SIZE) {
        for (int dx = tx; dx < tile_width; dx += BLOCK_SIZE) {
            int gx = bx + dx - offset;
            int gy = by + dy - offset;

            if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
                s_img[dy * tile_width + dx] = input[gy * width + gx];
            } else {
                s_img[dy * tile_width + dx] = 0; // valore neutro per dilation
            }
        }
    }
    __syncthreads();

    if (x >= width || y >= height) return;

    uint8_t max_val = 0;
    for (int j = 0; j < se_size; j++) {
        for (int i = 0; i < se_size; i++) {
            if (s_se[j * se_size + i] == 0) continue;

            int sx = local_x + i - offset;
            int sy = local_y + j - offset;

            max_val = max(max_val, s_img[sy * tile_width + sx]);
        }
    }

    output[y * width + x] = max_val;
}

__global__ void subtract_shared_kernel(uint8_t* a, uint8_t* b, uint8_t* out, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;

    int i = y * width + x;
    out[i] = max(0, a[i] - b[i]);
}

// ------------------------
// Funzione host con shared memory
// ------------------------
Image morphological_operation_cuda_shared(const Image& img, const StructuringElement& se, Operation operation)
{
    int width = img.getWidth();
    int height = img.getHeight();
    int se_size = se.getSize();
    Image output(width, height);

    size_t img_bytes = width * height * sizeof(uint8_t);
    size_t se_bytes = se_size * se_size * sizeof(uint8_t);

    uint8_t *d_input, *d_temp, *d_output, *d_se;
    cudaMalloc(&d_input, img_bytes);
    cudaMalloc(&d_output, img_bytes);
    cudaMalloc(&d_se, se_bytes);

    cudaMemcpy(d_input, img.getDataR(), img_bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_se, se.getDataR(), se_bytes, cudaMemcpyHostToDevice);

    dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid((width + BLOCK_SIZE - 1) / BLOCK_SIZE, (height + BLOCK_SIZE - 1) / BLOCK_SIZE);

    // shared memory totale: SE + tile immagine con halo
    size_t shared_mem_size = se_bytes + (BLOCK_SIZE + 2*(se_size/2))*(BLOCK_SIZE + 2*(se_size/2)) * sizeof(uint8_t);

    switch (operation) {
        case EROSION:
            erosion_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_output, width, height, d_se, se_size);
            break;
        case DILATION:
            dilation_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_output, width, height, d_se, se_size);
            break;
        case OPENING:
            cudaMalloc(&d_temp, img_bytes);
            erosion_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_temp, width, height, d_se, se_size);
            dilation_shared_kernel<<<grid, block, shared_mem_size>>>(d_temp, d_output, width, height, d_se, se_size);
            cudaFree(d_temp);
            break;
        case CLOSING:
            cudaMalloc(&d_temp, img_bytes);
            dilation_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_temp, width, height, d_se, se_size);
            erosion_shared_kernel<<<grid, block, shared_mem_size>>>(d_temp, d_output, width, height, d_se, se_size);
            cudaFree(d_temp);
            break;
        case GRADIENT:
            cudaMalloc(&d_temp, img_bytes);
            dilation_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_output, width, height, d_se, se_size);
            erosion_shared_kernel<<<grid, block, shared_mem_size>>>(d_input, d_temp, width, height, d_se, se_size);
            subtract_shared_kernel<<<grid, block>>>(d_output, d_temp, d_output, width, height);
            cudaFree(d_temp);
            break;
        default:
            throw std::invalid_argument("CUDA shared memory: invalid operation");
    }

    cudaDeviceSynchronize();

    cudaMemcpy(output.getDataW(), d_output, img_bytes, cudaMemcpyDeviceToHost);

    cudaFree(d_input);
    cudaFree(d_output);
    cudaFree(d_se);

    return output;
}