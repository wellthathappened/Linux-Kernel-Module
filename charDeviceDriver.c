// COP 4600 - Programming Assignment 2
// Students: Serra Abak, Ian Lewis, Jonathan Killeen
// Instructor: Matthew B. Gerber Ph.D.

// Assignment: Write a character-mode Linux device driver as a kernel module.

// Code structure and organization based on references from:
// Peter Jay Salzman's and Michael Burian, Ori Pomerantz's "The Linux Kernel Module Programming Guide"
// http://www.tldp.org/LDP/lkmpg/2.6/html/x569.html

#include <linux/modversions.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "charDeviceDriver"
#define BUFFER_SIZE 1024

void cleanup_module(void);
int init_module(void);
int writeToDevice();
int openDevice(struct inode *inode, struct file *file);
int closeDevice(struct inode *inode, struct file *file);
int readFromDevice(struct file *, char *, size_t, loff_t *);
int writeToDevice(struct file *, const char *, size_t, loff_t *);

int deviceNumber;                               // Device Driver Number
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
    deviceNumber = register_chrdev(0, DEVICE_NAME, &fops);
    
    if(deviceNumber < 0)
    {
        printk(KERN_ALERT "Device failed to register: %d\n", deviceNumber);
        
        return deviceNumber;
    }
    
    return 0;
}

// Method to be called when module is unloaded
void cleanup_module(void)
{
    int temp = unregister_chrdev(Major, DEVICE_NAME);
    
    if(temp < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", temp);
}

// Method called when opening a device
int openDevice(struct inode *inode, struct file *file)
{
    if(deviceOpen)
        return -EBusy
        
    else
    {
        try_module_get(THIS_MODULE);
        
        deviceOpen = true;
        
        return 0;
    }
}

// Method called when closing the current device
int closeDevice(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    
    deviceOpen = false;
    
    return 0;
}

// Method called when reading from the device to the buffer
int readFromDevice(struct file *, char *, size_t, loff_t *)
{
    // TODO
}

int writeToDevice(struct file *, const char *, size_t, loff_t *)
{
    // TODO
}
