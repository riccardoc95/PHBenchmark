#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MODULE_API_EXPORTS
#include "pixhom.h"

// Function to generate random normal dist from uniform with Marsaglia and Bray method
double randn(double mu, double sigma)
{
    double U1, U2, W, mult;
    static double X1, X2;
    static int call = 0;
    
    if (call == 1)
    {
        call = !call;
        return (mu + sigma * (double) X2);
    }
    
    do
    {
        U1 = -1 + ((double) rand () / RAND_MAX) * 2;
        U2 = -1 + ((double) rand () / RAND_MAX) * 2;
        W = pow (U1, 2) + pow (U2, 2);
    }
    while (W >= 1 || W == 0);
    
    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;
    X2 = U2 * mult;
    
    call = !call;
    
    return (mu + sigma * (double) X1);
}


// Function to find Argmin and Argmax of array
MinMaxIndices findArgminArgmax(const double *arr, const double *noise, int size) {
    MinMaxIndices result;

    if (size == 0) {
        // Handle the case when the array is empty
        result.argmin = -1;  // Convention for indicating no minimum
        result.argmax = -1;  // Convention for indicating no maximum
        return result;
    }

    double min_val = arr[0];
    double max_val = arr[0];
    int min_index = 0;
    int max_index = 0;

    for (int i = 1; i < size; i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
            min_index = i;
        }else if (arr[i] == min_val){
            if (noise[i] < noise[min_index]){
                min_index = i;
            }
        }

        if (arr[i] > max_val) {
            max_val = arr[i];
            max_index = i;
        }else if (arr[i] == max_val){
            if (noise[i] > noise[max_index]){
                max_index = i;
            }
        }

    }

    result.argmin = min_index;
    result.argmax = max_index;

    return result;
}

// Function to compare UPoints for qsort
int compareUPoints(const void *a, const void *b) { 
    UPoint *p1 = (UPoint *) a;
    UPoint *p2 = (UPoint *) b;
    if (p2->u_val > p1->u_val){
        return 1;
    }else if (p2->u_val < p1->u_val){
        return -1;
    }else{
        if (p2->c_val > p1->c_val){
            return 1;
        }else if (p2->c_val < p1->c_val){
            return -1;
        }else{
            return 0;    
        }
    }
}


void initialize_random_vector(double *vector, size_t size) {
    for (size_t i = 0; i < size; i++) {
        vector[i] = (rand() % 1000000000) / 1000000000; 

    }
}

// Persistent Homology dimension 0 function
int main(int argc, char* argv[]) {
    // Set up
    unsigned int seed = 12345; // You can use any integer as the seed
    srand(seed);
    
    int numRows = atoi(argv[1]);
    int numCols = atoi(argv[2]);

    double *inputArray = (double *)malloc(numRows * numCols * sizeof(double));
    if (inputArray == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    char path[100] = "../imgs/";
    strcat(path,argv[1]);
    strcat(path,".txt");
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    for (int i = 0; i < (numRows * numCols); i++) {
        if (fscanf(file, "%lf", &inputArray[i]) != 1) {
            perror("Failed to read value");
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    
    double *noise = malloc(numRows * numCols * sizeof(double));
    for (int i = 0; i < numRows * numCols; i++) { 
        noise[i] = randn(0, 1);
    }

    
    // Calculate Argmin and Argmax
    MinMaxIndices argMinMax = findArgminArgmax(inputArray, noise, numRows * numCols);

    
    // Allocate memory for the mpatch array
    int *mpatch = malloc(numRows * numCols * sizeof(int));

    // Set up mpatch array
    for (int i = 0; i < numRows * numCols; i++) {
        mpatch[i] = i;
    }

    // First pass to find local maxima
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            int x_start = (i > 0) ? (i - 1) : 0;
            int y_start = (j > 0) ? (j - 1) : 0;
            int x_end = (i < (numRows - 1)) ? (i + 1) : (numRows - 1);
            int y_end = (j < (numCols - 1)) ? (j + 1) : (numCols - 1);

            int c_point = i * numCols + j;
            double localmax = inputArray[c_point];

            for (int h = x_start; h <= x_end; h++) {
                for (int k = y_start; k <= y_end; k++) {
                     int t_point = h * numCols + k;
                    if (localmax < inputArray[t_point]) {
                        mpatch[c_point] = t_point;
                        localmax = inputArray[t_point];
                    }else if (localmax == inputArray[t_point]){
                        if (noise[c_point] < noise[t_point]){
                            mpatch[c_point] = t_point;
                        }else{
                            mpatch[c_point] = c_point;
                        }
                    }
                }
            }
        }
    }
    
    return 0; 
    
}
