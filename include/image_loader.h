#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Tipo per immagine grayscale
struct Image {
    int width;
    int height;
    std::vector<uint8_t> data; // row-major order

    uint8_t& at(int x, int y) { return data[y * width + x]; }
    const uint8_t& at(int x, int y) const { return data[y * width + x]; }
};

// Tipo per structuring element
struct StructuringElement {
    int size;
    std::vector<uint8_t> data; // 1 = active, 0 = inactive

    uint8_t& at(int x, int y) { return data[y * size + x]; }
    const uint8_t& at(int x, int y) const { return data[y * size + x]; }
};

// Funzioni principali
Image load_image(const std::string& filename);        // carica immagine grayscale
void save_image(const Image& img, const std::string& filename); // salva immagine grayscale

StructuringElement create_square_se(int size); // crea SE quadrato pieno
StructuringElement create_cross_se(int size);  // crea SE a croce