// File Driver to create a devince /dev/one like the /dev/zero

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");

static int device_file_major_number = 0;
static const char device_name[] = "one";

static ssize_t device_file_read(
        struct file *file_ptr,
        char __user *user_buffer,
        size_t count,
        loff_t *position) {

    // Allocate Kernel buffer
    char* ptr = (char*) vmalloc(count);

    // Fill it with one, byte per byte
    // -- Note that byte is the smallest accesible data unit
    memset(ptr, 0xFF, count);

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
    int res = 0;
    printk( KERN_NOTICE "One: register_device() is called.\n" );
    res = register_chrdev( 0, device_name, &simple_driver_fops );
    if( res < 0 ) {
        printk( KERN_WARNING "One:  can\'t register character device with error code = %i\n", res );
        return res;
    }
    device_file_major_number = res;
    printk( KERN_NOTICE "One: registered character device with major number = %i and minor numbers 0...255\n", device_file_major_number );
    return 0;
}

void unregister_device(void) {
    printk( KERN_NOTICE "One: unregister_device() is called\n" );
    if(device_file_major_number != 0) {
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
