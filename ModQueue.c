#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()

#define MIN(a, b) ((a) < (b) ? (a) : (b))
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

Queue* new_queue(int fault, int n){
    Queue* q = (Queue*) malloc(sizeof(Queue));
    int m = n * 2 - 1;
    q->front = NULL;
    q->length = 0;
    q->fault = fault;
    q->n = n;
    q->rowConstraints = (int *) malloc(n * sizeof (int *));
    q->rightConstraints = (int *) malloc(m * sizeof (int *));
    q->leftConstraints = (int *) malloc(m * sizeof (int *));
    q->board = (int *) malloc(n * sizeof(int*));
    q->fails = 0;
    q->unfound = (int*)malloc(sizeof(int) * 0);
    q->unfoundCount = 0;
    q->xx = 0;

   
    memset(q->rowConstraints, 0, q->n * sizeof (int *));
    memset(q->leftConstraints, 0, m * sizeof (int *));
    memset(q->rightConstraints, 0, m * sizeof(int *));
    
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

    q->stop = MIN(n, fault);
    
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

        free(q->board);
        free(q->leftConstraints);
        free(q->rightConstraints);
        free(q->rowConstraints);
        free(q->unfound);
        free(q);
    }
}

int testRows(Queue* Q, int col, int solve){
    // printQueue(Q);
    // PRINT_ARRAY(Q->board,Q->n);
    // printf("\n");
    // PRINT_ARRAY(Q->rowConstraints,Q->n);
    // printf("\n");
    // PRINT_ARRAY(Q->leftConstraints,Q->n);
    // printf("\n");
    // PRINT_ARRAY(Q->rightConstraints,Q->n);
    // printf("\n");
    // PRINT_ARRAY(Q->unfound,Q->unfoundCount);
    // printf("\n");
    int i = 0;
    Node *current = Q->front;
    while (i <= Q->stop && current){
        //printf("\n{%d} PREV NODE %d CURRENT NODE %d NEXT NODE %d",i,  current->prev, current, current->next);

        int row = current->row;
        int ld = row - col + (Q->n - 1);
        int rd = row + col;

        //printf("\n ATTEMPTING {%d} - ld: %d; rd: %d", row, Q->leftConstraints[ld], Q->rightConstraints[rd]);
        
        if (Q->leftConstraints[ld] == 0 && Q->rightConstraints[rd] == 0){
            // srand(time(NULL) + clock());
            // if (rand()%5 != 0){
                findAndDelete(Q,current->row);
                Q->board[col] = row;
                Q->rowConstraints[row] = col + 1;
                Q->leftConstraints[ld] = col + 1;
                Q->rightConstraints[rd] = col + 1;

                if (Q->n > 10000000 && row%(Q->n/10) == 9){
                    Q->xx++;
                    printf("\n\tCOMPLETED %d0%%", Q->xx);
                }

                //printf("\nCOL [%d] ROW [%d]",col, row);
                
                if (solve){
                    printf("\tSEARCHING COL %d. SOLVED WITH ROW %d;", col,row);
                }

                return 1;
            
        }
        current = current->next;
        i++;
    }
    if (!solve){
        int *ptr = Q->unfound + Q->unfoundCount;
        *ptr = col;
        Q->unfoundCount ++;
        Q->board[col] = Q->n;
        Q->fails ++;
        Q->front = current->next;
    }
    return 0;
}

int findAndDelete(Queue* Q, int row){
    // printf("EE(FD) -- {%d}", Q->length);
    // printf("\n\tBEFORE");
    // printQueue(Q);

    if (Q->length == 0){
        printf("Queue is empty");
        return 0;
    }

    Node *start = Q->front;
    Node *current = Q->front;
    do {
        if (current->row == row){

            if (Q->length == 1){
                Q->front = NULL;
            } else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                Q->front = current->next;
            }

            Q->length--;
            delete_node(current);
            Q->stop = MIN(Q->length, Q->fault);
            // printf("\n\tAFTER");
            // printQueue(Q);
            return 1;
        }
        current = current->next;


    } while (start != current);
    printf("COULD NOT FIND row %d\n", row);

}

int findAndReplace(Queue* Q, int row, int row2){

    if (Q->length == 0){
        printf("Queue is empty");
        return 0;
    }

    Node *start = Q->front;
    Node *current = Q->front;
    do {
        if (current->row == row){
            current->row = row2;
            return 1;
        }
        current = current->next;


    } while (start != current);
    printf("COULD NOT FIND row %d\n", row);

}

void printQueue(Queue* Q){
    if (Q->length == 0){
        printf("\nTHEQUEUEISEMPTY\n");
        return;
    }

    Node *current = Q->front;
    int i = 0;

    while (i < Q->length){
        
        printf("{%d} ", current->row);
        i += 1;
        current = current->next;
    }
}

void insertNode(Queue* Q, int row){
    //printf("EE(IN) {%d}", Q->length);
    Node *new = new_node(row,NULL,NULL);
    if (Q->length == 0){
        new->prev = new;
        new->next = new;
        Q->front = new;
    } else {
        new->prev = Q->front->prev;
        new->next = Q->front;
        Q->front->prev->next = new;
        Q->front->prev = new;
        
        
        
    }
    Q->front = new;
    Q->length ++;
    Q->stop = MIN(Q->length, Q->fault);
    return;
}

int fixBoard(Queue* Q, int MaxAttempts){
    int i = 0;
    int attempts = 0;
    int last = Q->n;
    int xx = 0;
    while (attempts < MaxAttempts){
        //printf("\n\nBOARD: ");
        //PRINT_ARRAY(Q->board, Q->n);
        printf("\n\nQUEUE: ");
        printQueue(Q);
        printf("\nUNFOUND: ");
        PRINT_ARRAY(Q->unfound, Q->unfoundCount);
        int inx = 0;
        int col = Q->unfound[0];
        int backup = 0;
        srand(time(NULL) + clock());
        if (Q->unfoundCount > 1){
            inx = rand() % (Q->unfoundCount);
            col = Q->unfound[inx]; //Random Selection
        }
        //printf("HELLO");
        if (testRows(Q, col, 1) == 0){
            int srow = rand() % Q->n; //Begin Search At Random Position
            int row = srow; 
            int found = 0; //Set to False
            int br = 0;
            while (!found){
                //printf("-");
                int ld = row - col + (Q->n - 1);
                int rd = row + col;
                if ((Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 1){ //Since we know there are no 0 options left and there is a Queen in most rows
                    //printf("HIT");
                    int conflict = Q->rowConstraints[row] + Q->leftConstraints[ld] + Q->rightConstraints[rd] - 1;
                    if (conflict != last){
                        //printf("A");
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

                        
                        printf("\tSEARCHING COL %d; STARTED AT %d; FOUND ROW %d; CONFLICTING WITH COL %d != %d\t|", col, srow, row, conflict, last);
                    }
                    
                } else if (!backup && (Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 2){
                    br = row + 1;
                    backup = 1;
                }


                if (!found && row + 1 == srow){
                    if (br != 0){
                        int brow = br - 1;
                        ld = brow - col + (Q->n - 1);
                        rd = brow + col;
                        int c1 = 0;
                        int c2 = 0;
                        printf("\tSEARCHING COL %d; STARTED AT %d; FOUND BACKUP ROW %d -> [%d][%d][%d]\t|", col, srow, brow, Q->leftConstraints[ld], Q->rightConstraints[rd], Q->rowConstraints[brow]);
                        if (Q->leftConstraints[ld] == 0){
                            printf("-> LEFT ZERO");
                            insertNode(Q, Q->board[Q->rightConstraints[rd] - 1]);
                            c1 = Q->rightConstraints[rd] - 1;
                            c2 = Q->rowConstraints[brow] - 1;
                            

                        } else if (Q->rightConstraints[rd] == 0) {
                            printf("-> RIGHT ZERO");
                            c1 = Q->leftConstraints[ld] - 1;
                            c2 = Q->rowConstraints[brow] - 1;

                            insertNode(Q, Q->board[Q->leftConstraints[ld] - 1]);
                        } else {
                            printf("-> ROW ZERO");
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
                        printf("\n\nFAILURE\n\n");
                        return 0;
                    }
                    found = 1;
                }
                row = (row + 1) % Q->n;
            }
        } else {
            //printf("CC");
            if (Q->unfoundCount == 1){
                return 1;
            } else {
                //printf("HERE\n");
                //PRINT_ARRAY(Q->unfound,Q->unfoundCount);
                for (int i = inx; i < Q->unfoundCount; i++) {
                    Q->unfound[i] = Q->unfound[i + 1];
                    //printf("SEARCHING COL %d. SOLVED ROW, %d more MISSES REMAINING\n", col, Q->unfoundCount - 1);
                }
                Q->unfoundCount --;
                //printf("WERE\n");
                //PRINT_ARRAY(Q->unfound,Q->unfoundCount);
            }
        }
        attempts ++;
        last = col;
    }
    printf("EXCCEDED ATTEMPTS");
    return 0;

}

int main(){
    int n;
    int fault;
    int steps;
    getInput("Enter the size of the Board: ", &n);
    getInput("Enter the fault tolerance: ", &fault);
    getInput("Enter the max steps allowed: ", &steps);
    Queue *Q = new_queue(fault, n);

    for (int i = 0; i < n; i++){
            testRows(Q, i, 0);
    }

    printf("\nCREATED THE GREEDY BOARD");

    int solve = fixBoard(Q, steps);

    if (solve){
        printf("\nBoard Is Solved");
    } else {
        printf("\nSize not solvable within steps allowed");
    }
    delete_queue(Q);
}

void getInput(char* message, int* data){
    do {
        printf("\n%s",message);
        if (scanf("%d", data) != 1){
            while (getchar() != '\n'); //'read' all bad values to clear
            *data = -1;
        }
    } while (*data <= 0);
    
}