//
// Created by Riccardo Ceccaroni on 01/10/24.
//
// g++ -o sort_floats sort_floats.cpp -std=gnu++0x

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define N1 1024
#define N2 16384
#define N3 65536
#define N4 262144
#define N5 1048576
#define N6 4194304
#define N7 16777216
#define N8 67108864
#define N9 268435456

// Function to fill a vector with random floating point numbers
void fill_vector(std::vector<float>& vec, int n) {
    for (int i = 0; i < n; i++) {
        vec.push_back(static_cast<float>(rand()) / RAND_MAX); // random floats between 0 and 100
    }
}

// Function to measure execution time
float measure_sort_time(std::vector<float>& vec) {
    clock_t start, end;
    start = clock();
    std::sort(vec.begin(), vec.end());  // Using std::sort
    end = clock();
    return static_cast<float>(end - start) / CLOCKS_PER_SEC;
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));  // Seed for random number generation

    // Test with N1, N2, and N3
    int sizes[] = {N1, N2, N3, N4, N5, N6, N7, N8, N9};

    for (int i = 0; i < 9; i++) {
        int n = sizes[i];
        std::vector<float> vec;
        vec.reserve(n);  // Reserve memory for n elements

        fill_vector(vec, n);

        std::cout << "Sorting " << n << " floating-point numbers..." << std::endl;
        float time_taken = measure_sort_time(vec);
        std::cout << "Time taken to sort " << n << " numbers: " << time_taken << " seconds" << std::endl;
    }

    return 0;
}
