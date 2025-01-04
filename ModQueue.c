#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PRINT_ARRAY(arr, size)            \
    for (int i = 0; i < size; i++) {     \
        printf("%d ", arr[i]);           \
    }                                    \
    printf("\n");
#include "ModQueue.h"

Node* new_node(int row, Node *prev, Node *next){
    Node* n = (Node*) malloc(sizeof(Node));
    n->row = row;
    n->prev = prev;
    n->next = next;
    return n;
}

Queue* new_queue(int fault, int n, int *rowConstraints, int *leftConstraints, int *rightConstraints, int *board, int *boardFlip){
    Queue* q = (Queue*) malloc(sizeof(Queue));
    q->front = NULL;
    q->length = 0;
    q->fault = fault;
    q->n = n;
    q->rowConstraints = rowConstraints;
    q->rightConstraints = rightConstraints;
    q->leftConstraints = leftConstraints;
    q->board = board;
    q->boardFlip = board;
    q->fails = 0;
    
    if (n > 0){
        Node *start = new_node(0,NULL,NULL);
        q->front = start;
        for (int i = 1; i < n; i++){
            Node *new = new_node(i,q->front,NULL);
            q->front->next = new;
            q->front = new;
            q->length += 1;
        }
        q->front->next = start;
        start->prev = q->front;
        q->front = start;
    }

    q->stop = MAX(n, fault);
    
    return q;
}

void delete_node(Node* n){
    if (n != NULL) {  //If it exists
        free(n);
    }
}

void delete_queue(Queue* q){
    if (q != NULL){
        Node* at = q->front;
        while (at != NULL){
            Node* next = at->next;
            delete_node(at);
            at = next;
        }
        free(q);
    }
}

void testRows(Queue* Q, int col){
    int i = 0;
    Node *current = Q->front;
    while (i <= Q->stop && current){
        int row = current->row;
        int ld = row - col + (Q->n - 1);
        int rd = row + col;

        //printf("\n ATTEMPTING {%d} - ld: %d; rd: %d", row, Q->leftConstraints[ld], Q->rightConstraints[rd]);
        
        if (Q->leftConstraints[ld] == 0 && Q->rightConstraints[rd] == 0){
            if (current->next){
                current->next->prev = current->prev;
            }
            if (current->prev){
                current->prev->next = current->next;
                //printf("   P{%d} -> %d ",current->prev->row, current->prev->next->row);
            } else {
                //printf("HII");
            }
            
            if (current->next && (current->next->row - current->row) == 1){
                Q->front = current->next->next;
            } else {
                Q->front = current->next;
            }
            delete_node(current);
            Q->length -= 1;
            Q->stop = MAX(Q->length, Q->fault);

            Q->board[col] = row;
            Q->rowConstraints[row] = 1;
            Q->leftConstraints[ld] = 1;
            Q->rightConstraints[rd] = 1;
            //printf("\nCOL [%d] ROW [%d]",col, row);
            return;
        }
        current = current->next;
        i++;
    }
    printf("\nNOT FOUND - COL [%d] ROW [%d] --- %d",col, Q->n, Q->length);
    Q->board[col] = Q->n;
    Q->fails ++;
    Q->front = current->next;
    return;
}

void printBoard(Queue* Q){
    char* row = (char*)malloc((Q->n * 2) + 1);
    if (row == NULL){
        perror("Failed to allocate memory");
        return;
    }

    for (int i = 0; i < Q->n; i++){
        row[i * 2] = ' ';
        row[i * 2 + 1] = '0';
    }
    row[Q->n * 2] = '\0';
    for (int i = 0; i < Q->n; i++){
        char* row_copy = (char*)malloc((Q->n * 2) + 1);
        strcpy(row_copy, row);
        if (Q->board[i] < Q->n){
            row_copy[(Q->board[i] * 2) + 1] = '1';
        }
        printf("\n %s ", row_copy);
        free(row_copy);
    }
    free(row);

}