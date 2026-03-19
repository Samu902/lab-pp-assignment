#include "../include/image_loader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// --- Funzioni di caricamento/salvataggio in formato PGM (Portable GrayMap) ---

Image load_image(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if(!file) throw std::runtime_error("Cannot open image: " + filename);

    std::string magic;
    file >> magic;
    if(magic != "P5") throw std::runtime_error("Only binary PGM (P5) supported");

    int width, height, maxval;
    file >> width >> height >> maxval;
    file.get(); // consume newline

    Image img;
    img.width = width;
    img.height = height;
    img.data.resize(width * height);

    file.read(reinterpret_cast<char*>(img.data.data()), width * height);
    if(!file) throw std::runtime_error("Error reading image data");

    return img;
}

void save_image(const Image& img, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if(!file) throw std::runtime_error("Cannot open file for writing: " + filename);

    file << "P5\n" << img.width << " " << img.height << "\n255\n";
    file.write(reinterpret_cast<const char*>(img.data.data()), img.width * img.height);
}

// --- Creazione Structuring Element ---

StructuringElement create_square_se(int size) {
    StructuringElement se;
    se.size = size;
    se.data.resize(size * size, 1); // tutto attivo
    return se;
}

StructuringElement create_cross_se(int size) {
    StructuringElement se;
    se.size = size;
    se.data.resize(size * size, 0);
    int mid = size / 2;
    for(int i = 0; i < size; i++) {
        se.at(mid, i) = 1; // colonna centrale
        se.at(i, mid) = 1; // riga centrale
    }
    return se;
}