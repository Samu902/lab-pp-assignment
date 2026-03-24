#!/bin/bash

# Configurazione dei path
EXECUTABLE="./morphological-image-processing"
IN_IMG_DIR="./images/in"

# Opzioni possibili dei parametri
APPROACHES=(1 2 3)            # seq, omp, cuda
MEM_TYPES=(1 2 3)             # global, constant, shared memory
OPERATIONS=(1 2 3 4 5)        # erosion, dilation, opening, closing, gradient
KERNEL_SIZES=(3 5 7)          # 3x3, 5x5, 7x7
IMAGE_SIZES=(256 512 1024)    # 256x256, 512x512, 1024x1024
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

for file in "$IN_IMG_DIR"/*.pgm; do
  [ -f "$file" ] || continue

  for app in "${APPROACHES[@]}"; do
    for mem in "${MEM_TYPES[@]}"; do
      for op in "${OPERATIONS[@]}"; do
        for k in "${KERNEL_SIZES[@]}"; do
          for img in "${IMAGE_SIZES[@]}"; do

            #echo "Eseguo: $file | $app | $mem | $op | k=$k | img=$img"

            "$EXECUTABLE" \
              --in-img="$file" \
              --approach="$app" \
              --memory-type="$mem" \
              --operation="$op" \
              --kernel-size="$k" \
              --img-size="$img" \
              --repetitions="$REPETITIONS" \
              --cold-start-repetitions="$COLD_START_REPETITIONS"

          done
        done
      done
    done
  done
done