#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include "../include/image_loader.h"   // gestione immagini
#include "../include/morphology.h"     // funzioni morfologiche

enum Approach { SEQUENTIAL, OPENMP, CUDA };

// funzione helper per leggere i command line args
template <typename T>
bool get_cmd_arg(int argc, char* argv[], const std::string arg_name, T& out_value) {
    std::string full_arg;
    std::string prefix;
    for (int i = 1; i < argc; i++) {
        full_arg = argv[i];
        prefix = "--" + arg_name + "=";
        if (full_arg.starts_with(prefix)) {
            std::istringstream iss(full_arg.substr(prefix.size()));
            return (bool)(iss >> out_value);
        }
    }
    return false;
}

std::string operation_name(Operation operation) {
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
Approach choose_approach(int argc, char* argv[]) {
    int choice;
    std::cout << "Select approach:\n";
    std::cout << "1 - Sequential CPU\n";
    std::cout << "2 - Parallel CPU (OpenMP)\n";
    std::cout << "3 - GPU (CUDA)\n";
    if (!get_cmd_arg(argc, argv, "approach", choice)) {
        std::cin >> choice;
    }
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
CudaMemoryType choose_memory_type(int argc, char* argv[]) {
    int choice;
    std::cout << "Select CUDA memory type:\n";
    std::cout << "1 - Global\n2 - Constant\n3 - Shared\n";
    if (!get_cmd_arg(argc, argv, "memory-type", choice)) {
        std::cin >> choice;
    }
    switch(choice) {
        case 1: return GLOBAL;
        case 2: return CONSTANT;
        case 3: return SHARED;
        default:
            std::cerr << "Invalid choice, defaulting to Global.\n";
            return GLOBAL;
    }
}

// funzione helper per operazione morfologica
Operation choose_operation(int argc, char* argv[]) {
    int choice;
    std::cout << "Select morphological operation:\n";
    std::cout << "1 - Erosion\n2 - Dilation\n3 - Opening\n4 - Closing\n5 - Gradient\n";
    if (!get_cmd_arg(argc, argv, "operation", choice)) {
        std::cin >> choice;
    }
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

int main(int argc, char* argv[]) {
    std::string in_image_filename;
    int se_size;

    // Input interattivo

    // manca quello della dimensione

    std::cout << "Enter input image file name (.pgm file in images/in folder): ";
    if (!get_cmd_arg(argc, argv, "in-img", in_image_filename)) {
        std::cin >> in_image_filename;
    }
    std::cout << "Enter structuring element size (odd integer, e.g., 3/5/7): ";
    if (!get_cmd_arg(argc, argv, "kernel-size", se_size)) {
        std::cin >> se_size;
    }

    // mancano anche quelli di repetitions e cold start -> fare funzione unica dei params?

    Approach approach = choose_approach(argc, argv);
    CudaMemoryType mem_type = (approach == CUDA) ? choose_memory_type(argc, argv) : GLOBAL;
    Operation operation = choose_operation(argc, argv);

    // Caricamento immagine
    Image in_img = load_image("../images/in/" + in_image_filename + ".pgm");
    Image out_img(in_img.getWidth(), in_img.getHeight());
    StructuringElement se = create_square_se(se_size);

    // Vettore per statistiche dei tempi
    std::vector<double> timings;

    // Esecuzione e misurazione
    const int repetitions = 10; // ripetizioni per statistiche
    const int coldStartRepetitions = 2; // ripetizioni ignorate per cold start effects
    for(int i=0; i<repetitions+coldStartRepetitions; i++) {
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
        if (i<coldStartRepetitions) {
            std::cout << "Run " << i+1 << ": " << duration << " ms (ignorata per cold start effects)\n";
        } else {
            timings.push_back(duration);
            std::cout << "Run " << i+1 << ": " << duration << " ms\n";
        }
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