//
// Created by Riccardo Ceccaroni on 01/10/24.
//
// gcc -o qsort_floats qsort_floats.c -std=c99
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N1 1024
#define N2 16384
#define N3 65536
#define N4 262144
#define N5 1048576
#define N6 4194304
#define N7 16777216
#define N8 67108864
#define N9 268435456


// Comparison function for qsort
int compare(const void *a, const void *b) {
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    if (fa < fb) return -1;
    else if (fa > fb) return 1;
    else return 0;
}

// Function to fill array with random floating point numbers
void fill_array(float *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = ((float)rand() / RAND_MAX); // random floats between 0 and 100
    }
}

// Function to measure execution time
float measure_sort_time(float *arr, int n) {
    clock_t start, end;
    start = clock();
    qsort(arr, n, sizeof(float), compare);
    end = clock();
    return ((float)(end - start)) / CLOCKS_PER_SEC;
}

int main() {
    srand(time(NULL));  // Seed for random number generation

    // Test with N1, N2, and N3
    int sizes[] = {N1, N2, N3, N4, N5, N6, N7, N8, N9};

    for (int i = 0; i < 9; i++) {
        int n = sizes[i];
        float *arr = (float *)malloc(n * sizeof(float));
        if (!arr) {
            printf("Memory allocation failed\n");
            return 1;
        }

        fill_array(arr, n);

        printf("Sorting %d floating-point numbers...\n", n);
        float time_taken = measure_sort_time(arr, n);
        printf("Time taken to sort %d numbers: %f seconds\n", n, time_taken);

        free(arr);
    }

    return 0;
}
