#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <time.h>

int main(int n) {
    //Allocate Board
    int m = n * 2 - 1;

    int *rowConstraints = (int *) malloc(n * sizeof (int *));
    int *leftConstraints = (int *) malloc(m * sizeof (int *));
    int *rightConstraints = (int *) malloc(m *sizeof (int *));

    int *board = (int *) malloc(n * sizeof(int*));

    //Intilize Arrays to 0
    memset(rowConstraints, 0, n * sizeof (int *));
    memset(leftConstraints, 0, m * sizeof (int *));
    memset(rightConstraints, 0, m * sizeof(int *));


    

    



}