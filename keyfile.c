#include "kvfs.h"

struct file_operations keyfile_fops = {
    .open = keyfile_open,
    .read = keyfile_read,
    .write = keyfile_write,
    .release = keyfile_release,
};

int keyfile_open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Opened a file.\n");
    return 0;
}

ssize_t keyfile_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    char tmp[13] = "Hello world\n";
    if (*offset > 11) return 0;
    if (count > 11 - *offset) count = 11 - *offset;
    if (copy_to_user(buf, tmp + *offset, count)) return -EFAULT;

    *offset += count;

    printk(KERN_INFO "Reading.\n");

    return count;
}

ssize_t keyfile_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    printk(KERN_INFO "Writing.\n");
    return count;
}

int keyfile_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Closing.\n");
    return 0;
}