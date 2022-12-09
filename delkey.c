#include "asm/string_64.h"
#include "kvfs.h"
#include "linux/dcache.h"
#include "linux/printk.h"
#include "linux/stringhash.h"
#include <linux/slab.h>

struct file_operations delkey_fops = {
    .open = delkey_open,
    .read = delkey_read,
    .write = delkey_write,
    .release = delkey_release
};

int delkey_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t delkey_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    return -ENXIO;
}

ssize_t delkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    size_t bounded = count > MAX_KEYLEN ? MAX_KEYLEN : count;
    char *buf_krn = kmalloc((bounded+1) * sizeof(char), GFP_KERNEL); /* The extra one char gives space for a null char */
    struct dentry *to_delete;
    struct qstr name_to_del;

    if (copy_from_user(buf_krn, buf, bounded)) return -EFAULT;
    buf_krn[count] = '\0';
    printk("name to delete: `%s`\n", buf_krn);
    name_to_del.len = strlen(buf_krn);
    name_to_del.name = buf_krn;

    if (!(to_delete = d_hash_and_lookup(filp->f_inode->i_sb->s_root, &name_to_del))) {
        printk("Couldn't find entry to delete\n");
        return -ENXIO;
    } else {
        printk("Deleting file %s\n", to_delete->d_name.name);
        d_delete(to_delete);
    }
    
    return count;
}

int delkey_release(struct inode *inode, struct file *filp) {
    return 0;
}