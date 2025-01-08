#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()
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
    q->unfound = (int*)malloc(sizeof(int) * 0);
    q->unfoundCount = 0;
    
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

int testRows(Queue* Q, int col, int solve){
    int i = 0;
    Node *current = Q->front;
    while (i <= Q->stop && current){
        int row = current->row;
        int ld = row - col + (Q->n - 1);
        int rd = row + col;

        //printf("\n ATTEMPTING {%d} - ld: %d; rd: %d", row, Q->leftConstraints[ld], Q->rightConstraints[rd]);
        
        if (Q->leftConstraints[ld] == 0 && Q->rightConstraints[rd] == 0){
            srand(time(NULL) + clock());
            if (rand()%5 != 0){
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
                Q->rowConstraints[row] = col + 1;
                Q->leftConstraints[ld] = col + 1;
                Q->rightConstraints[rd] = col + 1;
                //printf("\nCOL [%d] ROW [%d]",col, row);
                return 1;
            }
        }
        current = current->next;
        i++;
    }
    if (!solve){
        //printf("\nNOT FOUND - COL [%d] ROW [%d] --- %d",col, Q->n, Q->length);
        //Q->unfound = (int *) realloc(Q->unfound,Q->unfoundCount * sizeof(int));
        int *ptr = Q->unfound + Q->unfoundCount;
        *ptr = col;
        Q->unfoundCount ++;
        Q->board[col] = Q->n;
        Q->fails ++;
        Q->front = current->next;
    }
    return 0;
}

void printBoard(Queue* Q){
    PRINT_ARRAY(Q->rowConstraints,Q->n);
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

int fixBoard(Queue* Q, int MaxAttempts){
    int i = 0;
    int attempts = 0;
    int last = Q->n;
    while (attempts < MaxAttempts){
        // PRINT_ARRAY(Q->unfound, Q->unfoundCount);
        // printf("\n");
        // PRINT_ARRAY(Q->rowConstraints,Q->n);
        // printf("\n");
        // PRINT_ARRAY(Q->leftConstraints,Q->n * 2 - 1);
        // printf("\n");
        // PRINT_ARRAY(Q->rightConstraints,Q->n * 2 - 1);
        // printf("\n");
        int inx = 0;
        int col = Q->unfound[0];
        srand(time(NULL) + clock());
        if (Q->unfoundCount > 1){
            inx = rand() % (Q->unfoundCount);
            col = Q->unfound[inx]; //Random Selection
        }

        if (testRows(Q, col, 1) == 0){
            int srow = rand() % Q->n; //Begin Search At Random Position
            int row = srow; 
            int found = 0; //Set to False
            while (!found){
                int ld = row - col + (Q->n - 1);
                int rd = row + col;
                
                if ((Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 1){ //Since we know there are no 0 options left and there is a Queen in most rows
                    int conflict = Q->rowConstraints[row] + Q->leftConstraints[ld] + Q->rightConstraints[rd] - 1;
                    if (conflict != last){
                        if (Q->rowConstraints[row] == 0){
                            //Edge Case Where Exiting Non Same Row, Must Add Back to Search List
                            Node *current = Q->front;
                            int updated = 0;
                            while (!updated){
                                if (current->row == row){
                                    current->row = conflict;
                                    updated = 1;
                                }
                                current = current->next;
                                if (!updated && current == Q->front){
                                    perror("BOARD IS INCORRECT");
                                    return 0;
                                }

                            }
                        }
                        int cr = Q->board[conflict];
                        Q->leftConstraints[cr - conflict + (Q->n - 1)] = 0; //RESET
                        Q->rightConstraints[cr + conflict] = 0;
                        Q->rowConstraints[cr] = 0;

                        found = 1;

                        Q->unfound[inx] = conflict; //Add to solving Queue
                        Q->rowConstraints[row] = col + 1; //Update variables
                        Q->leftConstraints[ld] = col + 1;
                        Q->rightConstraints[rd] = col + 1;
                        Q->board[conflict] = Q->n;
                        Q->board[col] = row;

                        
                        //printf("SEARCHING COL %d; STARTED AT %d; FOUND ROW %d; CONFLICTING WITH COL %d != %d\n", col, srow, row, conflict, last);
                    }
                    
                } 
                if (!found && (row + 1)%Q->n == srow && Q->unfoundCount == 1){
                    printf("\nFAILURE");
                    return 0;
                }
                row = (row + 1) % Q->n;
            }
        } else {
            if (Q->unfoundCount == 1){
                printf("SEARCHING COL %d. SOLVED!\n", col);
                return 1;
            } else {
                for (int i = inx; i < Q->unfoundCount; i++) {
                    Q->unfound[i] = Q->unfound[i + 1];
                    //printf("SEARCHING COL %d. SOLVED ROW, %d more MISSES REMAINING\n", col, Q->unfoundCount - 1);
                }
                Q->unfoundCount --;
            }
        }
        attempts ++;
        last = col;
    }
    return 0;

}