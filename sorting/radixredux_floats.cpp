//
// Created by Riccardo Ceccaroni on 01/10/24.
//
// g++ -o radixredux_floats radixredux_floats.cpp -std=gnu++0x

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>

// Define the number of elements for testing
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
        vec.push_back(static_cast<float>(rand()) / RAND_MAX * 100.0); // random floats between 0 and 100
    }
}

// Helper function to convert float to unsigned int (bitwise)
unsigned int float_to_bits(float f) {
    unsigned int bits;
    std::memcpy(&bits, &f, sizeof(f));
    return bits;
}

// Helper function to convert unsigned int back to float (bitwise)
float bits_to_float(unsigned int bits) {
    float f;
    std::memcpy(&f, &bits, sizeof(bits));
    return f;
}

// Custom comparison function (similar to qsort)
int compare_floats(const void *a, const void *b) {
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    if (fa < fb) return -1;
    else if (fa > fb) return 1;
    else return 0;
}

// RadixRedux function (variant of radix sort for floating point numbers)
// with a comparison function as qsort-like parameter
void radixredux(std::vector<float>& arr, int (*compare)(const void*, const void*)) {
    const int BITS = 64; // 32 bits for floating-point numbers
    const int BUCKETS = 256; // 2^8 buckets for each byte (8 bits)

    std::vector<float> output(arr.size());
    std::vector<unsigned int> count(BUCKETS, 0);

    // Sort bit by bit, 8 bits (1 byte) at a time
    for (int shift = 0; shift < BITS; shift += 8) {
        // Reset count array
        std::fill(count.begin(), count.end(), 0);

        // Count occurrences of each bucket based on the current 8 bits
        for (float num : arr) {
            unsigned int bits = float_to_bits(num);
            unsigned int byte = (bits >> shift) & 0xFF; // Extract current byte
            count[byte]++;
        }

        // Cumulative count to calculate positions
        for (int i = 1; i < BUCKETS; i++) {
            count[i] += count[i - 1];
        }

        // Build output array
        for (int i = arr.size() - 1; i >= 0; i--) {
            unsigned int bits = float_to_bits(arr[i]);
            unsigned int byte = (bits >> shift) & 0xFF;
            output[--count[byte]] = arr[i];
        }

        // Copy output back to original array
        arr = output;
    }

    // Final pass: manually sort using the comparison function
    for (size_t i = 0; i < arr.size() - 1; ++i) {
        for (size_t j = i + 1; j < arr.size(); ++j) {
            if (compare(&arr[i], &arr[j]) > 0) {
                std::swap(arr[i], arr[j]);
            }
        }
    }
}

// Function to measure execution time
float measure_sort_time(std::vector<float>& vec, int (*compare)(const void*, const void*)) {
    clock_t start, end;
    start = clock();
    radixredux(vec, compare);  // Using RadixRedux (variant of radix sort with comparison function)
    end = clock();
    return static_cast<float>(end - start) / CLOCKS_PER_SEC;
}

// Function to print the vector
void print_vector(const std::vector<float>& vec) {
    for (float num : vec) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
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

        std::cout << "Sorting " << n << " floating-point numbers with RadixRedux..." << std::endl;
        float time_taken = measure_sort_time(vec, compare_floats);  // Passing comparison function
        std::cout << "Time taken to sort " << n << " numbers: " << time_taken << " seconds" << std::endl;

        // Print the sorted vector
        //std::cout << "Sorted vector:" << std::endl;
        //print_vector(vec);
        //std::cout << std::endl;
    }

    return 0;
}
