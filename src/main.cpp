#include <iostream>
#include <string>
#include "../include/image_loader.h"   // gestione immagini
#include "../include/morphology.h"     // funzioni morfologiche

enum Approach { SEQUENTIAL, OPENMP, CUDA };

// funzione helper per chiedere approccio
Approach choose_approach() {
    int choice;
    std::cout << "Select approach:\n";
    std::cout << "1 - Sequential CPU\n";
    std::cout << "2 - Parallel CPU (OpenMP)\n";
    std::cout << "3 - GPU (CUDA)\n";
    std::cin >> choice;

    switch(choice) {
        case 1: return SEQUENTIAL;
        case 2: return OPENMP;
        case 3: return CUDA;
        default:
            std::cerr << "Invalid choice, defaulting to Sequential.\n";
            return SEQUENTIAL;
    }
}

// funzione helper per tipo memoria CUDA
CudaMemoryType choose_memory_type() {
    int choice;
    std::cout << "Select CUDA memory type:\n";
    std::cout << "1 - Global\n2 - Shared\n3 - Texture\n";
    std::cin >> choice;
    switch(choice) {
        case 1: return GLOBAL;
        case 2: return SHARED;
        case 3: return TEXTURE;
        default:
            std::cerr << "Invalid choice, defaulting to Global.\n";
            return GLOBAL;
    }
}

// funzione helper per operazione morfologica
std::string choose_operation() {
    int choice;
    std::cout << "Select morphological operation:\n";
    std::cout << "1 - Erosion\n2 - Dilation\n3 - Opening\n4 - Closing\n5 - Gradient\n";
    std::cin >> choice;
    switch(choice) {
        case 1: return "erosion";
        case 2: return "dilation";
        case 3: return "opening";
        case 4: return "closing";
        case 5: return "gradient";
        default:
            std::cerr << "Invalid choice, defaulting to Erosion.\n";
            return "erosion";
    }
}

int main() {
    std::string input_path, output_path;
    int se_size;

    // Input interattivo
    std::cout << "Enter input image path: ";
    std::cin >> input_path;
    std::cout << "Enter output image path: ";
    std::cin >> output_path;
    std::cout << "Enter structuring element size (odd integer, e.g., 3,5,7): ";
    std::cin >> se_size;

    Approach approach = choose_approach();
    std::string operation = choose_operation();
    CudaMemoryType mem_type = choose_memory_type();

    // Caricamento immagine
    Image in_img = load_image(input_path);
    Image out_img;
    StructuringElement se = create_square_se(se_size);

    // Esecuzione
    const int repetitions = 5;
    for(int i=0; i<repetitions; i++) {
        switch(approach) {
            case SEQUENTIAL:
                out_img = morphological_operation_seq(in_img, se, operation);
                break;
            case OPENMP:
                out_img = morphological_operation_omp(in_img, se, operation);
                break;
            case CUDA:
                out_img = morphological_operation_cuda(in_img, se, operation, mem_type);
                break;
        }
    }

    // Salva immagine risultato
    save_image(out_img, output_path);
    std::cout << "Done. Output saved to " << output_path << "\n";

    return 0;
}