#include "kvfs.h"
#include "linux/gfp.h"
#include "linux/gfp_types.h"
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
    struct kv_value **value = (struct kv_value**)&filp->f_inode->i_private;
    size_t val_len;

    if (!*value || !(*value)->data) return 0;

    mutex_lock(&(*value)->mut);
    val_len = strlen((*value)->data);

    if (*offset >= val_len) 
        return 0;
    if (*offset + count > val_len) 
        count = val_len - *offset;
    if (copy_to_user(buf, (*value)->data + *offset, count)) 
        return -EFAULT;
    mutex_unlock(&(*value)->mut);

    *offset += count;

    return count;
}

ssize_t keyfile_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    struct kv_value **value = (struct kv_value**)&(filp->f_inode->i_private);
    size_t new_size = 1 + REALLOC_FACTOR * count * sizeof(char);

    if (!*value) {
        *value = kmalloc(sizeof(struct kv_value), GFP_KERNEL);
        (*value)->len = 0;
    }

    mutex_lock(&(*value)->mut);
    if (count > (*value)->len)
        if (!((*value)->data = krealloc((*value)->data, new_size, GFP_KERNEL))) 
            return -ENOMEM;
    if (copy_from_user((*value)->data, buf, count))
        return -EFAULT;

    (*value)->len = new_size;
    (*value)->data[count] = '\0';
    mutex_unlock(&(*value)->mut);

    return count;
}

int keyfile_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Closing.\n");
    return 0;
}
