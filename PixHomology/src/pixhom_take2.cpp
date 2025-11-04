#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include "pixhom.hpp"

// Tabelle direzionali globali (offset)
static const int di8[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
static const int dj8[8] = {-1,  0,  1, -1,  1, -1,  0,  1};


int find_root(int start_idx,
              const std::vector<int8_t>& mpatch_dir,
              int numRows,
              int numCols)
{
    int idx = start_idx;

    while (true) {
        int dir = mpatch_dir[idx];
        if (dir == 8)
            break;
        if (dir == 9)
            return numRows * numCols + 1;
        idx += di8[dir] * numCols + dj8[dir];
    }

    return idx;
}

Result test(double *inputArray, int numRows, int numCols) {
    N = numRows * numCols;

    // Trova min, max, argmin, argmax
    min = inputArray[0];
    max = inputArray[0];
    argmin = 0; argmax = 0;
    for (i = 1; i < N; i++) {
        val = inputArray[i];
        if (val <= min) { min = val; argmin = i; }
        if (val >= max) { max = val; argmax = i; }
    }


    // Due array di direzioni (1 byte per pixel)
    std::vector<int8_t> mpatch8_dir(N, 0);
    std::vector<int8_t> mpatch4_dir(N, 0);

    // === Passaggio unico ===
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float val_c = inputArray[c_point];

            float localmax8 = val_c;
            int best8_dir = 8; // 0 = massimo locale

            float localmax4 = -val_c;
            int best4_dir = 8; // 0 = minimo locale

            for (int k = 0; k < 8; k++) {
                int ni = i + di8[k];
                int nj = j + dj8[k];
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols)
                    localmax4 = max;
                    best4_dir = 9;
                    continue;

                int t_point = ni * numCols + nj;
                float val_t = inputArray[t_point];

                // --- 8-neighbors (massimi) ---
                if (val_t > localmax8 || (val_t == localmax8 && t_point > c_point)) {
                    localmax8 = val_t;
                    best8_dir = k; // direzioni numerate 1..8
                }

                // --- 4-neighbors invertiti ---
                if (k == 1 || k == 3 || k == 4 || k == 6) { // ↑ ↓ ← →
                    float val_t_inv = -val_t;
                    if (val_t_inv > localmax4 || (val_t_inv == localmax4 && t_point > c_point)) {
                        localmax4 = val_t_inv;
                        best4_dir = k;
                    }
                }
            }

            mpatch8_dir[c_point] = best8_dir; // direzione verso massimo
            mpatch4_dir[c_point] = best4_dir; // direzione verso minimo (invertito)
        }
    }

    // === Set up UPoints ===
    num_u_points = 0;

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float c_val = inputArray[c_point];

            // Trova il massimo (o minimo) locale collegato usando gli offset
            int c_obj = find_root(c_point, mpatch8_dir, numCols);

            for (int k = 0; k < 8; k++) {
                int ni = i + di8[k];
                int nj = j + dj8[k];

                // salta se fuori dai limiti dell'immagine
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols)
                    continue;

                int t_point = ni * numCols + nj;
                float t_val = inputArray[t_point];
                int t_obj = find_root(t_point, mpatch8_dir, numCols);

                // stessa logica del codice originale:
                // crea un uPoint se appartengono a oggetti diversi
                // e c_val > t_val o parità risolta da indice maggiore
                if (c_obj != t_obj && ((c_val > t_val) ||
                    ((c_val == t_val) && (c_point > t_point)))) {

                    uPoints.push_back(UPoint());
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;
                    num_u_points++;
                }
            }
        }
    }

    return res;
}
