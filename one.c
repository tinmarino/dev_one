/*
Brief: File Driver to create a devince /dev/one like the /dev/zero
Source: Greatly copyed from: https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os
Author: Created by Tinmarino 2020-10-12
License: Open Source
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static int device_file_major_number = 0;
static const char device_name[] = "one";

static const char    g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);

static ssize_t device_file_read(
                        struct file *file_ptr
                       , char __user *user_buffer
                       , size_t count
                       , loff_t *position) {
    printk( KERN_NOTICE "One: Device file is read at offset = %i, read bytes count = %u\n"
                , (int)*position
                , (unsigned int)count );
    /* If position is behind the end of a file we have nothing to read */
    if( *position >= g_s_Hello_World_size ){ return 0; }
    /* If a user tries to read more than we have, read only as many bytes as we have */
    if( *position + count > g_s_Hello_World_size )
        count = g_s_Hello_World_size - *position;
    if( copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0 )
        return -EFAULT;    
    /* Move reading position */
    *position += count;
    return count;
}

static struct file_operations simple_driver_fops = {
    .owner   = THIS_MODULE,
    .read    = device_file_read,
};

int register_device(void) {
    int result = 0;
    printk( KERN_NOTICE "One: register_device() is called.\n" );
    result = register_chrdev( 0, device_name, &simple_driver_fops );
    if( result < 0 ) {
        printk( KERN_WARNING "One:  can\'t register character device with error code = %i\n", result );
        return result;
    }
    device_file_major_number = result;
    printk( KERN_NOTICE "One: registered character device with major number = %i and minor numbers 0...255\n", device_file_major_number );
    return 0;
}

void unregister_device(void) {
    printk( KERN_NOTICE "One: unregister_device() is called\n" );
    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, device_name);
    }
}

static int my_init(void) {
    register_device();
    return 0;
}
    
static void my_exit(void) {
    unregister_device();
    return;
}
    
module_init(my_init);
module_exit(my_exit);
