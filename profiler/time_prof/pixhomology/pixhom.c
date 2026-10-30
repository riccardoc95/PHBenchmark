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


// Function to compare UPoints for qsort
int sortFunction(const void *a, const void *b) { 
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


int setup(char *name_path){
    inputArray = (double *)malloc(numRows * numCols * sizeof(double));
    if (inputArray == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    char path[100] = "../imgs/";
    strcat(path, name_path);
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

    return 0;
    
}



void identifyConcaveComponents(){
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
    mpatch = malloc(numRows * numCols * sizeof(int));
    for (int i = 0; i < numRows * numCols; i++) {
        mpatch[i] = i;
    }
    // First pass to find local maxima
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {
            c_point = i * numCols + j;
            localmax = inputArray[c_point];
            t_point = 0;
            if ((i > 0) && (j > 0) && (i < numRows - 1) && (j < numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
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
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i - 1) * numCols + (j + 1));
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
                t_point = ((i) * numCols + (j - 1));
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
                t_point = ((i) * numCols + (j + 1));
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
                t_point = ((i + 1) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j));
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
                t_point = ((i + 1) * numCols + (j + 1));
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
            } else if ((i == 0) && (j > 0) && (j < numCols - 1)){
                t_point = ((i) * numCols + (j - 1));
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
                t_point = ((i) * numCols + (j + 1));
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
                t_point = ((i + 1) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j));
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
                t_point = ((i + 1) * numCols + (j + 1));
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
            } else if ((i > 0) && (j == 0) && (i < numRows - 1)){
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i - 1) * numCols + (j + 1));
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
                t_point = ((i) * numCols + (j + 1));
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
                t_point = ((i + 1) * numCols + (j));
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
                t_point = ((i + 1) * numCols + (j + 1));
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
            } else if ((i == numRows - 1) && (j > 0) && (j < numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
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
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i - 1) * numCols + (j + 1));
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
                t_point = ((i) * numCols + (j - 1));
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
                t_point = ((i) * numCols + (j + 1));
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
            } else if ((j == numCols - 1) && (i > 0) && (i < numRows - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
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
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j));
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
            } else if ((i == 0) && (j == 0)){ 
                t_point = ((i) * numCols + (j + 1));
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
                t_point = ((i + 1) * numCols + (j));
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
                t_point = ((i + 1) * numCols + (j + 1));
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
            } else if ((i == numRows - 1) && (j == numCols - 1)){
                t_point = ((i - 1) * numCols + (j - 1));
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
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i) * numCols + (j - 1));
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
            } else if ((i == numRows - 1) && (j == 0)){ 
                t_point = ((i - 1) * numCols + (j));
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
                t_point = ((i - 1) * numCols + (j + 1));
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
                t_point = ((i) * numCols + (j + 1));
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
            } else if ((i == 0) && (j == numCols - 1)){
                t_point = ((i) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j - 1));
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
                t_point = ((i + 1) * numCols + (j));
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
}


void edgePointsDetection(){
    // Set up UPoints array
    uPoints = malloc(1 * sizeof(UPoint));
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
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
                    uPoints = realloc(uPoints, (1 + num_u_points) * sizeof(UPoint));
            
                    // Store information about u_point in the uPoints array
                    uPoints[num_u_points].c_point = c_point;
                    uPoints[num_u_points].u_point = t_point;
            
                    num_u_points = num_u_points + 1;           
                    
                }
            } 

        }
    }
}


void filterFunction(){
    // Create an array to store information about dgm
    dgm = (double *)malloc(2 * sizeof(double));
    dgm_pos = (int *)malloc(2 * sizeof(int));
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
                    dgm[num_dgm] = inputArray[t_obj];
                    dgm[(num_dgm + 1)] = inputArray[t_point];
                    
                    dgm_pos[num_dgm] = t_obj;
                    dgm_pos[(num_dgm + 1)] = t_point;
                    
                    num_dgm = num_dgm + 2;
                    
                    dgm = realloc(dgm, (num_dgm + 2) * sizeof(double));
                    dgm_pos = realloc(dgm_pos, (num_dgm + 2) * sizeof(int));
                }
            } else if (inputArray[c_obj] < inputArray[t_obj]) {
                mpatch[c_obj] = t_obj;
                if (fabs(inputArray[c_obj] - inputArray[t_point]) > 0) {
                    dgm[num_dgm]  = inputArray[c_obj];
                    dgm[(num_dgm + 1)] = inputArray[t_point];

                    dgm_pos[num_dgm]  = c_obj;
                    dgm_pos[(num_dgm + 1)] = t_point;
                    
                    num_dgm = num_dgm + 2;
                    
                    dgm = realloc(dgm, (num_dgm + 2) * sizeof(double));
                    dgm_pos = realloc(dgm_pos, (num_dgm + 2) * sizeof(int));
                }
            } else{
                if (c_obj > t_obj){
                    mpatch[t_obj] = c_obj;
                    if (fabs(inputArray[t_obj] - inputArray[t_point]) > 0) {
                        dgm[num_dgm] = inputArray[t_obj];
                        dgm[(num_dgm + 1)] = inputArray[t_point];

                        dgm_pos[num_dgm] = t_obj;
                        dgm_pos[(num_dgm + 1)] = t_point;
                        
                        num_dgm = num_dgm + 2;
                        
                        dgm = realloc(dgm, (num_dgm + 2) * sizeof(double));
                        dgm_pos = realloc(dgm_pos, (num_dgm + 2) * sizeof(int));
                    }
                }else{
                    mpatch[c_obj] = t_obj;
                    if (fabs(inputArray[c_obj] - inputArray[t_point]) > 0) {
                        dgm[num_dgm]  = inputArray[c_obj];
                        dgm[(num_dgm + 1)] = inputArray[t_point];

                        dgm_pos[num_dgm]  = c_obj;
                        dgm_pos[(num_dgm + 1)] = t_point;
                        
                        num_dgm = num_dgm + 2;
                        
                        dgm = realloc(dgm, (num_dgm + 2) * sizeof(double));
                        dgm_pos = realloc(dgm_pos, (num_dgm + 2) * sizeof(int));
                    }
                }
                
            }
        }
    }

    // Append the maximum and minimum values to dgm
    dgm[num_dgm] = max;  // Maximum value
    dgm[(num_dgm + 1)] = min;  // Minimum value
    
    dgm_pos[num_dgm] = argmax;  // Maximum value
    dgm_pos[(num_dgm + 1)] = argmin;  // Minimum value
    
    num_dgm = num_dgm + 2;
}


// Persistent Homology dimension 0 function
int main(int argc, char* argv[]) {
    // Set up
    numRows = atoi(argv[1]);
    numCols = atoi(argv[2]);
    setup(argv[1]);

    
    identifyConcaveComponents();

    
    // Set up UPoints array
    // Find uPoints
    edgePointsDetection();

    qsort(uPoints, num_u_points, sizeof(*uPoints), sortFunction);

    // Find dgm
    filterFunction();

    res.data = dgm;
    res.posix = dgm_pos;
    res.length = num_dgm;
    
    return 0; 
    
}


