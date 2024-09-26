#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
MODULE_LICENSE("GPL");

static char character_info[27][16] = {
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", // q
    "1110001100011001", // R
    "1101110100010001", // S
    "1100000001000100", // T
    "0011111100000000", // U
    "0000001100100010", // V
    "0011001100001010", // W
    "0000000010101010", // X
    "0000000010100100", // Y
    "1100110000100010", // Z
    "0000000000000000"
};
// static int c_count = 0;
static char rec_buf[1] = {};
static char tar;
static char *out;



// dev_t dev = 0;
// static struct class *dev_class;
// static struct cdev etx_cdev;

// static int __init my_init(void);
// static void __exit my_exit(void);
// /*************** Driver functions **********************/
// static int (struct inode *inode, struct file *file);
// // static int etx_release(struct inode *inode, struct file *file);
// static ssize_t my_read(struct file *filp,
//                 char __user *buf, size_t len,loff_t * off);
// static ssize_t my_write(struct file *filp,
//                 const char *buf, size_t len, loff_t * off);
// /******************************************************/


// File Operations
static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos) {

    count = 16;
    tar = rec_buf[0];
    
    if(tar == 'a')
        out = character_info[0];
    else if(tar == 'b')
        out = character_info[1];
    else if(tar == 'c')
        out = character_info[2];
    else if(tar == 'd')
        out = character_info[3];
    else if(tar == 'e')
        out = character_info[4];
    else if(tar == 'f')
        out = character_info[5];
    else if(tar == 'g')
        out = character_info[6];
    else if(tar == 'h')
        out = character_info[7];
    else if(tar == 'i')
        out = character_info[8];
    else if(tar == 'j')
        out = character_info[9];
    else if(tar == 'k')
        out = character_info[10];
    else if(tar == 'l')
        out = character_info[11];
    else if(tar == 'm')
        out = character_info[12];
    else if(tar == 'n')
        out = character_info[13];
    else if(tar == 'o')
        out = character_info[14];
    else if(tar == 'p')
        out = character_info[15];
    else if(tar == 'q')
        out = character_info[16];
    else if(tar == 'r')
        out = character_info[17];
    else if(tar == 's')
        out = character_info[18];
    else if(tar == 't')
        out = character_info[19];
    else if(tar == 'u')
        out = character_info[20];
    else if(tar == 'v')
        out = character_info[21];
    else if(tar == 'w')
        out = character_info[22];
    else if(tar == 'x')
        out = character_info[23];
    else if(tar == 'y')
        out = character_info[24];
    else if(tar == 'z')
        out = character_info[25];
    else
        out = character_info[26];

    if( copy_to_user(buf, out, count) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }
    // c_count++;
    printk("call read\n");
    return count;
}

static ssize_t my_write(struct file *fp,const char *buf, size_t count, loff_t *fpos)
{

    // char rec_buf[10] = {};

    // get value
    if( copy_from_user( rec_buf, buf, count ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    // name = rec_buf;

    pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);

    printk("call write\n");
    return count;
}

static int my_open(struct inode *inode, struct file *fp) {
    printk("call open\n");
    return 0;
}

struct file_operations my_fops = {
    .read = my_read,
    .write = my_write,
    .open = my_open
};

#define MAJOR_NUM 100
#define DEVICE_NAME "mydev"

static int my_init(void) {
    printk("call init\n");
    if(register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0) {
        printk("Can not get major %d\n", MAJOR_NUM);
        return (-EBUSY);
    }
    printk("My device is started and the major is %d\n", MAJOR_NUM);
    return 0;
}

static void my_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk("call exit\n");
}

module_init(my_init);
module_exit(my_exit);