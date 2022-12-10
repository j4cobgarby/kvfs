#include "kvfs.h"
#include "linux/fs.h"

#include <linux/kstrtox.h>
#include <linux/kernel.h>

#define MAX_LONG_SIZE 32

struct file_operations inckey_fops = {
    .read = inckey_read,
    .write = inckey_write,
    .open = inckey_open,
    .release = inckey_release
};

struct file_operations deckey_fops = {
    .read = deckey_read,
    .write = deckey_write,
    .open = deckey_open,
    .release = deckey_release
};

int add_to_key(struct file *filp, const char *buf, size_t count, loff_t *offset, int n) {
    size_t bounded = count > MAX_KEYLEN ? MAX_KEYLEN : count;
    char *buf_krn = kmalloc((bounded+1) * sizeof(char), GFP_KERNEL); /* The extra one char gives space for a null char */
    struct dentry *to_add;
    struct qstr name_to_add;

    if (copy_from_user(buf_krn, buf, bounded)) return -EFAULT;
    buf_krn[count] = '\0';
    name_to_add.len = strlen(buf_krn);
    name_to_add.name = buf_krn;

    if (!(to_add = d_hash_and_lookup(filp->f_inode->i_sb->s_root, &name_to_add))) {
        return -ENXIO;
    } else {
        struct kv_value **value = (struct kv_value**)&to_add->d_inode->i_private;
        char *str = (*value)->data;
        size_t str_len = (*value)->len;
        long value_int;

        if (!value) return count;
        if (kstrtol(str, 0, &value_int) != 0) return count;

        value_int += n;
        if (str_len <= MAX_LONG_SIZE)
            if (!((*value)->data = krealloc((*value)->data, 16, GFP_KERNEL)))
                return -ENOMEM;

        printk("New value is %d\n", value_int);

        snprintf((*value)->data, MAX_LONG_SIZE, "%ld", value_int);
        (*value)->len = MAX_LONG_SIZE;
    }

    return count;
}

int inckey_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t inckey_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    return -ENXIO;
}

ssize_t inckey_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    return add_to_key(filp, buf, count, offset, 1);
}

int inckey_release(struct inode *inode, struct file *filp) {
    return 0;
}


int deckey_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t deckey_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    return -ENXIO;
}

ssize_t deckey_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    return add_to_key(filp, buf, count, offset, -1);
}

int deckey_release(struct inode *inode, struct file *filp) {
    return 0;
}