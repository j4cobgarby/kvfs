#include "kvfs.h"

struct file_system_type kvfs_type = {
    .owner = THIS_MODULE,
    .name = "kvfs",
    .mount = kvfs_mount,
    .kill_sb = kill_litter_super,
};

struct super_operations kvfs_sops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

struct file_operations kvfs_fops = {
    .open = kvfs_open,
    .read = kvfs_read,
    .write = kvfs_write,
    .release = kvfs_release,
};

struct inode *mkinode(struct super_block *sb, int mode, const struct file_operations *fops) {
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

struct dentry *mkfile(struct super_block *sb, struct dentry *dir, const char *name) {
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

struct dentry *kvfs_mount(struct file_system_type *fst, int flags, const char *dev, void *data) {
    printk("%s\n", dev);
    return mount_nodev(fst, flags, data, kvfs_fill_super);
}

int kvfs_fill_super(struct super_block *sb, void *data, int silent) {
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

int kvfs_open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Opened a file.\n");
    return 0;
}

ssize_t kvfs_read(struct file *filp, char *buf, size_t count, loff_t *offset) {
    char tmp[13] = "Hello world\n";
    if (*offset > 11) return 0;
    if (count > 11 - *offset) count = 11 - *offset;
    if (copy_to_user(buf, tmp + *offset, count)) return -EFAULT;

    *offset += count;

    printk(KERN_INFO "Reading.\n");
    return count;
}

ssize_t kvfs_write(struct file *filp, const char *buf, size_t count, loff_t *offset) {
    printk(KERN_INFO "Writing.\n");
    return count;
}

int kvfs_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Closing.\n");
    return 0;
}