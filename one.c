/*
Brief: File Driver to create a devince /dev/one like the /dev/zero
Source:
  Device Driver: https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os
  Signing driver: https://gist.github.com/Garoe/74a0040f50ae7987885a0bebe5eda1aa
  Mok: https://ubuntu.com/blog/how-to-sign-things-for-secure-boot
Author: Created by Tinmarino 2020-10-12
License: GPL => Open Source

Install:
  1/ make build        # Compile
  2/ sudo make sign    # Sign driver module to permit MOK enforcement (security)
  3/ sudo reboot now   # Reboot and enable Mok
    3.1/ 
  3/ sudo make load    # Load
  3/ sudo make device  # Create /dev/one
  4/ make test         # Test if all is ok

Play:
  cat /dev/one | hexdump -v 

Util:
  sudo mokutil --list-new
  sudo mokutil --reset
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
        struct file *file_ptr,
        char __user *user_buffer,
        size_t count,
        loff_t *position) {
    printk( KERN_NOTICE "One: Device file is read at offset = %i, read bytes count = %u\n" , (int)*position , (unsigned int)count );

    // Allocate Kernel buffer
    char* ptr = (char*) vmalloc(count);

    // Fill it with one, byte per byte
    // -- Note that byte is the smallest accesible data unit
    for (size_t i = 0; i < count; i++) {
        ptr[i] = 0xFF;
    }

    char res = copy_to_user(user_buffer, ptr, count);
    if (res != 0){ return -EFAULT; }

    // Return number of byte read
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

// Declare register and unregister command
module_init(my_init);
module_exit(my_exit);
