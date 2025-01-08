#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node Node;

struct Node {
    int row;
    Node *prev;
    Node *next;

};

typedef struct Queue {
    Node *front;
    int length;
    int fault;
    int stop; //Min of length and fault
    int n;
    int *rowConstraints;
    int *leftConstraints;
    int *rightConstraints;
    int *board;
    int *boardFlip;
    int *unfound;
    int unfoundCount;
    int fails;
    

} Queue;




Queue* new_queue(int fault, int n, int *rowConstraints, int *leftConstraints, int *rightConstraints, int *board, int *boardFlip);
Node* new_node(int row, Node *prev, Node *next);


void delete_queue(Queue* Q);
void delete_node(Node* N);
int testRows(Queue* Q, int col, int attempts);
void printBoard(Queue* Q);
int fixBoard(Queue* Q, int MaxAttempts);




#endif