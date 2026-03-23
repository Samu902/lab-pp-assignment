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
    extern __shared__ uint8_t s_se[]; // shared memory dinamica
    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    // Copia SE dalla global alla shared memory (solo i primi se_size*se_size thread)
    if(tid < se_size * se_size){
        s_se[tid] = se[tid];
    }
    __syncthreads();

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int offset = se_size / 2;

    if(x >= width || y >= height) return;

    uint8_t min_val = 255;
    for(int j=0; j<se_size; j++){
        for(int i=0; i<se_size; i++){
            if(s_se[j*se_size + i] == 0) continue;
            int xi = x + i - offset;
            int yj = y + j - offset;
            if(xi >=0 && xi < width && yj >=0 && yj < height){
                min_val = min(min_val, input[yj*width + xi]);
            }
        }
    }
    output[y*width + x] = min_val;
}

__global__ void dilation_shared_kernel(const uint8_t* input, uint8_t* output, int width, int height, const uint8_t* se, int se_size)
{
    extern __shared__ uint8_t s_se[];
    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    if(tid < se_size * se_size){
        s_se[tid] = se[tid];
    }
    __syncthreads();

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int offset = se_size / 2;

    if(x >= width || y >= height) return;

    uint8_t max_val = 0;
    for(int j=0; j<se_size; j++){
        for(int i=0; i<se_size; i++){
            if(s_se[j*se_size + i] == 0) continue;
            int xi = x + i - offset;
            int yj = y + j - offset;
            if(xi >=0 && xi < width && yj >=0 && yj < height){
                max_val = max(max_val, input[yj*width + xi]);
            }
        }
    }
    output[y*width + x] = max_val;
}

__global__ void subtract_shared_kernel(uint8_t* a, uint8_t* b, uint8_t* out, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if(x >= width || y >= height) return;

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
    dim3 grid((width+BLOCK_SIZE-1)/BLOCK_SIZE, (height+BLOCK_SIZE-1)/BLOCK_SIZE);

    size_t shared_mem_size = se_bytes; // shared memory dinamica per SE

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