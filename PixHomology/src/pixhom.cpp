#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>

#include "pixhom.hpp"

// Global Variables
int i = 0;
int j = 0;
int changed = 0;

int numRows = 0;
int numCols = 0;
int N = 0;
int num_u_points = 0;
int num_dgm = 0;
int argmin = -1;
int argmax = -1;
int c_obj = 0;
int t_obj = 0;
int c_point = 0;
int t_point = 0;

double input = 0;
double val = 0;
double min = -1;
double max = -1;
double localmax = -1;
double c_val = 0;
double t_val = 0;

double *inputArray = nullptr;
std::vector <int> mpatch;
std::vector <int> dgm_pos;
std::vector <double> dgm;
std::vector <UPoint> uPoints;
Result res;


// Function to compare UPoints for qsort
bool compareUPoints(const UPoint& p1, const UPoint& p2) {
    double p1_u_val = inputArray[p1.u_point];
    double p2_u_val = inputArray[p2.u_point];
    double p1_c_val = inputArray[p1.c_point];
    double p2_c_val = inputArray[p2.c_point];

    if (p2_u_val > p1_u_val){
        return false;
    }else if (p2_u_val< p1_u_val){
        return true;
    }else{
        if (p2_c_val > p1_c_val){
            return false;
        }else if (p2_c_val < p1_c_val){
            return true;
        }else{
            return false;
        }
    }
}


// PixHomology core functions
Result computePH(double *inputArr, int numR, int numC) {
    // Set up
    inputArray = inputArr;
    numRows = numR;
    numCols = numC;

    // Calculate Argmin and Argmax
    if ((numRows * numCols) != 0) {
        min = inputArray[0];
        max = inputArray[0];
        argmin = 0;
        argmax = 0;

        for (i = 1; i < (numRows * numCols); i++) {
            input = inputArray[i];
            if (input <= min){
                min = input;
                argmin = i;
            }
            if (input >= max) {
                max = input;
                argmax = i;
            }
        }
    }

    // Set up mpatch array
    for (int i = 0; i < numRows * numCols; i++) {
        mpatch.push_back(i);
    }

    // First pass to find local maxima
    // First pass to find local maxima
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float localmax = inputArray[c_point];
            int best_point = c_point;

            // ↖, ↑, ↗, ←, →, ↙, ↓, ↘  (ordine identico all’originale)
            int di[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
            int dj[8] = {-1,  0,  1, -1,  1, -1,  0,  1};



            for (int k = 0; k < 8; k++) {
                int ni = i + di[k];
                int nj = j + dj[k];

                // evita accessi fuori dai limiti
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols){
                    //localmax = max;
                    //best_point = numRows * numCols + 1;
                    continue;
                }


                int t_point = ni * numCols + nj;
                float input = inputArray[t_point];

                // stessa logica del codice originale
                if (input > localmax) {
                    localmax = input;
                    best_point = t_point;
                } else if (input == localmax) {
                    // stesso tie-breaking: preferisci indice maggiore
                    if (best_point < t_point)
                        best_point = t_point;
                }
            }

            mpatch[c_point] = best_point;
        }
    }

    // Second pass to update the mpatch array
    while (1) {
        changed = 0;
        for (i = 0; i < numRows * numCols; i++) {
            c_obj = mpatch[mpatch[i]];
            if (mpatch[i] != c_obj) {
                mpatch[i] = c_obj;
                changed = 1;
            }
        }
        if (!changed) {
            break;
        }
    }

    // Set up UPoints
    num_u_points = 0;

    // Find uPoints
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {
            c_point = i * numCols + j;
            t_point = 0;
            localmax = inputArray[c_point];
            if ((i > 0) && (j > 0) && (i < numRows - 1) && (j < numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == 0) && (j > 0) && (j < numCols - 1)){
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i > 0) && (j == 0) && (i < numRows - 1)){
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == numRows - 1) && (j > 0) && (j < numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((j == numCols - 1) && (i > 0) && (i < numRows - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == 0) && (j == 0)){
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == numRows - 1) && (j == numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == numRows - 1) && (j == 0)){
                t_point = ((i - 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i - 1) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i) * numCols + (j + 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            } else if ((i == 0) && (j == numCols - 1)){
                t_point = ((i) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j - 1));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
                t_point = ((i + 1) * numCols + (j));
                c_obj = mpatch[c_point];
                t_obj = mpatch[t_point];

                c_val = inputArray[c_point];
                t_val = inputArray[t_point];

                if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
                    uPoints.push_back(UPoint());

                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;

                    num_u_points = num_u_points + 1;

                }
            }

        }
    }

    // Sort uPoints in descending order
    sort(uPoints.begin(), uPoints.end(), compareUPoints);

    // Information about dgm
    num_dgm = 0;

    // Find dgm
    for (i = 0; i < num_u_points; i++) {
        c_point = uPoints[i].c_point;
        t_point = uPoints[i].u_point;

        c_obj = c_point;
        t_obj = t_point;

        while (c_obj != mpatch[c_obj]) {
            c_obj = mpatch[c_obj];
        }
        while (t_obj != mpatch[t_obj]) {
            t_obj = mpatch[t_obj];
        }

        if (c_obj != t_obj) {
            if (inputArray[c_obj] > inputArray[t_obj]) {
                mpatch[t_obj] = c_obj;
                if (fabs(inputArray[t_obj] - inputArray[t_point]) > 0) {
                    dgm.push_back(inputArray[t_obj]);
                    dgm.push_back(inputArray[t_point]);

                    dgm_pos.push_back(t_obj);
                    dgm_pos.push_back(t_point);

                    num_dgm = num_dgm + 2;
                }
            } else if (inputArray[c_obj] < inputArray[t_obj]) {
                mpatch[c_obj] = t_obj;
                if (fabs(inputArray[c_obj] - inputArray[t_point]) > 0) {
                    dgm.push_back(inputArray[c_obj]);
                    dgm.push_back(inputArray[t_point]);

                    dgm_pos.push_back(c_obj);
                    dgm_pos.push_back(t_point);

                    num_dgm = num_dgm + 2;
                }
            } else{
                if (c_obj > t_obj){
                    mpatch[t_obj] = c_obj;
                    if (fabs(inputArray[t_obj] - inputArray[t_point]) > 0) {
                        dgm.push_back(inputArray[t_obj]);
                        dgm.push_back(inputArray[t_point]);

                        dgm_pos.push_back(t_obj);
                        dgm_pos.push_back(t_point);

                        num_dgm = num_dgm + 2;
                    }
                }else{
                    mpatch[c_obj] = t_obj;
                    if (fabs(inputArray[c_obj] - inputArray[t_point]) > 0) {
                        dgm.push_back(inputArray[c_obj]);
                        dgm.push_back(inputArray[t_point]);

                        dgm_pos.push_back(c_obj);
                        dgm_pos.push_back(t_point);

                        num_dgm = num_dgm + 2;
                    }
                }

            }
        }
    }

    // Append the maximum and minimum values to dgm
    dgm.push_back(max);
    dgm.push_back(min);
    dgm_pos.push_back(argmax);
    dgm_pos.push_back(argmin);

    num_dgm = num_dgm + 2;

    res.data = std::move(dgm);
    res.posix = std::move(dgm_pos);
    res.length = std::move(num_dgm);

    return res;

}


// PixHomology core functions
Result computePH1(double *inputArr, int numR, int numC) {
    // Set up
    inputArray = inputArr;
    numRows = numR;
    numCols = numC;

    // Calculate Argmin and Argmax
    if ((numRows * numCols) != 0) {
        min = inputArray[0];
        max = inputArray[0];
        argmin = 0;
        argmax = 0;

        for (i = 1; i < (numRows * numCols); i++) {
            input = inputArray[i];
            if (input <= min){
                min = input;
                argmin = i;
            }
            if (input >= max) {
                max = input;
                argmax = i;
            }
        }
    }

    const int num_pixels = numRows * numCols;
    bool argmax_on_boundary = false;
    if (num_pixels > 0) {
        int argmax_row = argmax / numCols;
        int argmax_col = argmax % numCols;
        argmax_on_boundary = (argmax_row == 0 || argmax_row == numRows - 1 ||
                              argmax_col == 0 || argmax_col == numCols - 1);
    }
    const int boundary_idx = argmax_on_boundary ? argmax : num_pixels;

    // Set up mpatch array (include a virtual boundary node)
    mpatch.clear();
    if (argmax_on_boundary) {
        mpatch.reserve(num_pixels);
        for (int i = 0; i < num_pixels; i++) {
            mpatch.push_back(i);
        }
    } else {
        mpatch.reserve(num_pixels + 1);
        for (int i = 0; i < num_pixels; i++) {
            mpatch.push_back(i);
        }
        mpatch.push_back(boundary_idx);
    }

    // First pass to find local maxima
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float localmax = inputArray[c_point];
            int best_point = c_point;

            // ↖, ↑, ↗, ←, →, ↙, ↓, ↘  (ordine identico all’originale)
            //int di[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
            //int dj[8] = {-1,  0,  1, -1,  1, -1,  0,  1};
            int di[4] = {-1,  0,  0,  1};
            int dj[4] = { 0, -1,  1,  0};


            //for (int k = 0; k < 8; k++) {
            for (int k = 0; k < 4; k++) {
                int ni = i + di[k];
                int nj = j + dj[k];

                // evita accessi fuori dai limiti
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols){
                    localmax = max;
                    best_point = boundary_idx;
                    continue;
                }


                int t_point = ni * numCols + nj;
                float input = inputArray[t_point];

                // stessa logica del codice originale
                if (input > localmax) {
                    localmax = input;
                    best_point = t_point;
                } else if (input == localmax) {
                    // stesso tie-breaking: preferisci indice maggiore
                    if (best_point < t_point)
                        best_point = t_point;
                }
            }

            mpatch[c_point] = best_point;
        }
    }

    // Second pass to update the mpatch array
    while (1) {
        changed = 0;
        for (i = 0; i < numRows * numCols; i++) {
            c_obj = mpatch[mpatch[i]];
            if (mpatch[i] != c_obj) {
                mpatch[i] = c_obj;
                changed = 1;
            }
        }
        if (!changed) {
            break;
        }
    }

    // Set up UPoints
    num_u_points = 0;

    // Find uPoints
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float c_val = inputArray[c_point];
            int c_obj = mpatch[c_point];

            // 4-neighbors: ↑, ↓, ←, →
            int di[4] = {-1, 1, 0, 0};
            int dj[4] = {0, 0, -1, 1};

            for (int k = 0; k < 4; k++) {
                int ni = i + di[k];
                int nj = j + dj[k];

                // salta se fuori dai limiti dell'immagine
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols)
                    continue;

                int t_point = ni * numCols + nj;
                float t_val = inputArray[t_point];
                int t_obj = mpatch[t_point];

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

    // Sort uPoints in descending order
    sort(uPoints.begin(), uPoints.end(), compareUPoints);

    // Information about dgm
    num_dgm = 0;

    // Find dgm
    for (i = 0; i < num_u_points; i++) {
        c_point = uPoints[i].c_point;
        t_point = uPoints[i].u_point;

        c_obj = c_point;
        t_obj = t_point;

        while (c_obj != mpatch[c_obj]) {
            c_obj = mpatch[c_obj];
        }
        while (t_obj != mpatch[t_obj]) {
            t_obj = mpatch[t_obj];
        }

        if (c_obj != t_obj) {
            double c_obj_val = (c_obj == boundary_idx) ? max : inputArray[c_obj];
            double t_obj_val = (t_obj == boundary_idx) ? max : inputArray[t_obj];
            int c_obj_pos = (c_obj == boundary_idx) ? argmax : c_obj;
            int t_obj_pos = (t_obj == boundary_idx) ? argmax : t_obj;

            if (c_obj_val > t_obj_val) {
                mpatch[t_obj] = c_obj;
                if (fabs(t_obj_val - inputArray[t_point]) > 0) {
                    dgm.push_back(t_obj_val);
                    dgm.push_back(inputArray[t_point]);

                    dgm_pos.push_back(t_obj_pos);
                    dgm_pos.push_back(t_point);

                    num_dgm = num_dgm + 2;
                }
            } else if (c_obj_val < t_obj_val) {
                mpatch[c_obj] = t_obj;
                if (fabs(c_obj_val - inputArray[t_point]) > 0) {
                    dgm.push_back(c_obj_val);
                    dgm.push_back(inputArray[t_point]);

                    dgm_pos.push_back(c_obj_pos);
                    dgm_pos.push_back(t_point);

                    num_dgm = num_dgm + 2;
                }
            } else{
                if (c_obj > t_obj){
                    mpatch[t_obj] = c_obj;
                    if (fabs(t_obj_val - inputArray[t_point]) > 0) {
                        dgm.push_back(t_obj_val);
                        dgm.push_back(inputArray[t_point]);

                        dgm_pos.push_back(t_obj_pos);
                        dgm_pos.push_back(t_point);

                        num_dgm = num_dgm + 2;
                    }
                }else{
                    mpatch[c_obj] = t_obj;
                    if (fabs(c_obj_val - inputArray[t_point]) > 0) {
                        dgm.push_back(c_obj_val);
                        dgm.push_back(inputArray[t_point]);

                        dgm_pos.push_back(c_obj_pos);
                        dgm_pos.push_back(t_point);

                        num_dgm = num_dgm + 2;
                    }
                }

            }
        }
    }

    // Append the maximum and minimum values to dgm
    //dgm.push_back(max);
    //dgm.push_back(min);
    //dgm_pos.push_back(argmax);
    //dgm_pos.push_back(argmin);

    //num_dgm = num_dgm + 2;

    res.data = std::move(dgm);
    res.posix = std::move(dgm_pos);
    res.length = std::move(num_dgm);

    return res;

}

void freemem(){
    i = 0;
    j = 0;
    changed = 0;

    numRows = 0;
    numCols = 0;
    num_u_points = 0;
    num_dgm = 0;
    argmin = -1;
    argmax = -1;
    c_obj = 0;
    t_obj = 0;
    c_point = 0;
    t_point = 0;

    input = 0;
    min = -1;
    max = -1;
    localmax = -1;
    c_val = 0;
    t_val = 0;

    inputArray = nullptr;

    res.data.clear();
    res.posix.clear();
    res.length = 0;

    mpatch.clear();
    dgm_pos.clear();
    dgm.clear();
    uPoints.clear();
}




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

int find_parent(int x, std::vector<int>& parent) {
        if (parent[x] == -1)
            return x;
        // Path compression
        parent[x] = find_parent(parent[x], parent);
        return parent[x];
    }

void union_merge(int a, int b, std::vector<int>& parent) {
    int pa = find_parent(a, parent);
    int pb = find_parent(b, parent);
    if (pa != pb)
        parent[pa] = pb;
}

Result test(double *inputArr, int numR, int numC) {
    // Set up
    inputArray = inputArr;
    numRows = numR;
    numCols = numC;
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
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols){
                    localmax4 = max;
                    best4_dir = 9;
                    continue;
                }


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
    int num_u_points_0 = 0;
    int num_u_points_1 = 0;
    std::vector <UPoint> uPoints0;
    std::vector <UPoint> uPoints1;

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {

            int c_point = i * numCols + j;
            float c_val = inputArray[c_point];

            // Trova il massimo (o minimo) locale collegato usando gli offset
            int c_obj8 = find_root(c_point, mpatch8_dir, numRows, numCols);
            int c_obj4 = find_root(c_point, mpatch4_dir, numRows, numCols);

            for (int k = 0; k < 8; k++) {
                int ni = i + di8[k];
                int nj = j + dj8[k];

                // salta se fuori dai limiti dell'immagine
                if (ni < 0 || ni >= numRows || nj < 0 || nj >= numCols)
                    continue;

                int t_point = ni * numCols + nj;
                float t_val = inputArray[t_point];
                int t_obj8 = find_root(t_point, mpatch8_dir, numRows, numCols);

                // stessa logica del codice originale:
                // crea un uPoint se appartengono a oggetti diversi
                // e c_val > t_val o parità risolta da indice maggiore
                if (c_obj8 != t_obj8 && ((c_val > t_val) ||
                    ((c_val == t_val) && (c_point > t_point)))) {

                    uPoints0.push_back(UPoint());
                    uPoints0[num_u_points].c_point = c_point;
                    uPoints0[num_u_points].u_point = t_point;
                    num_u_points_0++;
                }
                if (k == 1 || k == 3 || k == 4 || k == 6) {
                    int t_obj4 = find_root(t_point, mpatch4_dir, numRows, numCols);
                    if (c_obj4 != t_obj4 && ((c_val > t_val) ||
                        ((c_val == t_val) && (c_point > t_point)))) {

                        uPoints1.push_back(UPoint());
                        uPoints1[num_u_points].c_point = c_point;
                        uPoints1[num_u_points].u_point = t_point;
                        num_u_points_1++;
                    }
                }
            }
        }
    }

    // Sort uPoints in descending order
    sort(uPoints0.begin(), uPoints0.end(), compareUPoints);
    sort(uPoints1.begin(), uPoints1.end(), compareUPoints);

    // Information about dgm
    num_dgm = 0;

    std::vector<int> parent(numRows * numCols, -1);

    for (i = 0; i < num_u_points; i++) {

        int c_point = uPoints[i].c_point;
        int t_point = uPoints[i].u_point;

        // Trova i massimi locali (root) di ciascun punto
        int c_obj = find_root(c_point, mpatch8_dir, numRows, numCols);
        int t_obj = find_root(t_point, mpatch8_dir, numRows, numCols);

        // comprimi eventuali fusioni precedenti
        c_obj = find_parent(c_obj, parent);
        t_obj = find_parent(t_obj, parent);

        if (c_obj == t_obj)
            continue;

        double c_val = inputArray[c_obj];
        double t_val = inputArray[t_obj];
        double tc_val = inputArray[t_point];

        if (c_val > t_val || (c_val == t_val && c_obj > t_obj)) {
            union_merge(t_obj, c_obj, parent);

            if (fabs(t_val - tc_val) > 0) {
                dgm.push_back(t_val);
                dgm.push_back(tc_val);
                dgm_pos.push_back(t_obj);
                dgm_pos.push_back(t_point);
                num_dgm += 2;
            }
        } else {
            union_merge(c_obj, t_obj, parent);

            if (fabs(c_val - tc_val) > 0) {
                dgm.push_back(c_val);
                dgm.push_back(tc_val);
                dgm_pos.push_back(c_obj);
                dgm_pos.push_back(t_point);
                num_dgm += 2;
            }
        }
    }

    // Append the maximum and minimum values to dgm
    dgm.push_back(max);
    dgm.push_back(min);
    dgm_pos.push_back(argmax);
    dgm_pos.push_back(argmin);

    num_dgm = num_dgm + 2;

    res.data = std::move(dgm);
    res.posix = std::move(dgm_pos);
    res.length = std::move(num_dgm);

    return res;
}
