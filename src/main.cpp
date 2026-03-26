#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <map>
#include "../include/image_loader.h"   // gestione immagini
#include "../include/morphology.h"     // funzioni morfologiche

struct Params {
    int image_size;
    std::string in_image_filename;
    int structuring_element_size;
    Operation operation;
    Approach approach;
    CudaMemoryType memory_type;
    int repetitions;
    int cold_start_repetitions;
};

// funzione helper per leggere i command line args
template <typename T>
bool get_cmd_arg(int argc, char* argv[], const std::string arg_name, T& out_value) {
    const std::string prefix = "--" + arg_name + "=";
    for (int i = 1; i < argc; i++) {
        std::string full_arg = argv[i];
        if (full_arg.starts_with(prefix)) {
            std::istringstream iss(full_arg.substr(prefix.size()));
            return static_cast<bool>(iss >> out_value);
        }
    }
    return false;
}

Params choose_params(const int argc, char* argv[]) {
    Params params;

    // 1) image size
    int choice;
    std::cout << "Select image size:\n";
    std::cout << "1 - 256x256\n";
    std::cout << "2 - 512x512\n";
    std::cout << "3 - 1024x1024\n";
    if (!get_cmd_arg(argc, argv, "img-size", choice)) {
        std::cin >> choice;
    } else {
        std::cout << "Got " << choice << " from arg\n";
    }
    switch(choice) {
        case 1:
            params.image_size = 256;
            break;
        case 2:
            params.image_size = 512;
            break;
        case 3:
            params.image_size = 1024;
            break;
        default:
            std::cerr << "Invalid choice, defaulting to 256x256.\n";
            params.image_size = 256;
    }

    // 2) input image filename
    std::string in_image_filename;
    std::cout << "Enter input image file name (.pgm file in images/in folder): ";
    if (!get_cmd_arg(argc, argv, "in-img", in_image_filename)) {
        std::cin >> in_image_filename;
    } else {
        std::cout << "Got " << in_image_filename << " from arg\n";
    }
    params.in_image_filename = in_image_filename;

    // 3) structuring element size
    std::cout << "Select structuring element size:\n";
    std::cout << "1 - 3x3\n";
    std::cout << "2 - 5x5\n";
    std::cout << "3 - 7x7\n";
    if (!get_cmd_arg(argc, argv, "se-size", choice)) {
        std::cin >> choice;
    } else {
        std::cout << "Got " << choice << " from arg\n";
    }
    switch(choice) {
        case 1:
            params.structuring_element_size = 3;
            break;
        case 2:
            params.structuring_element_size = 5;
            break;
        case 3:
            params.structuring_element_size = 7;
            break;
        default:
            std::cerr << "Invalid choice, defaulting to 3x3.\n";
            params.structuring_element_size = 3;
    }

    // 4) morphological operation
    std::cout << "Select morphological operation:\n";
    std::cout << "1 - Erosion\n2 - Dilation\n3 - Opening\n4 - Closing\n5 - Gradient\n";
    if (!get_cmd_arg(argc, argv, "operation", choice)) {
        std::cin >> choice;
    } else {
        std::cout << "Got " << choice << " from arg\n";
    }
    switch(choice) {
        case 1:
            params.operation = EROSION;
            break;
        case 2:
            params.operation = DILATION;
            break;
        case 3:
            params.operation = OPENING;
            break;
        case 4:
            params.operation = CLOSING;
            break;
        case 5:
            params.operation = GRADIENT;
            break;
        default:
            std::cerr << "Invalid choice, defaulting to Erosion.\n";
            params.operation = EROSION;
    }

    // 5) approach
    std::cout << "Select approach:\n";
    std::cout << "1 - Sequential CPU\n";
    std::cout << "2 - Parallel CPU (OpenMP)\n";
    std::cout << "3 - GPU (CUDA)\n";
    if (!get_cmd_arg(argc, argv, "approach", choice)) {
        std::cin >> choice;
    } else {
        std::cout << "Got " << choice << " from arg\n";
    }
    switch(choice) {
        case 1:
            params.approach = SEQUENTIAL;
            break;
        case 2:
            params.approach = OPENMP;
            break;
        case 3:
            params.approach = CUDA;
            break;
        default:
            std::cerr << "Invalid choice, defaulting to Sequential.\n";
            params.approach = SEQUENTIAL;
    }

    // 6) memory model (solo CUDA)
    if (params.approach == CUDA) {
        std::cout << "Select CUDA memory type:\n";
        std::cout << "1 - Global\n2 - Constant\n3 - Shared\n";
        if (!get_cmd_arg(argc, argv, "memory-type", choice)) {
            std::cin >> choice;
        } else {
            std::cout << "Got " << choice << " from arg\n";
        }
        switch(choice) {
            case 1:
                params.memory_type = GLOBAL;
                break;
            case 2:
                params.memory_type = CONSTANT;
                break;
            case 3:
                params.memory_type = SHARED;
                break;
            default:
                std::cerr << "Invalid choice, defaulting to Global.\n";
                params.memory_type = GLOBAL;
                break;
        }
    } else {
        params.memory_type = NOT_RELEVANT;
    }

    // 7) repetitions
    int repetitions;
    std::cout << "Enter number of repetitions: ";
    if (!get_cmd_arg(argc, argv, "repetitions", repetitions)) {
        std::cin >> repetitions;
    } else {
        std::cout << "Got " << repetitions << " from arg\n";
    }
    params.repetitions = repetitions;

    // 8) cold start repetitions
    int cold_start_repetitions;
    std::cout << "Enter number of cold start repetitions: ";
    if (!get_cmd_arg(argc, argv, "cold-start-repetitions", cold_start_repetitions)) {
        std::cin >> cold_start_repetitions;
    } else {
        std::cout << "Got " << cold_start_repetitions << " from arg\n";
    }
    params.cold_start_repetitions = cold_start_repetitions;

    return params;
}

std::string make_key(const Params& p) {
    std::ostringstream oss;
    oss << p.image_size << ","
        << p.in_image_filename << ","
        << p.structuring_element_size << ","
        << operation_name(p.operation) << ","
        << approach_name(p.approach) << ","
        << memory_type_name(p.memory_type) << ","
        << p.repetitions << ","
        << p.cold_start_repetitions;
    return oss.str();
}

void save_to_csv(const Params& params, double min_time, double max_time, double avg_time) {
    const std::string filename = "results.csv";

    std::ifstream infile(filename);
    std::map<std::string, std::string> rows;

    std::string header = "img_size,in_img,se_size,operation,approach,memory_type,repetitions,cold_start_repetitions,min_ms,max_ms,avg_ms";

    // Leggi file esistente
    if (infile.is_open()) {
        std::string line;
        std::getline(infile, line); // salta header

        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string key_part;

            // key = primi 8 campi
            std::getline(iss, key_part, '\n');

            std::string key = key_part.substr(0, key_part.find_last_of(','));
            rows[key] = line;
        }
        infile.close();
    }

    // Crea nuova riga
    std::ostringstream new_row;
    new_row << params.image_size << ","
            << params.in_image_filename << ","
            << params.structuring_element_size << ","
            << operation_name(params.operation) << ","
            << approach_name(params.approach) << ","
            << memory_type_name(params.memory_type) << ","
            << params.repetitions << ","
            << params.cold_start_repetitions << ","
            << min_time << ","
            << max_time << ","
            << avg_time;

    std::string key = make_key(params);
    rows[key] = new_row.str(); // overwrite automatico

    // Riscrivi file
    std::ofstream outfile(filename);
    outfile << header << "\n";
    for (const auto& [k, v] : rows) {
        outfile << v << "\n";
    }
    outfile.close();
}

int main(int argc, char* argv[]) {
    // input interattivo
    Params params = choose_params(argc, argv);

    // caricamento immagine e creazione structuring element
    Image in_img = load_image("./images/in/" + std::to_string(params.image_size) + "/" + params.in_image_filename + ".pgm");
    Image out_img(in_img.getWidth(), in_img.getHeight());
    StructuringElement se = create_square_se(params.structuring_element_size);

    // vettore per statistiche dei tempi
    std::vector<double> timings;

    // esecuzione e misurazione
    for(int i = 0; i < params.repetitions + params.cold_start_repetitions; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        switch(params.approach) {
            case SEQUENTIAL:
                out_img = morphological_operation_seq(in_img, se, params.operation);
                break;
            case OPENMP:
                out_img = morphological_operation_omp(in_img, se, params.operation);
                break;
            case CUDA:
                out_img = morphological_operation_cuda(in_img, se, params.operation, params.memory_type);
                break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        if (i < params.cold_start_repetitions) {
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
    double avg_time = sum / params.repetitions;
    std::cout << "Timing stats (ms): min=" << min_time << " max=" << max_time << " avg=" << avg_time << "\n";

    // Salva le misurazioni su file .csv
    save_to_csv(params, min_time, max_time, avg_time);
    std::cout << "Statistics saved to .csv file.\n";

    // Salva immagine risultato
    std::stringstream output_path_stream;
    output_path_stream << "./images/out/"
        << params.image_size << "/"
        << params.in_image_filename
        << "_se-" << params.structuring_element_size << "x" << params.structuring_element_size
        << "_" << operation_name(params.operation)
        << "_" << approach_name(params.approach)
        << (params.approach == CUDA ? ("_" + memory_type_name(params.memory_type)) : "")
        << ".pgm";
    save_image(out_img, output_path_stream.str());
    std::cout << "Done. Output saved to images/out folder.\n";

    return 0;
}