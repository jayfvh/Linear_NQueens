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
        q->length = 1;
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
    printQueue(Q);
    int i = 0;
    Node *current = Q->front;
    while (i <= Q->stop && current){
        int row = current->row;
        int ld = row - col + (Q->n - 1);
        int rd = row + col;

        //printf("\n ATTEMPTING {%d} - ld: %d; rd: %d", row, Q->leftConstraints[ld], Q->rightConstraints[rd]);
        
        if (Q->leftConstraints[ld] == 0 && Q->rightConstraints[rd] == 0){
            printf("YO");
            // srand(time(NULL) + clock());
            // if (rand()%5 != 0){
                if (Q->length == 1){
                    printf(";;");
                    Q->front = NULL;
                    Q->length = 0;
                } else {
                    Q->front = current->next;
                    if (Q->length == 2){
                        Q->front->next = Q->front;
                        Q->front->prev = Q->front;
                        printf("FRONT NODE %d CURRENT NODE", Q->front->row, current->row);
                    } else {
                        current->next->prev = current->prev;
                        current->prev->next = current->next;
                        Q->front = Q->front->next;
                    }
                }
                
                printf("?");
                delete_node(current);
                printf("?");
                Q->length -= 1;
                printf("?");
                Q->stop = MAX(Q->length, Q->fault);
                printf("?");
                Q->board[col] = row;
                Q->rowConstraints[row] = col + 1;
                Q->leftConstraints[ld] = col + 1;
                Q->rightConstraints[rd] = col + 1;
                //printf("\nCOL [%d] ROW [%d]",col, row);
                return 1;
            
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

int findAndDelete(Queue* Q, int row){
    printf("EE(FD)");
    printQueue(Q);
    Node *current = Q->front;
    int i = 0;
    while (i < Q->length){
        if (current->row == row){
            if (Q->length == 1){
                Q->front = NULL;
            } else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                Q->front = current->next;
            }
            Q->length --;
            delete_node(current);
            return 1;
            
        }
        current = current->next;
        i++;
    }
    perror("COULD NOT FIND");
    return 0;

}

int findAndReplace(Queue* Q, int row, int row2){
    printf("EE(FR) -> [%d] -> [%d]; ", row, row2);
    printQueue(Q);
    Node *current = Q->front;
    int i = 0;

    while (i < Q->length){
        if (current->row == row){
            current->row = row2;
            return 1;
        }
        current = current->next;
        i ++;

    }
    perror("COULD NOT FIND");
    return 0;
}

void printQueue(Queue* Q){
    Node *current = Q->front;
    int i = 0;
    printf("\n\n\t\t");
    while (i < Q->length){
        printf(" {%d} ", current->row);
        i += 1;
        current = current->next;
    }
    printf("\n");
}

void insertNode(Queue* Q, int row){
    printf("EE(IN)");
    Node *new = new_node(row,Q->front,NULL);
    if (Q->length == 0){
        new->prev = new;
        new->next = new;
        Q->front = new;
    } else {
        new->next = Q->front;
        new->prev = Q->front->prev;
    }
    Q->front = new;
    Q->length ++;
    return;
}

int fixBoard(Queue* Q, int MaxAttempts){
    int i = 0;
    int attempts = 0;
    int last = Q->n;
    while (attempts < MaxAttempts){
        printf("COUNT %d", Q->unfoundCount);
        PRINT_ARRAY(Q->board,Q->n);
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
        int backup = 0;
        srand(time(NULL) + clock());
        if (Q->unfoundCount > 1){
            inx = rand() % (Q->unfoundCount);
            col = Q->unfound[inx]; //Random Selection
        }
        printf("HELLO");
        if (testRows(Q, col, 1) == 0){
            int srow = rand() % Q->n; //Begin Search At Random Position
            int row = srow; 
            int found = 0; //Set to False
            int br = 0;
            while (!found){
                printf("-");
                int ld = row - col + (Q->n - 1);
                int rd = row + col;
                if ((Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 1){ //Since we know there are no 0 options left and there is a Queen in most rows
                    int conflict = Q->rowConstraints[row] + Q->leftConstraints[ld] + Q->rightConstraints[rd] - 1;
                    if (conflict != last){
                        printf("A");
                        if (Q->rowConstraints[row] == 0){
                            findAndReplace(Q, row, Q->board[conflict]);
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
                    
                } else if (!backup && (Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 2){
                    br = row + 1;
                    backup = 1;
                }


                if (!found && row + 1 == srow){
                    printf("\t BB");
                    if (br != 0){
                        int brow = br - 1;
                        ld = brow - col + (Q->n - 1);
                        rd = brow + col;
                        int c1 = 0;
                        int c2 = 0;
                        printf("\nROW %d -> [%d][%d][%d]\n", brow, Q->leftConstraints[ld], Q->rightConstraints[rd], Q->rowConstraints[brow]);
                        if (Q->leftConstraints[ld] == 0){
                            printf("Z");
                            insertNode(Q, Q->board[Q->rightConstraints[rd] - 1]);
                            c1 = Q->rightConstraints[rd] - 1;
                            c2 = Q->rowConstraints[brow] - 1;
                            

                        } else if (Q->rightConstraints[rd] == 0) {
                            printf("X");
                            c1 = Q->leftConstraints[ld] - 1;
                            c2 = Q->rowConstraints[brow] - 1;

                            insertNode(Q, Q->board[Q->leftConstraints[ld] - 1]);
                        } else {
                            printf("T");
                            findAndReplace(Q, brow, Q->board[Q->rightConstraints[rd] - 1]);
                            insertNode(Q, Q->board[Q->leftConstraints[ld] - 1]);

                            c1 = Q->rightConstraints[rd] - 1;
                            c2 = Q->leftConstraints[ld] - 1;
                        }

                        int cr1 = Q->board[c1];
                        int cr2 = Q->board[c2];


                        Q->leftConstraints[cr1 - c1 + (Q->n - 1)] = 0; //RESET
                        Q->rightConstraints[cr1 + c1] = 0;
                        Q->rowConstraints[cr1] = 0;

                        Q->leftConstraints[cr2 - c2 + (Q->n - 1)] = 0; //RESET
                        Q->rightConstraints[cr2 + c2] = 0;
                        Q->rowConstraints[cr2] = 0;


                        Q->rightConstraints[rd] = col + 1;
                        Q->leftConstraints[ld] = col + 1;
                        Q->rowConstraints[brow] = col + 1;

                        Q->unfound[inx] = c1;
                        int *ptr = Q->unfound + Q->unfoundCount;
                        *ptr = c2;
                        Q->unfoundCount ++;
                        Q->board[col] = brow;
                        Q->board[c1] = Q->n;
                        Q->board[c2] = Q->n;
                    } else if (Q->unfoundCount == 1){
                        printf("\nFAILURE");
                        return 0;
                    }
                    found = 1;
                }
                row = (row + 1) % Q->n;
            }
        } else {
            printf("CC");
            if (Q->unfoundCount == 1){
                printf("SEARCHING COL %d. SOLVED!\n", col);
                return 1;
            } else {
                printf("HERE\n");
                PRINT_ARRAY(Q->unfound,Q->unfoundCount);
                for (int i = inx; i < Q->unfoundCount; i++) {
                    Q->unfound[i] = Q->unfound[i + 1];
                    //printf("SEARCHING COL %d. SOLVED ROW, %d more MISSES REMAINING\n", col, Q->unfoundCount - 1);
                }
                Q->unfoundCount --;
                printf("WERE\n");
                PRINT_ARRAY(Q->unfound,Q->unfoundCount);
            }
        }
        attempts ++;
        last = col;
    }
    printf("EXCCEDED ATTEMPTS");
    return 0;

}