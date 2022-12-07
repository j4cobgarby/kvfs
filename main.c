#include "linux/dcache.h"
#include "linux/kern_levels.h"
#include "linux/mm_types_task.h"
#include "linux/printk.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacob Garby <j4cobgarby@gmail.com>");
MODULE_DESCRIPTION("A simple key-value store.");

#define KVFS_MAGIC 0x14c0b4b7

static struct dentry *kvfs_mount(struct file_system_type *fst, int flags, const char *dev, void *data);
static int kvfs_fill_super(struct super_block *sb, void *data, int silent);
static struct dentry *mkfile(struct super_block *sb, struct dentry *dir, const char *name);

static int kvfs_open(struct inode *inode, struct file *filp);
static ssize_t kvfs_read(struct file *filp, char *buf, size_t count, loff_t *offset);
static ssize_t kvfs_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
static int kvfs_release(struct inode *, struct file *);

static struct file_system_type kvfs_type = {
    .owner = THIS_MODULE,
    .name = "kvfs",
    .mount = kvfs_mount,
    .kill_sb = kill_litter_super,
};

static struct super_operations kvfs_sops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static struct file_operations kvfs_fops = {
    .open = kvfs_open,
    .read = kvfs_read,
    .write = kvfs_write,
    .release = kvfs_release,
};

static struct inode *mkinode(struct super_block *sb, int mode, const struct file_operations *fops) {
    struct inode *inode;
    if (!(inode = new_inode(sb))) {
        printk(KERN_CRIT "kvfs failed to make new inode.\n");
        return NULL;
    }
    inode->i_mode = mode;
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
    inode->i_fop = fops;
    inode->i_ino = get_next_ino();
    return inode;
}

static struct dentry *mkfile(struct super_block *sb, struct dentry *dir, const char *name) {
    struct dentry *dentry;
    struct inode *inode;


    dentry = d_alloc_name(dir, name);
    if (!dentry) return 0;

    inode = mkinode(sb, S_IFREG | 0644, &kvfs_fops);
    if (!inode) {
        dput(dentry);
        return 0;
    }
    
    d_add(dentry, inode);
    return dentry;
}

static struct dentry *kvfs_mount(struct file_system_type *fst, int flags, const char *dev, void *data) {
    return mount_nodev(fst, flags, data, kvfs_fill_super);
}

static int kvfs_fill_super(struct super_block *sb, void *data, int silent) {
    struct inode *root;
    struct dentry *root_dentry;

    sb->s_blocksize = VMACACHE_SIZE;
    sb->s_blocksize_bits = VMACACHE_SIZE;
    sb->s_magic = KVFS_MAGIC;
    sb->s_op = &kvfs_sops;

    root = mkinode(sb, S_IFDIR | 0755, &simple_dir_operations);
    inode_init_owner(sb->s_user_ns, root, NULL, S_IFDIR | 0755);
    if (!root) {
        return -ENOMEM;
    }
    root->i_op = &simple_dir_inode_operations;

    set_nlink(root, 2);
    root_dentry = d_make_root(root);
    if (!root_dentry) {
        iput(root);
        return -ENOMEM;
    }

    sb->s_root = root_dentry;
    mkfile(sb, root_dentry, "file1");

    return 0;
}

static int kvfs_open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Opened a file.\n");
    return 0;
}

static ssize_t kvfs_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    char tmp[13] = "Hello world\n";
    if (*offset > 11) return 0;
    if (count > 11 - *offset) count = 11 - *offset;
    if (copy_to_user(buf, tmp + *offset, count)) return -EFAULT;

    *offset += count;

    printk(KERN_INFO "Reading.\n");
    return count;
}

static ssize_t kvfs_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    printk(KERN_INFO "Writing.\n");
    return count;
}

static int kvfs_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Closing.\n");
    return 0;
}

static int __init kvfs_init(void) 
{
    printk("kvfs initialising\n");
    return register_filesystem(&kvfs_type);
}

static void __exit kvfs_exit(void)
{
    printk("bye :)\n");
    unregister_filesystem(&kvfs_type);
}

module_init(kvfs_init);
module_exit(kvfs_exit);