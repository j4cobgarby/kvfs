#include "kvfs.h"
#include "linux/gfp.h"
#include "linux/slab.h"

#define REALLOC_FACTOR 2

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
    char **value = (char**)&(filp->f_inode->i_private);
    size_t val_len = strlen(*value);    

    if (*offset >= val_len) 
        return 0;
    if (*offset + count > val_len) 
        count = val_len - *offset;
    if (copy_to_user(buf, *value + *offset, count)) 
        return -EFAULT;

    *offset += count;

    return count;
}

ssize_t keyfile_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    char **value = (char**)&(filp->f_inode->i_private);
    
    if (!(*value = krealloc(*value, 1 + REALLOC_FACTOR * count * sizeof(char), GFP_KERNEL))) 
        return -ENOMEM;
    if (copy_from_user(*value, buf, count)) 
        return -EFAULT;

    (*value)[count] = '\0';

    return count;
}

int keyfile_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Closing.\n");
    return 0;
}