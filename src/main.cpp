#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include "../include/image_loader.h"   // gestione immagini
#include "../include/morphology.h"     // funzioni morfologiche

enum Approach { SEQUENTIAL, OPENMP, CUDA };

std::string operation_name(Operation operation){
    switch(operation) {
    case EROSION: return "erosion";
    case DILATION: return "dilation";
    case OPENING: return "opening";
    case CLOSING: return "closing";
    case GRADIENT: return "gradient";
    default:
        std::cerr << "Invalid choice, defaulting to Erosion.\n";
        return "erosion";
    }
}

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
Operation choose_operation() {
    int choice;
    std::cout << "Select morphological operation:\n";
    std::cout << "1 - Erosion\n2 - Dilation\n3 - Opening\n4 - Closing\n5 - Gradient\n";
    std::cin >> choice;
    switch(choice) {
        case 1: return EROSION;
        case 2: return DILATION;
        case 3: return OPENING;
        case 4: return CLOSING;
        case 5: return GRADIENT;
        default:
            std::cerr << "Invalid choice, defaulting to Erosion.\n";
            return EROSION;
    }
}

int main() {
    std::string in_image_filename;
    int se_size;

    // Input interattivo
    std::cout << "Enter input image file name (.pgm file in images/in folder): ";
    std::cin >> in_image_filename;
    std::cout << "Enter structuring element size (odd integer, e.g., 3/5/7): ";
    std::cin >> se_size;

    Approach approach = choose_approach();
    CudaMemoryType mem_type = (approach == CUDA) ? choose_memory_type() : GLOBAL;
    Operation operation = choose_operation();

    // Caricamento immagine
    Image in_img = load_image("../images/in/" + in_image_filename + ".pgm");
    Image out_img(in_img.getWidth(), in_img.getHeight());
    StructuringElement se = create_square_se(se_size);

    // Vettore per statistiche dei tempi
    std::vector<double> timings;

    // Esecuzione e misurazione
    const int repetitions = 5; // ripetizioni per statistiche
    for(int i=0; i<repetitions; i++) {
        auto start = std::chrono::high_resolution_clock::now();

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

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        timings.push_back(duration);
        std::cout << "Run " << i+1 << ": " << duration << " ms\n";
    }

    // Statistiche
    double sum = 0, min_time = timings[0], max_time = timings[0];
    for(double t : timings) {
        sum += t;
        if(t < min_time) min_time = t;
        if(t > max_time) max_time = t;
    }
    double avg_time = sum / repetitions;
    std::cout << "Timing stats (ms): min=" << min_time << " max=" << max_time << " avg=" << avg_time << "\n";

    // Salva immagine risultato
    save_image(out_img, "../images/out/" + in_image_filename + "_" + operation_name(operation) + ".pgm");
    std::cout << "Done. Output saved to images/out folder.\n";

    return 0;
}