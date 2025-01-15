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
    int xx;
    FILE *file;
    

} Queue;




Queue* new_queue(int fault, int n, char* fileName);
Node* new_node(int row, Node *prev, Node *next);
void insertNode(Queue* Q, int row);
int findAndDelete(Queue* Q, int row);
int externalVerify(int* board, int n);
void getInput(char* message, int* data);
void delete_queue_verify(Queue* q);
void delete_node(Node* N);
int testRows(Queue* Q, int col, int attempts);
void printBoard(Queue* Q);
int fixBoard(Queue* Q, int MaxAttempts);
void printQueue(Queue* Q);



#endif