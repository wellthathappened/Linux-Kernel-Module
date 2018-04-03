// COP 4600 - Programming Assignment 3
// Students: Serra Abak, Ian Lewis, Jonathan Killeen
// Instructor: Matthew B. Gerber Ph.D.

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
#include <linux/mutex.h>	

#define DEVICE_NAME "wochardevdriver"
#define BUFFER_SIZE 1024
#define EBUSY 16

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("SA, IL, JK");  
MODULE_DESCRIPTION("Linux write only char driver PA 3"); 
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

// Writes passed character to end of cbuffer and returns the passed character if space is available. Returns -1 if an error occurs.
int writeToBuffer(cbuffer_t *cb, char c);

































// Function prototypes
void cleanup_module(void);
int init_module(void);
int openDevice(struct inode *inode, struct file *file);
int closeDevice(struct inode *inode, struct file *file);
int writeToDevice(struct file *, const char *, size_t, loff_t *);

// File operations defined for use in our program
struct file_operations fops = {
    .write = writeToDevice,
    .open = openDevice,
    .release = closeDevice
};

// Global variables
	
int driverNumber;                               // Device Driver Number
char buffer[BUFFER_SIZE];                       // Character Buffer
bool deviceOpen = false;                        // Checks if a device is in use



// First method to be called when the module is loaded
int init_module(void)
{
    driverNumber = register_chrdev(0, DEVICE_NAME, &fops);
	
    // Error-check
    if(driverNumber < 0)
    {
        printk(KERN_INFO "writechar: Device failed to register: %d\n", driverNumber);
        
        return driverNumber;
    }
    // Success
    else
    {
        printk(KERN_INFO "writechar: Module successfully loaded!\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, driverNumber);
        return 0;
    }



}

// Method to be called when module is unloaded
void cleanup_module(void)
{
    unregister_chrdev(driverNumber, DEVICE_NAME);

    printk(KERN_ALERT "writechar: Module successfully unloaded.\n");
}

// Method called when opening a device
int openDevice(struct inode *inode, struct file *file)
{
    if(deviceOpen)
    {
        printk(KERN_ALERT "writechar: A device is currently opened in this module.\n");
        
        return EBUSY;
    }
        
    else
    {
        try_module_get(THIS_MODULE);
        
        printk(KERN_INFO "writechar: The device was successfully opened!\n");
        
        deviceOpen = true;
        
        return 0;
    }
}

// Method called when closing the current device
int closeDevice(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    
    printk(KERN_ALERT "writechar: The device was successfully released!\n");
    
    deviceOpen = false;
    
    return 0;
}


int writeToDevice(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    int i = 0;
	int temp = 0;
	
	extern cbuffer_t *cb;
	
	extern struct mutex cb_mutex; 	
	
	printk(KERN_INFO "writechar: Received %d characters from the user\n", length);
	//printk(KERN_INFO "writechar: Current CB count before write is: %d\n", cb->charsinbuffer);
	
	// attempt to access and lock the shared cb
	//extern struct *test temp;
	//cb = createCirBuffer(BUFFER_SIZE);
	//cbuffer_t* createCirBuffer
	//static cbuffer_t *cb = NULL;  
	
	
	// Try to acquire the mutex (i.e., put the lock on/down)
	printk(KERN_INFO "writechar: locking mutex\n");
	if(!mutex_trylock(&cb_mutex) ){    
	
		printk(KERN_ALERT "writechar: failed to lock mutex");
		return -1;
				
	}else{
		printk(KERN_INFO "writechar: mutex locked\n");
		printk(KERN_INFO "writechar: adding char to cb: %d\n", temp);
			
		for(i = 0; i < length; i++){
			temp =  writeToBuffer(cb, buffer[i]);
			printk(KERN_INFO "writechar: adding char to cb: %d\n", temp);
			//printEffectiveBuffer(cb);
			if(temp == -1){
				printk(KERN_INFO "writechar: cb is full\n");
				break;
			}
			
		}
		printk(KERN_INFO "writechar: Current CB count after write is: %d\n", cb->charsinbuffer);

		printk(KERN_INFO "writechar: unlocking mutex\n");		
		mutex_unlock(&cb_mutex);          /// Releases the mutex (i.e., the lock goes up)
		printk(KERN_INFO "writechar: mutex unlocked\n");
	}
	

    return 0;
}

//========================================================================================================================================================================================================
//========================================================================================================================================================================================================
//========================================================================================================================================================================================================

// Functions


// Writes passed character to end of cbuffer and returns the passed character if space is available. Returns -1 if an error occurs.
int writeToBuffer(cbuffer_t *cb, char c){

    if(cb->charsinbuffer < cb->buffersize){
        // buffer is not full
        if(cb->charsinbuffer == 0){
            // buffer is empty
            cb->buffer[cb->start] = c;
	cb->end = cb->start;
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
