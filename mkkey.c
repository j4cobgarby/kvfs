#include "kvfs.h"
#include "linux/uaccess.h"
#include <linux/slab.h>

struct file_operations mkkey_fops = {
    .open = mkkey_open,
    .read = mkkey_read,
    .write = mkkey_write,
    .release = mkkey_release
};

int mkkey_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t mkkey_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    return -ENXIO;
}

ssize_t mkkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    struct dentry *new_dentry;
    size_t bounded = count > MAX_KEYLEN ? MAX_KEYLEN : count;
    char *buf_krn = kmalloc((bounded+1) * sizeof(char), GFP_KERNEL); /* The extra one char gives space for a null char */
    
    if (copy_from_user(buf_krn, buf, bounded)) return -EFAULT;
    buf_krn[count] = '\0';
    
    new_dentry = mkfile_generic(FILP_SB(filp), FILP_SB(filp)->s_root, buf_krn, &keyfile_fops);

    /* The private data is used to store the value corresponding to a key.
    It starts off at NULL because we don't know how much data the user wants to
    store there, so we can't efficiently allocate memory quite yet.
    When using this value, make sure to check if it's NULL. When writing to a keyfile,
    if it's still NULL, we can then allocate more memory for it. */
    new_dentry->d_inode->i_private = NULL;
    return count;
}

int mkkey_release(struct inode *inode, struct file *filp) {
    return 0;
}