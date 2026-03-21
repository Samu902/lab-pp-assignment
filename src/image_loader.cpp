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

    Image img(width, height);

    file.read(reinterpret_cast<char*>(img.getDataW()), width * height);
    if(!file) throw std::runtime_error("Error reading image data");

    return img;
}

void save_image(const Image& img, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if(!file) throw std::runtime_error("Cannot open file for writing: " + filename);

    file << "P5\n" << img.getWidth() << " " << img.getHeight() << "\n255\n";
    file.write(reinterpret_cast<const char*>(img.getDataR()), img.getWidth() * img.getHeight());
}

// --- Creazione Structuring Element ---

StructuringElement create_square_se(int size) {
    StructuringElement se(size);
    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++)
            se(x, y) = 1;  // tutto attivo
    return se;
}

StructuringElement create_cross_se(int size) {
    StructuringElement se(size);
    int mid = size / 2;
    for(int y = 0; y < size; y++) {
        for(int x = 0; x < size; x++) {
            se(x, y) = (x == mid || y == mid) ? 1 : 0;
        }
    }
    return se;
}