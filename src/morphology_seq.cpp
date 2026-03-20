#include "../include/morphology.h"
#include <algorithm>
#include <stdexcept>

// --- Primitive base ---
Image erosion_seq(const Image& img, const StructuringElement& se) {
    Image out = img;
    int offset = se.getSize() / 2;

    for(int y = 0; y < img.getHeight(); y++) {
        for(int x = 0; x < img.getWidth(); x++) {
            uint8_t min_val = 255;
            for(int j = 0; j < se.getSize(); j++) {
                for(int i = 0; i < se.getSize(); i++) {
                    if(se(i,j) == 0) continue;
                    int xi = x + i - offset;
                    int yj = y + j - offset;
                    if(xi >=0 && xi < img.getWidth() && yj >=0 && yj < img.getHeight()) {
                        min_val = std::min(min_val, img(xi,yj));
                    }
                }
            }
            out(x,y) = min_val;
        }
    }
    return out;
}

Image dilation_seq(const Image& img, const StructuringElement& se) {
    Image out = img;
    int offset = se.getSize() / 2;

    for(int y = 0; y < img.getHeight(); y++) {
        for(int x = 0; x < img.getWidth(); x++) {
            uint8_t max_val = 0;
            for(int j = 0; j < se.getSize(); j++) {
                for(int i = 0; i < se.getSize(); i++) {
                    if(se(i,j) == 0) continue;
                    int xi = x + i - offset;
                    int yj = y + j - offset;
                    if(xi >=0 && xi < img.getWidth() && yj >=0 && yj < img.getHeight()) {
                        max_val = std::max(max_val, img(xi,yj));
                    }
                }
            }
            out(x,y) = max_val;
        }
    }
    return out;
}

// --- Operazioni derivate ---
Image opening_seq(const Image& img, const StructuringElement& se) {
    return dilation_seq(erosion_seq(img,se), se);
}

Image closing_seq(const Image& img, const StructuringElement& se) {
    return erosion_seq(dilation_seq(img,se), se);
}

Image gradient_seq(const Image& img, const StructuringElement& se) {
    Image dil = dilation_seq(img,se);
    Image ero = erosion_seq(img,se);
    Image out = img;
    for(int y=0; y<img.getHeight(); y++) {
        for(int x=0; x<img.getWidth(); x++) {
            out(x,y) = dil(x,y) - ero(x,y);
        }
    }
    return out;
}

// --- Interfaccia principale ---
Image morphological_operation_seq(const Image& img, const StructuringElement& se, Operation operation) {
    if(operation == EROSION) return erosion_seq(img,se);
    if(operation == DILATION) return dilation_seq(img,se);
    if(operation == OPENING) return opening_seq(img,se);
    if(operation == CLOSING) return closing_seq(img,se);
    if(operation == GRADIENT) return gradient_seq(img,se);
    throw std::invalid_argument("Unknown operation");
}