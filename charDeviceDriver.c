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

#define DEVICE_NAME "charDeviceDriver"
#define BUFFER_SIZE 1024
#define EBUSY 16

void cleanup_module(void);
int init_module(void);
int openDevice(struct inode *inode, struct file *file);
int closeDevice(struct inode *inode, struct file *file);
int readFromDevice(struct file *, char *, size_t, loff_t *);
int writeToDevice(struct file *, const char *, size_t, loff_t *);

int driverNumber;                               // Device Driver Number
char buffer[BUFFER_SIZE];                       // Character Buffer
bool deviceOpen = false;                        // Checks if a device is in use

// File operations defined for use in our program
struct file_operations fops = {
    .read = readFromDevice,
    .write = writeToDevice,
    .open = openDevice,
    .release = closeDevice
};

// First method to be called when the module is loaded
int init_module(void)
{
    driverNumber = register_chrdev(0, DEVICE_NAME, &fops);
    
    if(driverNumber < 0)
    {
        printk(KERN_ALERT "Device failed to register: %d\n", driverNumber);
        
        return driverNumber;
    }
    
    else
    {
        printk(KERN_ALERT "Module successfully loaded!");
        
        return 0;
    }
}

// Method to be called when module is unloaded
void cleanup_module(void)
{
    unregister_chrdev(driverNumber, DEVICE_NAME);
    /*
    if(temp < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", temp);
    
    else
        printk(KERN_ALERT "Module successfully unloaded.");*/
}

// Method called when opening a device
int openDevice(struct inode *inode, struct file *file)
{
    if(deviceOpen)
    {
        printk(KERN_ALERT "A device is currently opened in this module.");
        
        return EBUSY;
    }
        
    else
    {
        try_module_get(THIS_MODULE);
        
        printk(KERN_ALERT "The device was successfully opened!");
        
        deviceOpen = true;
        
        return 0;
    }
}

// Method called when closing the current device
int closeDevice(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    
    printk(KERN_ALERT "The device was successfully released!");
    
    deviceOpen = false;
    
    return 0;
}

// Method called when reading from the device to the buffer
int readFromDevice(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    // TODO
    return 0;
}

int writeToDevice(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    // TODO
    return 0;
}
