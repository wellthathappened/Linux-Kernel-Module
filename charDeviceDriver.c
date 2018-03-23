
#include <linux/init.h>           
#include <linux/module.h>      
#include <linux/device.h>  
#include <linux/kernel.h>       
#include <linux/fs.h>             
#include <linux/uaccess.h>          
#include <linux/vmalloc.h>




//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
// The stuff here is from the Molly example and should not be submitted. It's only purpose was to get the project up and running to test the read/write functions


// Constants
#define DEVICE_NAME "charMode"    
#define	CLASS_NAME  "chrm"       
#define	CB_BUFFER_SIZE 1024

// Module information
MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Jonathan Killeen");  
MODULE_DESCRIPTION("Linux char driver PA2"); 
MODULE_VERSION("1");     

// Device number
static int    majorNumber;                 

// Counts the number of times the device is opened
static int    numberOpens = 0;  

// pointer to device driver class
static struct class*  charModeClass  = NULL; 

// pointer to device driver object
static struct device* charModeDevice = NULL; 

// prototype functions for device driver
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// The stuff here is from the Molly example and should not be submitted. It's only purpose was to get the project up and running to test the read/write functions
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================









//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
// This is code from the circular buffer c file


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
int writeToBuffer(cbuffer_t *cb, char c);

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

// This is code from the circular buffer c file
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================











// global circular buffer pointer
cbuffer_t *cb;















//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
// The stuff here is from the Molly example and should not be submitted. It's only purpose was to get the project up and running to test the read/write functions

// define the functions of the module 
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

// initializes the module
static int __init charMode_init(void){
   printk(KERN_INFO "charMode: Initializing the charMode LKM\n");

   // dynamically allocate a major number for the device
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   
   if (majorNumber<0){
      printk(KERN_ALERT "charMode failed to register a major number\n");
      return majorNumber;
   }
   
   printk(KERN_INFO "charMode: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   charModeClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(charModeClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(charModeClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "charMode: device class registered correctly\n");

   // Register the device driver
   charModeDevice = device_create(charModeClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   
   // perform cleanup on error
   if (IS_ERR(charModeDevice)){        
      class_destroy(charModeClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(charModeDevice);
   }
   
   printk(KERN_INFO "charMode: device class created correctly\n"); 

	// initialize the CB
	cb = createCirBuffer(10);

   return 0;
}

// exit function for module
static void __exit charMode_exit(void){
	// destroy the device
	device_destroy(charModeClass, MKDEV(majorNumber, 0)); 
    
	// unregister the device class
	class_unregister(charModeClass);                          
	
	// destroy the device class
	class_destroy(charModeClass);      

	// unregister the device                       
	unregister_chrdev(majorNumber, DEVICE_NAME);  
           
	printk(KERN_INFO "charMode: Device exiting\n");
   
	// destroy the circular buffer
	destroyCirBuffer(cb);
}

// open function for module
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;

   printk(KERN_INFO "charMode: Device has been opened %d time(s)\n", numberOpens);
   
   return 0;
}

// release function for module
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "charMode: Device successfully closed\n");
   return 0;
}


// The stuff here is from the Molly example and should not be submitted. It's only purpose was to get the project up and running to test the read/write functions
//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================














// read function for the module
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){

	int i = 0;
	char temp = -2;

	printk(KERN_INFO "charMode: Received request for %zu characters from the user\n", len);
	printk(KERN_INFO "charMode: Current CB count before read is: %d\n", cb->charsinbuffer);

	for(i = 0; i < len; i++){
		temp =  readFromBuffer(cb);
		
		if(temp == -1){
			buffer[i] = '\0';
			break;
		}else{
			buffer[i] = temp;
		}
		
		
	}

	printk(KERN_INFO "charMode: Current CB count after read is: %d\n", cb->charsinbuffer);
	return 0;
}

// write function for the module
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int i = 0;
	int temp = 0;
	printk(KERN_INFO "charMode: Received %zu characters from the user\n", len);
	printk(KERN_INFO "charMode: Current CB count before write is: %d\n", cb->charsinbuffer);
	

	for(i = 0; i < len; i++){
		temp =  writeToBuffer(cb, buffer[i]);
		
		if(temp == -1){
			break;
		}
		
	}	
	printk(KERN_INFO "charMode: Current CB count after write is: %d\n", cb->charsinbuffer);
	
	return 0;
}


// exit
module_init(charMode_init);
module_exit(charMode_exit);
