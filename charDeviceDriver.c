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
#include <linux/init.h>               
#include <linux/device.h>           
#include <linux/vmalloc.h>

#define DEVICE_NAME "chardevdriver"
#define BUFFER_SIZE 1024
#define EBUSY 16

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("SA, IL, JK");  
MODULE_DESCRIPTION("Linux char driver PA2"); 
MODULE_VERSION("1"); 

// Represents a circular buffer
typedef struct cbuffer {
    char * buffer;      // the buffer
    int start;          // index of the first effective first element of the buffer
    int end;            // index of the effective last element of the buffer
    int buffersize;     // total number of cells in the buffer
    int charsinbuffer;  // total number of filled cells in the buffer
}cbuffer_t;


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
int writeToBuffer(cbuffer_t *cb, char c);

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
cbuffer_t *cb = NULL;                                  	// Circle buffer for input


// First method to be called when the module is loaded
int init_module(void)
{
    driverNumber = register_chrdev(0, DEVICE_NAME, &fops);
    
	// initialize the CB
	cb = createCirBuffer(BUFFER_SIZE);

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
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, driverNumber);
        return 0;
    }



}

// Method to be called when module is unloaded
void cleanup_module(void)
{
    unregister_chrdev(driverNumber, DEVICE_NAME);
    // destroy the circular buffer
	destroyCirBuffer(cb);
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
	
	int i = 0;
	char temp = -2;

	printk(KERN_INFO "charMode: Received request for %zu characters from the user\n", length);
	printk(KERN_INFO "charMode: Current CB count before read is: %d\n", cb->charsinbuffer);

	if(cb->charsinbuffer > 0){
		temp =  readFromBuffer(cb);
		while(i < length && cb->charsinbuffer > 0){		
			//printk(KERN_INFO "charMode: adding char to buffer: %d\n", temp);
			buffer[i] = temp;
			temp =  readFromBuffer(cb);
			i++;
		
		}
	}
	

	printk(KERN_INFO "charMode: adding null char to buffer\n");
	buffer[i] = '\0';

	printk(KERN_INFO "charMode: Current CB count after read is: %d\n", cb->charsinbuffer);

    return 0;
}

int writeToDevice(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    int i = 0;
	int temp = 0;
	printk(KERN_INFO "charMode: Received %zu characters from the user\n", length);
	printk(KERN_INFO "charMode: Current CB count before write is: %d\n", cb->charsinbuffer);
	

	for(i = 0; i < length; i++){
		temp =  writeToBuffer(cb, buffer[i]);
		
		if(temp == -1){
			break;
		}
		
	}	
	printk(KERN_INFO "charMode: Current CB count after write is: %d\n", cb->charsinbuffer);
	
    return 0;
}

//========================================================================================================================================================================================================
//========================================================================================================================================================================================================
//========================================================================================================================================================================================================


// Functions
// Creates a cbuffer of size n and returns a pointer to it. Returns NULL if an error occurs.
cbuffer_t* createCirBuffer(int n) {
    //THISint i = 0;

    cbuffer_t * cb = (struct cbuffer*) vmalloc(sizeof(cbuffer_t));

    // allocate space for the cbs character array
    cb->buffer = (char *) vmalloc(sizeof(char)*n);

    // set the cb attributes to default values
    cb->start = 0;

    cb->end = 0;

    cb->buffersize = n;

    cb->charsinbuffer = 0;

    // if there were no issues, return a pointer to the newcbuffer
    return cb;
}

// Destroys a cbuffer.
void destroyCirBuffer(cbuffer_t *cb){
    if(cb != NULL){
        if(cb->buffer != NULL){
            vfree(cb->buffer);
        }
        vfree(cb);
    }
}

// Writes passed character to end of cbuffer and returns the passed character if space is available. Returns -1 if an error occurs.
int writeToBuffer(cbuffer_t *cb, char c){

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
    //THISint i;

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


// Reads but does not remove the first character of the cbuffer and returns it. Returns -1 if an error occurs.
char peekBuffer(cbuffer_t *cb){
    char temp = -1;

    if(cb->charsinbuffer > 0){
        temp = cb->buffer[cb->start];
    }

    return temp;
}



// Prints the effective buffer starting from the starting index to the ending index of the buffer
void printEffectiveBuffer(cbuffer_t *cb){

    int cursor = 0;
    printk(KERN_INFO "[");

    if(cb->charsinbuffer > 0){
        // 1 or more chars

        if(cb->charsinbuffer == 1){
            // 1 char
            printk(KERN_INFO "%d",cb->buffer[cursor]);

        }else{
            // 2 or more chars
            cursor = cb->start;

            while(cursor != cb->end){
                printk(KERN_INFO "%d,",cb->buffer[cursor]);
                cursor = (cursor + 1) % cb->buffersize;
            }

            printk(KERN_INFO "%d",cb->buffer[cursor]);


        }

    }else{
        // 0 chars

    }
    printk(KERN_INFO "]\n");

}

// Prints the whole actual buffer starting from index 0 to end of the buffer
void printActualBuffer(cbuffer_t *cb){
    int i;
    int f;
    printk(KERN_INFO "[");

    if(cb->buffersize > 0){
        // 1 or more chars
        f = cb->buffersize - 2;
        for(i = 0; i < f; i++){
            printk(KERN_INFO "%d,",cb->buffer[i]);
        }
        i++;
        printk(KERN_INFO "%d",cb->buffer[i]);
    }else{
        // 0 chars

    }
    printk(KERN_INFO "]\n");

}
