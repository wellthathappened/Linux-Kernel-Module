#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


// Represents a circular buffer
typedef struct cbuffer {
    char * buffer;      // the buffer
    int start;          // index of the first effective first element of the buffer
    int end;            // index of the effective last element of the buffer
    int buffersize;     // total number of cells in the buffer
    int charsinbuffer;  // total number of filled cells in the buffer
}cbuffer_t;

// Function prototypes

// Creates a cbuffer of size n and returns a pointer to it. Returns NULL if an error occurs.
cbuffer_t* createCirBuffer(int n);

// Destroys a cbuffer.
void destroyCirBuffer(cbuffer_t *cb);

// Writes passed character to end of cbuffer and returns the passed character if space is available. Returns -1 if an error occurs.
int *writeToBuffer(cbuffer_t *cb, char c);

// Reads the last character from the cbuffer, removes it from the buffer and returns it. Returns -1 if an error occurs.
char readFromBuffer(cbuffer_t *cb);

// Prints the effective buffer starting from the starting index to the ending index of the buffer
void printEffectiveBuffer(cbuffer_t *cb);

// Prints the whole actual buffer starting from index 0 to end of the buffer
void printActualBuffer(cbuffer_t *cb);

// Reads but does not remove the first character of the cbuffer and returns it. Returns -1 if an error occurs.
char peekBuffer(cbuffer_t *cb);


// Functions
// Creates a cbuffer of size n and returns a pointer to it. Returns NULL if an error occurs.
cbuffer_t* createCirBuffer(int n) {
    int i = 0;

    cbuffer_t * cb = (struct cbuffer*) malloc(sizeof(cbuffer_t));

    // allocate space for the cbs character array
    cb->buffer = (char *) malloc(sizeof(char)*n);

    // set the cb attributes to default values
    cb->start = 0;

    cb->end = 0;

    cb->buffersize = n;

    cb->charsinbuffer = 0;

//    for(i = 0; i < n; i++){
//        cb->buffer[i] = -1;
//    }

    // if there were no issues, return a pointer to the newcbuffer
    return cb;
}

// Destroys a cbuffer.
void destroyCirBuffer(cbuffer_t *cb){
    if(cb != NULL){
        if(cb->buffer != NULL){
            free(cb->buffer);
        }
        free(cb);
    }
}

// Writes passed character to end of cbuffer and returns the passed character if space is available. Returns -1 if an error occurs.
int *writeToBuffer(cbuffer_t *cb, char c){

    if(cb->charsinbuffer < cb->buffersize){
        // buffer is not full
        if(cb->charsinbuffer == 0){
            // buffer is empty
            cb->buffer[cb->start] = c;

        }else{
            // buffer is not empty
            cb->end = (cb->end + 1) % cb->buffersize;

            // check if end is outside actual array size

            // check if end is equal to start

            cb->buffer[cb->end] = c;

        }

        cb->charsinbuffer++;

    }else{
        // buffer is full
        c = -1;
    }

    return c;
}

// Reads the last character from the cbuffer, removes it from the buffer and returns it. Returns -1 if an error occurs.
char readFromBuffer(cbuffer_t *cb){
    char bchar = -1;
    int i;

    if(cb->charsinbuffer > 0) {
        // buffer is not empty
        bchar = cb->buffer[cb->start];
        cb->start = (cb->start + 1) % cb->buffersize;
        cb->charsinbuffer--;

    }else{
        // buffer is empty

    }

    return bchar;
}

// Prints the effective buffer starting from the starting index to the ending index of the buffer
void printEffectiveBuffer(cbuffer_t *cb){
    int i;
    int f;
    int cursor;
    printf("[");

    if(cb->charsinbuffer > 0){
        // 1 or more chars

        if(cb->charsinbuffer == 1){
            // 1 char
            printf("%d",cb->buffer[cursor]);

        }else{
            // 2 or more chars
            cursor = cb->start;

            while(cursor != cb->end){
                printf("%d,",cb->buffer[cursor]);
                cursor = (cursor + 1) % cb->buffersize;
            }

            printf("%d",cb->buffer[cursor]);


        }

    }else{
        // 0 chars

    }
    printf("]\n");

}

// Prints the whole actual buffer starting from index 0 to end of the buffer
void printActualBuffer(cbuffer_t *cb){
    int i;
    int f;
    printf("[");

    if(cb->buffersize > 0){
        // 1 or more chars
        f = cb->buffersize - 2;
        for(i = 0; i < f; i++){
            printf("%d,",cb->buffer[i]);
        }
        i++;
        printf("%d",cb->buffer[i]);
    }else{
        // 0 chars

    }
    printf("]\n");

}

// Reads but does not remove the first character of the cbuffer and returns it. Returns -1 if an error occurs.
char peekBuffer(cbuffer_t *cb){
    char temp = -1;

    if(cb->charsinbuffer > 0){
        temp = cb->buffer[cb->start];
    }

    return temp;
}

// Example of the cbuffer class and functions in work
int main(){
    // create the buffer
    cbuffer_t *cb = createCirBuffer(20);

    int i = 0;
    printf("create the buffer\n");
    printf("actual buffer\n");
    printActualBuffer(cb);
    printf("effective buffer\n");
    printEffectiveBuffer(cb);
    printf("\n");

    // fill the buffer
    printf("fill the buffer\n");
    for(i = 0; i < 24; i++){
        printf("%d,", writeToBuffer(cb, i));
    }
    printf("\n");

    printf("actual buffer\n");
    printActualBuffer(cb);
    printf("effective buffer\n");
    printEffectiveBuffer(cb);
    printf("\n");

    // empty the buffer
    printf("remove from the buffer\n");
    for(i = 0; i < 15; i++){
        printf("%d,", readFromBuffer(cb));
    }
    printf("\n");

    printf("actual buffer\n");
    printActualBuffer(cb);
    printf("effective buffer\n");
    printEffectiveBuffer(cb);
    printf("\n");

    // fill the buffer
    printf("fill the buffer\n");
    for(i=60; i < 62; i++){
        printf("%d,", writeToBuffer(cb, i));
    }
    printf("\n");

    printf("actual buffer\n");
    printActualBuffer(cb);
    printf("effective buffer\n");
    printEffectiveBuffer(cb);
    printf("\n");

    // destroy the buffer
    printf("destroy buffer\n");
    destroyCirBuffer(cb);

    return 0;
}
