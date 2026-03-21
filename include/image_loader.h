#pragma once
#include <assert.h>
#include <string>
#include <vector>
#include <cstdint>

// Tipo per immagine grayscale
class Image {
public:
    Image(int w, int h) : width(w), height(h), data(w * h) {}

    // accesso modificabile
    uint8_t& operator()(int x, int y) {
        assert(in_bounds(x, y));
        return data[y * width + x];
    }

    // accesso in sola lettura
    const uint8_t& operator()(int x, int y) const {
        assert(in_bounds(x, y));
        return data[y * width + x];
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    auto getDataR() const { return data.data(); }
    auto getDataW() { return data.data(); }
private:
    bool in_bounds(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    int width, height;
    std::vector<uint8_t> data; // row major order
};

// Tipo per structuring element
class StructuringElement {
public:
    StructuringElement(int s) : size(s), data(s * s) {}

    // accesso modificabile
    uint8_t& operator()(int x, int y) {
        assert(in_bounds(x, y));
        return data[y * size + x];
    }

    // accesso in sola lettura
    const uint8_t& operator()(int x, int y) const {
        assert(in_bounds(x, y));
        return data[y * size + x];
    }

    int getSize() const { return size; }
    auto getDataR() const { return data.data(); }
    auto getDataW()  { return data.data(); }
private:
    bool in_bounds(int x, int y) const {
        return x >= 0 && x < size && y >= 0 && y < size;
    }

    int size;
    std::vector<uint8_t> data; // 1 = active, 0 = inactive
};

// Funzioni principali
Image load_image(const std::string& filename);        // carica immagine grayscale
void save_image(Image& img, const std::string& filename); // salva immagine grayscale

StructuringElement create_square_se(int size); // crea SE quadrato pieno
StructuringElement create_cross_se(int size);  // crea SE a croce