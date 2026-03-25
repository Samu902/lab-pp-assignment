#!/bin/bash

shopt -s nullglob

# Configurazione dei path
EXECUTABLE="./morphological-image-processing"
IN_IMG_DIR="./images/in"
REAL_IMAGE_SIZES=(256 512 1024)

# Opzioni possibili dei parametri
IMAGE_SIZES=(1 2 3)           # 256x256, 512x512, 1024x1024
SE_SIZES=(1 2 3)              # 3x3, 5x5, 7x7
OPERATIONS=(1 2 3 4 5)        # erosion, dilation, opening, closing, gradient
APPROACHES=(1 2 3)            # seq, omp, cuda
MEM_TYPES=(1 2 3)             # global, constant, shared memory
REPETITIONS=10                # default: 10
COLD_START_REPETITIONS=2      # default: 2

# Controllo errori
if [ ! -x "$EXECUTABLE" ]; then
  echo "Errore: eseguibile non trovato o non eseguibile"
  exit 1
fi

if [ ! -d "$IN_IMG_DIR" ]; then
  echo "Errore: directory input non trovata"
  exit 1
fi

# Loop di esecuzione di tutti i casi possibili

for img_size in "${IMAGE_SIZES[@]}"; do
  real_img_size=${REAL_IMAGE_SIZES[$((img_size-1))]}

  for file in "$IN_IMG_DIR"/"$real_img_size"/*.pgm; do
    [ -f "$file" ] || continue
    filename=$(basename "$file" .pgm)

    for se_size in "${SE_SIZES[@]}"; do
      for op in "${OPERATIONS[@]}"; do
        for app in "${APPROACHES[@]}"; do
          # Se approach non è CUDA, non contare i memory type
          if [ "$app" -ne 3 ]; then
            mem_list=(1)
          else
            mem_list=("${MEM_TYPES[@]}")
          fi
          for mem in "${mem_list[@]}"; do
            "$EXECUTABLE" \
              --img-size="$img_size" \
              --in-img="$filename" \
              --se-size="$se_size" \
              --operation="$op" \
              --approach="$app" \
              --memory-type="$mem" \
              --repetitions="$REPETITIONS" \
              --cold-start-repetitions="$COLD_START_REPETITIONS"
          done
        done
      done
    done
  done
done