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

Queue* new_queue(int fault, int n, char* fileName){

    /*
    The contraints will store the location (col) of the Queens which is currently constraining it.
    Thus there may only ever be one Queen in a slot at a time, should this be voilated it will erase
    the other queen at its location and coresponding contraints. This allows for extremly quick look-up
    times and corrections.
    */

    //Create New Class Variables
    Queue* q = (Queue*) malloc(sizeof(Queue)); 
    int m = n * 2 - 1;
    q->front = NULL;
    q->length = 0;
    q->fault = fault;
    q->n = n;
    q->rowConstraints = (int *) malloc(n * sizeof (int *)); // Constraint of Queens in same row
    q->rightConstraints = (int *) malloc(m * sizeof (int *)); //Constraint of Queens in same RD
    q->leftConstraints = (int *) malloc(m * sizeof (int *)); //Constraint of Queens in same LD
    q->board = (int *) malloc(n * sizeof(int*));
    q->fails = 0;
    q->unfound = (int*)malloc(sizeof(int) * 0);
    q->unfoundCount = 0;
    q->xx = 0;

    //Create Debuging File
    q->file = fopen(fileName, "w");
        if (q->file == NULL) {
        perror("Error opening file");
        return NULL;
    }

   //Initilize arrays to 0
    memset(q->rowConstraints, 0, q->n * sizeof (int *));
    memset(q->leftConstraints, 0, m * sizeof (int *));
    memset(q->rightConstraints, 0, m * sizeof(int *));

    printf("\nMemory Allocated");
    
    //Generate one node for every unused row (n)
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

    printf("\nNodes Created");

    q->stop = MIN(n, fault); //Set the stop threshhold to the minium of fault tolerance and size of board
    
    return q;
}

void delete_node(Node* n){
    
    if (n != NULL) {  //If it exists
        free(n);
    }
}

void delete_queue_verify(Queue* q){
    /*
    This function will delete the Queue and free all of the allocated memeory. It will also 
    use the external verify function to check if the board is valid before fully erasing.
    To work within space limitations, it will erase all non-needed elemets before calling verify
    */
    if (q != NULL){
        Node* at = q->front;
        while (at != NULL){ //Loop through all elements and delete
            Node* next = at->next;
            delete_node(at);
            at = next;
        }

        //Free allocated arrays
        free(q->leftConstraints);
        free(q->rightConstraints);
        free(q->rowConstraints);
        free(q->unfound);

        //Call external verify before deleting board
        if (externalVerify(q->board,q->n)){
            printf("\nBOARD IS VERIFIED TO BE CORRECT!");
        } else {
            printf("\nBOARD WAS NOT VERIFIED TO BE CORRECT - FAILURE!");
        }


        free(q->board);

        free(q);
    }

    //Close Debuging File
    if (fclose(q->file) != 0) {
        perror("Error closing file");
    }
}

int testRows(Queue* Q, int col, int solve){
    /*
    Given a column, this function will find a row to place it in which has 0 contraints. Should
    there be no element found within the stop range it will terminate and leave the column unassigned.

    The unfound Array keeps track of all of these columns which at any point failed to find a currently
    existing solution, and marks it for later solving.

    The linklist simplifys the searching process, since the list only keeps elements (rows) which have
    no conflicts, then checking for the rest of the elements. Since the list is based upon rows, the 
    problem set is still O(n), which does not increase the runtime. However it means with each iteration
    it should grow no harder to find a solution as the possible elements are already reduced. The circular
    list also updates itself to reflect the front as the next row which should be checked, based upon
    the circular nature it has a high likleyhood of success.
    */
    int i = 0;
    Node *current = Q->front;
    while (i <= Q->stop && current){
        //Calculate constraints
        int row = current->row;
        int ld = row - col + (Q->n - 1);
        int rd = row + col;        
        if (Q->leftConstraints[ld] == 0 && Q->rightConstraints[rd] == 0){ //Row must already be 0, so check to see if other constraints are as well
                Q->front = current;
                if (!findAndDelete(Q,current->row)){
                    perror("ERROR FOUND");
                    return 0;
                }
                Q->board[col] = row;
                Q->rowConstraints[row] = col + 1; //Set value = to col + 1, since 0 index would not allow disinction from unassigned
                Q->leftConstraints[ld] = col + 1;
                Q->rightConstraints[rd] = col + 1;

                if (Q->n > 10000000 && row%(Q->n/10) == 9){ //Should problem size be large enough print progress
                    Q->xx++;
                    printf("\n\tCOMPLETED %d0%%", Q->xx);
                }
                if (solve){
                    fprintf(Q->file,"\tSEARCHING COL %d. SOLVED WITH ROW %d;", col,row);
                }
                return 1;
        }
        current = current->next;
        i++;
    }
    if (!solve){
        //Update unsolved column list
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
    /*
    Given a row number, find its element and delete the value. Since the queue is always updating it cannot be generally far away
    */
    if (Q->length == 0){
        printf("Queue is empty");
        return 0;
    }

    Node *start = Q->front;
    Node *current = Q->front;
    do {
        if (current->row == row){
        
            if (Q->length == 1){
                //If there is one element, remove front pointer as nothing will be left
                Q->front = NULL;
            } else {
                //Otherwise update existsing pointers remmove the found node
                current->prev->next = current->next;
                current->next->prev = current->prev;
                Q->front = current->next;
            }

            Q->length--;
            delete_node(current);
            Q->stop = MIN(Q->length, Q->fault);
            return 1;
        }
        current = current->next;


    } while (start != current);
    printf("COULD NOT FIND row %d\n", row);
    return 0;

}

int findAndReplace(Queue* Q, int row, int row2){
    /*
    Find a Node matching the row, and replace the integer with row2, keeping everything else intact
    */
    if (Q->length == 0){
        printf("\nTHE QUEUE IS EMPTY\n");
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
    return 0;

}

void printQueue(Queue* Q){
    /*
    Print the current state of the queue to the debugging file
    */
    if (Q->length == 0){
        fprintf(Q->file,"\nTHE QUEUE IS EMPTY\n");
        return;
    }

    Node *current = Q->front;
    int i = 0;

    while (i < Q->length){
        fprintf(Q->file,"{%d} ", current->row);
        i += 1;
        current = current->next;
    }
}

void insertNode(Queue* Q, int row){
    /*
    Insert a node into the Queue, place it at the current front of the list and update other pointers
    accordinly.
    */
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
    /*
    Solves incorrect columns until a solution is reached or Max Attempts allowed is exeeded! Follows the process of:
    1. Selects random unsolved column to start search on
    2. Run TestRows to determine if there exists a solution to this column
    3. Otherwise search all elements of the array, starting from random row to find an element which has total conflict of 1.
        a. If the conlfict is on Left/Right replace the current node with the conflicting row element.
        b. If conflict is on row, do nothing.
    4. Otherwise Otherwise take the very first element found in previous search with two conflicts
        a. Should there be a conflict with Left, Right; Overwrite the current node with left conflict, and insert right conflict (or vise versa)
        b. Should there be a conflict with Left, Row; Keep row node, and insert Right conflict
        c. Should there be a conflict with Right, Row; Keep row node, and insert Left conflict\
        d. NOTE: There cannot be double conflict in same constraint array due to orginal limitations. Does not impact ability to solve
    */
    int i = 0;
    int attempts = 0;
    int last = Q->n;
    int xx = 0;
    while (attempts < MaxAttempts){
        fprintf(Q->file,"\n\nQUEUE: ");
        printQueue(Q);
        int inx = 0;
        int col = Q->unfound[0];
        int backup = 0;
        //Generate random column to search
        srand(time(NULL) + clock());
        if (Q->unfoundCount > 1){
            inx = rand() % (Q->unfoundCount);
            col = Q->unfound[inx]; //Random Selection
        }

        if (testRows(Q, col, 1) == 0){
            //No solution was found within 0 so look for 1
            int srow = rand() % Q->n; //Begin Search At Random Position
            int row = srow; 
            int found = 0; //Set to False
            int br = 0;
            while (!found){
                //Calculate contraints
                int ld = row - col + (Q->n - 1);
                int rd = row + col;
                if ((Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 1){ //If found solution to 1 conflict
                    int conflict = Q->rowConstraints[row] + Q->leftConstraints[ld] + Q->rightConstraints[rd] - 1; //Calculate column of conflict
                    if (conflict != last){
                        if (Q->rowConstraints[row] == 0){ //Replace if conlfict of Left/Right
                            if (!findAndReplace(Q, row, Q->board[conflict])){
                                perror("ERROR FOUND");
                                return 0;
                            }
                        }
                        //Otherwise update the contraints and conflicts accordinly
                        int cr = Q->board[conflict];
                        Q->leftConstraints[cr - conflict + (Q->n - 1)] = 0; //RESET
                        Q->rightConstraints[cr + conflict] = 0;
                        Q->rowConstraints[cr] = 0;

                        found = 1;

                        Q->unfound[inx] = conflict; //Add to solving Queue - overwrite (net 0)
                        Q->rowConstraints[row] = col + 1; //Update variables
                        Q->leftConstraints[ld] = col + 1;
                        Q->rightConstraints[rd] = col + 1;
                        Q->board[conflict] = Q->n;
                        Q->board[col] = row;
                        fprintf(Q->file,"\tSEARCHING COL %d; STARTED AT %d; FOUND ROW %d; CONFLICTING WITH COL %d != %d\t|", col, srow, row, conflict, last);
                    }
                    
                } else if (!backup && (Q->leftConstraints[ld] != 0) + (Q->rightConstraints[rd] != 0) + (Q->rowConstraints[row] != 0) == 2){
                    //If there has yet to be a 'next-best' solution found, save the state to be used later if needed. Update flag
                    br = row + 1;
                    backup = 1;
                }


                if (!found && row + 1 == srow){
                    if (br != 0){
                        //There was no solution found within rows = 1 or 0, so greedily take best found = 2 (first match).
                        int brow = br - 1;
                        ld = brow - col + (Q->n - 1);
                        rd = brow + col;
                        int c1 = 0; //c1 represents the first conflicting column
                        int c2 = 0; //c2 represents the second conflicting column
                        fprintf(Q->file,"\tSEARCHING COL %d; STARTED AT %d; FOUND BACKUP ROW %d -> [%d][%d][%d]\t|", col, srow, brow, Q->leftConstraints[ld], Q->rightConstraints[rd], Q->rowConstraints[brow]);
                        if (Q->leftConstraints[ld] == 0){
                            //Insert right, and update c1, c2 for later changes
                            fprintf(Q->file,"-> LEFT ZERO");
                            insertNode(Q, Q->board[Q->rightConstraints[rd] - 1]);
                            c1 = Q->rightConstraints[rd] - 1;
                            c2 = Q->rowConstraints[brow] - 1;
                            

                        } else if (Q->rightConstraints[rd] == 0) {
                            //Insert left, and update c1, c2 for later changes
                            fprintf(Q->file,"-> RIGHT ZERO");
                            c1 = Q->leftConstraints[ld] - 1;
                            c2 = Q->rowConstraints[brow] - 1;

                            insertNode(Q, Q->board[Q->leftConstraints[ld] - 1]);
                        } else {
                            //Insert right, and replace (row -> left), then update c1, c2 for later changes
                            fprintf(Q->file,"-> ROW ZERO");
                            if (!findAndReplace(Q, brow, Q->board[Q->rightConstraints[rd] - 1])){
                                perror("ERROR FOUND");
                                return 0;
                            }
                            insertNode(Q, Q->board[Q->leftConstraints[ld] - 1]);

                            c1 = Q->rightConstraints[rd] - 1;
                            c2 = Q->leftConstraints[ld] - 1;
                        }

                        //Update the constraints of c1 and c2 and found col/row

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
            //There was match = 0 found!
            if (Q->unfoundCount == 1){
                return 1;
            } else {
                for (int i = inx; i < Q->unfoundCount; i++) {
                    Q->unfound[i] = Q->unfound[i + 1];
                }
                Q->unfoundCount --;
            }
        }
        attempts ++;
        last = col;
    }
    printf("EXCCEDED ATTEMPTS");
    return 0;

}

int externalVerify(int* board, int n){
    /*
    Externally verify board validity by recreating the contraints and checking to see if there are
    any which voilate.
    */
    int m = n * 2 - 1;
    int *rowConstraints = (int *) malloc(n * sizeof (int *));
    int *rightConstraints = (int *) malloc(m * sizeof (int *));
    int *leftConstraints = (int *) malloc(m * sizeof (int *));

    memset(rowConstraints, 0, n * sizeof (int *));
    memset(leftConstraints, 0, m * sizeof (int *));
    memset(rightConstraints, 0, m * sizeof(int *));

    for (int col = 0; col < n; col++){
        int row = board[col];
        rowConstraints[row] += 1;
        rightConstraints[row + col] += 1;
        leftConstraints[row - col + n - 1] += 1;
    }

    for (int col = 0; col < n; col++){
        int row = board[col];
        if (rowConstraints[row] + rightConstraints[row + col] + leftConstraints[row - col + n - 1] != 3){
            return 0;
        }
    }
    return 1;
}

int main(){
    int n;
    int fault;
    int steps;
    getInput("Enter the size of the Board: ", &n);
    getInput("Enter the fault tolerance: ", &fault);
    getInput("Enter the max steps allowed: ", &steps);
    Queue *Q = new_queue(fault, n,"debug.txt");

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
    delete_queue_verify(Q);
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