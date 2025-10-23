#!/bin/bash
# Usage: bash script.sh <software> <dataset> <maxdim> <output_csv>

PHBENCHMARKDIR=$1
SOFTWARE=$2
DATASET=$3
MAXDIM=$4
OUTPUT_CSV=$5

echo "Running $SOFTWARE on $DATASET (maxdim=$MAXDIM)..."

# === Configurazione ===
PYTHON_SCRIPT="${PHBENCHMARKDIR}/test_${SOFTWARE}.py"
NPY_DIR="${DATASET}"

# === Inizializzazione ===
echo "filename,time_s,peak_memory_mb" > "$OUTPUT_CSV"

# Controllo che lo script Python esista
if [ ! -f "$PYTHON_SCRIPT" ]; then
    echo "Errore: script Python $PYTHON_SCRIPT non trovato."
    exit 1
fi

# === Loop su tutti i file .npy ===
for file in "$NPY_DIR"/*.npy; do
    [ -e "$file" ] || continue  # salta se non ci sono file .npy

    echo "▶️  Eseguo $file ..."

    # Tempo di inizio (in secondi)
    start=$(date +%s)

    # Esegui lo script e misura memoria con /usr/bin/time -l
    # (stderr rediretto su stdout per poter leggere)
    output=$(/usr/bin/time -l python "$PYTHON_SCRIPT" "$file" 2>&1)

    # Tempo di fine
    end=$(date +%s)
    elapsed=$((end - start))

    # Estrai la memoria (in byte → MB)
    mem_bytes=$(echo "$output" | awk '/maximum resident set size/ {print $1}')
    if [ -z "$mem_bytes" ]; then
        mem_bytes=0
    fi
    mem_mb=$(awk -v b="$mem_bytes" 'BEGIN {printf "%.2f", b/1024/1024}')

    # Aggiungi una riga al CSV
    echo "$(basename "$file"),$elapsed,$mem_mb" >> "$OUTPUT_CSV"

    echo "✅  $(basename "$file") → ${elapsed}s, ${mem_mb}MB"
done

echo "📊 Risultati salvati in $OUTPUT_CSV"
