#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MODULE_API_EXPORTS
#include "pixhom.h"



// Global Variables
int i = 0;
int j = 0;
int changed = 0;

int numRows = 0; 
int numCols = 0;
int num_u_points = 0;
int num_dgm = 0;
int argmin = -1;
int argmax = -1;
int c_obj = 0;
int t_obj = 0;
int c_point = 0;
int t_point = 0;

double input = 0;
double min = -1;
double max = -1;
double localmax = -1;
double c_val = 0;
double t_val = 0;

int *mpatch = NULL;
int *dgm_pos = NULL;
double *inputArray = NULL;
double *dgm = NULL;
UPoint *uPoints = NULL;
Result res;



// Function to find Argmin and Argmax of array
void findArgminArgmax() {
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
}

// Function to compare UPoints for qsort
int compareUPoints(const void *a, const void *b) { 
    const UPoint *p1 = (const UPoint *) a;
    const UPoint *p2 = (const UPoint *) b;
    
    double p1_u_val = inputArray[p1->u_point];
    double p2_u_val = inputArray[p2->u_point];
    double p1_c_val = inputArray[p1->c_point];
    double p2_c_val = inputArray[p2->c_point];
    
    if (p2_u_val > p1_u_val){
        return 1;
    }else if (p2_u_val< p1_u_val){
        return -1;
    }else{
        if (p2_c_val > p1_c_val){
            return 1;
        }else if (p2_c_val < p1_c_val){
            return -1;
        }else{
            return 0;    
        }
    }
}

// Function to update mpatch array
void mpatchUpdate(){
    input = inputArray[t_point];
    if (localmax < input) {
        mpatch[c_point] = t_point;
        localmax = input;
    }else if (localmax == input){
        if (c_point < t_point){
            mpatch[c_point] = t_point;
        }else{
            mpatch[c_point] = c_point;
        }
    }
}

// Function to find Upoints
void findUpoints(){
    c_obj = mpatch[c_point];
    t_obj = mpatch[t_point];

    c_val = inputArray[c_point];
    t_val = inputArray[t_point];

    if (c_obj != t_obj && ((c_val > t_val) || ((c_val == t_val) && (c_point > t_point)))) {
        uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));

        // Store information about u_point in the uPoints array
        uPoints[num_u_points].c_point = c_point;
        uPoints[num_u_points].u_point = t_point;

        num_u_points = num_u_points + 1;           
        
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
    int numRows = atoi(argv[1]);
    int numCols = atoi(argv[2]);

    inputArray = (double *)malloc(numRows * numCols * sizeof(double));
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
    
    
    // Calculate Argmin and Argmax
    findArgminArgmax();
    
    return 0; 
    
}


