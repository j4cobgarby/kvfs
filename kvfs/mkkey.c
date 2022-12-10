#include "kvfs.h"
#include "linux/dcache.h"
#include "linux/mutex.h"
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
    
    struct qstr desired_name;

    if (copy_from_user(buf_krn, buf, bounded)) return -EFAULT;
    buf_krn[count] = '\0';
    desired_name.len = strlen(buf_krn);
    desired_name.name = buf_krn;

    mutex_lock(&mut_create);
    if (d_hash_and_lookup(filp->f_inode->i_sb->s_root, &desired_name)) {
        // Key by that name already exists
        mutex_unlock(&mut_create);
        return -ENXIO;
    }
    mutex_unlock(&mut_create);
    
    printk(KERN_INFO "MKKEY making dentry\n");
    new_dentry = mkfile_generic(FILP_SB(filp), FILP_SB(filp)->s_root, buf_krn, &keyfile_fops, S_IFREG | 0666);

    printk(KERN_INFO "Made dentry\n");
    { // Initialise values in the key's private data
        struct kv_value **value = (struct kv_value**)&new_dentry->d_inode->i_private;
        
        *value = kmalloc(sizeof(struct kv_value), GFP_KERNEL);

        (*value)->len = 0;
        mutex_init(&(*value)->mut);
        (*value)->data = NULL;
    }
    printk(KERN_INFO "MKKEY end\n");
    
    return count;
}

int mkkey_release(struct inode *inode, struct file *filp) {
    return 0;
}