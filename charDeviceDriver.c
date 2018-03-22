// COP 4600 - Programming Assignment 2
// Students: Serra Abak, Ian Lewis, Jonathan Killeen
// Instructor: Matthew B. Gerber Ph.D.

// Assignment: Write a character-mode Linux device driver as a kernel module.

// Code structure and organization based on references from:
// Peter Jay Salzman's and Michael Burian, Ori Pomerantz's "The Linux Kernel Module Programming Guide"
// http://www.tldp.org/LDP/lkmpg/2.6/html/x569.html

#include <linux/module.h>
#include <linux/tty.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define DEVICE_NAME "charDeviceDriver"
#define BUFFER_SIZE 1024
#define EBUSY 16


// Represents a circular buffer
typedef struct cbuffer_t {
    char * buffer;      // the buffer
    int start;          // index of the first effective first element of the buffer
    int end;            // index of the effective last element of the buffer
    int buffersize;     // total number of cells in the buffer
    int charsinbuffer;  // total number of filled cells in the buffer
} cbuffer_t;

// Function prototypes
void cleanup_module(void);
int init_module(void);
int openDevice(struct inode *inode, struct file *file);
int closeDevice(struct inode *inode, struct file *file);
int readFromDevice(struct file *, char *, size_t, loff_t *);
int writeToDevice(struct file *, const char *, size_t, loff_t *);

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

// File operations defined for use in our program
struct file_operations fops = {
    .read = readFromDevice,
    .write = writeToDevice,
    .open = openDevice,
    .release = closeDevice
};

// Global variables

int driverNumber;                               // Device Driver Number
char buffer[BUFFER_SIZE];                       // Character Buffer
bool deviceOpen = false;                        // Checks if a device is in use
cbuffer_t msg;                                  // Circle buffer for input


// First method to be called when the module is loaded
int init_module(void)
{
    driverNumber = register_chrdev(0, DEVICE_NAME, &fops);
    
    // Error-check
    if(driverNumber < 0)
    {
        printk(KERN_INFO "Device failed to register: %d\n", driverNumber);
        
        return driverNumber;
    }
    // Success
    else
    {
        printk(KERN_INFO "Module successfully loaded!\n");

        return 0;
    }
}

// Method to be called when module is unloaded
void cleanup_module(void)
{
    unregister_chrdev(driverNumber, DEVICE_NAME);
    
    printk(KERN_ALERT "Module successfully unloaded.\n");
}

// Method called when opening a device
int openDevice(struct inode *inode, struct file *file)
{
    if(deviceOpen)
    {
        printk(KERN_ALERT "A device is currently opened in this module.\n");
        
        return EBUSY;
    }
        
    else
    {
        try_module_get(THIS_MODULE);
        
        printk(KERN_INFO "The device was successfully opened!\n");
        
        deviceOpen = true;
        
        return 0;
    }
}

// Method called when closing the current device
int closeDevice(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    
    printk(KERN_ALERT "The device was successfully released!\n");
    
    deviceOpen = false;
    
    return 0;
}

// Method called when reading from the device to the buffer
int readFromDevice(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    // Number of bytes written to buffer
    int bytes_read = 0;


    return 0;
}

int writeToDevice(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    // TODO
    return 0;
}


// Functions
// Creates a cbuffer of size n and returns a pointer to it. Returns NULL if an error occurs.
cbuffer_t* createCirBuffer(int n) {
    cbuffer_t * cb = (cbuffer_t*) kmalloc(sizeof(cbuffer_t), GFP_KERNEL);

    // allocate space for the cbs character array
    cb->buffer = (char *) kmalloc(sizeof(char)*n, GFP_KERNEL);

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
            kfree(cb->buffer);
        }
        kfree(cb);
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
    printk("[");

    if(cb->charsinbuffer > 0){
        // 1 or more chars

        if(cb->charsinbuffer == 1){
            // 1 char
            printk("%d",cb->buffer[cursor]);

        }else{
            // 2 or more chars
            cursor = cb->start;

            while(cursor != cb->end){
                printk("%d,",cb->buffer[cursor]);
                cursor = (cursor + 1) % cb->buffersize;
            }

            printk("%d",cb->buffer[cursor]);


        }

    }else{
        // 0 chars

    }
    printk("]\n");

}

// Prints the whole actual buffer starting from index 0 to end of the buffer
void printActualBuffer(cbuffer_t *cb){
    int i;
    int f;
    printk("[");

    if(cb->buffersize > 0){
        // 1 or more chars
        f = cb->buffersize - 2;
        for(i = 0; i < f; i++){
            printk("%d,",cb->buffer[i]);
        }
        i++;
        printk("%d",cb->buffer[i]);
    }else{
        // 0 chars

    }
    printk("]\n");

}

// Reads but does not remove the first character of the cbuffer and returns it. Returns -1 if an error occurs.
char peekBuffer(cbuffer_t *cb){
    char temp = -1;

    if(cb->charsinbuffer > 0){
        temp = cb->buffer[cb->start];
    }

    return temp;
}