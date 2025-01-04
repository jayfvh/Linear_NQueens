#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <time.h>
#include "ModQueue.h"
#define PRINT_ARRAY(arr, size)            \
    for (int i = 0; i < size; i++) {     \
        printf("%d ", arr[i]);           \
    }                                    \
    printf("\n");
Queue* board(int n) {
    //Allocate Board
    int m = n * 2 - 1;

    

    int *rowConstraints = (int *) malloc(n * sizeof (int *));
    int *leftConstraints = (int *) malloc(m * sizeof (int *));
    int *rightConstraints = (int *) malloc(m *sizeof (int *));

    int *board = (int *) malloc(n * sizeof(int*));
    int *boardFlip = (int *) malloc(n * sizeof(int*));

    //Intilize Arrays to 0
    memset(rowConstraints, 0, n * sizeof (int *));
    memset(leftConstraints, 0, m * sizeof (int *));
    memset(rightConstraints, 0, m * sizeof(int *));
    for (int i = 0; i < n; i++) {
        boardFlip[i] = n;
    }

    Queue *q = new_queue(100,n,rowConstraints, leftConstraints, rightConstraints,board,boardFlip);
    return q;

}

void greedy(int n,int *rowConstraints, int *leftConstraints, int *rightConstraints, int *board, int peak){
    for (int col = 0; col < n; col++){
        int row = 0;
        int ld = row - col + n;
        int rd = row + col;
        int attempt = 0;

        if (rowConstraints[row] + leftConstraints[ld] + rightConstraints[rd] == 0){
            board[col] = row;

        }


    }
}

int main(){
    printf("RUNNING");
    int n = 100000000;
    Queue *q = board(n);
    for (int i = 0; i < n; i++){
        testRows(q, i);
    }
    printf("\nCOMPLETED WITH %d FAILS", q->fails);
    //printBoard(q);

}