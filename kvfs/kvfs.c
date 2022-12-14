#include "kvfs.h"
#include "linux/dcache.h"
#include "linux/fs.h"
#include "linux/gfp_types.h"
#include "linux/mutex.h"
#include "linux/printk.h"
#include "linux/uidgid.h"
#include "linux/kstrtox.h"

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

struct mutex mut_delete;
struct mutex mut_create;

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

struct dentry *mkfile_generic(struct super_block *sb, struct dentry *dir, 
        const char *name, const struct file_operations *fops, int mode) {
    struct dentry *dentry;
    struct inode *inode;

    dentry = d_alloc_name(dir, name);
    if (!dentry) return 0;

    inode = mkinode(sb, mode, fops);
    if (!inode) {
        dput(dentry);
        return 0;
    }
    
    d_add(dentry, inode);
    return dentry;
}

struct dentry *kvfs_mount(struct file_system_type *fst, int flags, const char *dev, void *data) {
    struct dentry *ret;
    struct dentry *key;

    int gid;
    kgid_t gidt;

    mutex_init(&mut_delete);
    mutex_init(&mut_create);

    if (!data || kstrtoint((char*)data, 10, &gid) != 0) gid = 0;
    gidt = KGIDT_INIT(gid);

    printk(KERN_INFO "data = %s\n", (char*)data);
    printk(KERN_INFO "File GID = %d\n", gid);

    ret = mount_nodev(fst, flags, data, kvfs_fill_super);

    // Make control files in the root directory.
    // Setting their gid. mkkey's gid is passed on to any keys it creates

    key = mkfile_generic(ret->d_sb, ret->d_sb->s_root, "_mk", &mkkey_fops, S_IFREG | FILE_MODE);
    key->d_inode->i_gid = gidt;

    key = mkfile_generic(ret->d_sb, ret->d_sb->s_root, "_del", &delkey_fops, S_IFREG | FILE_MODE);
    key->d_inode->i_gid = gidt;

    key = mkfile_generic(ret->d_sb, ret->d_sb->s_root, "_inc", &inckey_fops, S_IFREG | FILE_MODE);
    key->d_inode->i_gid = gidt;

    key = mkfile_generic(ret->d_sb, ret->d_sb->s_root, "_dec", &inckey_fops, S_IFREG | FILE_MODE);
    key->d_inode->i_gid = gidt;

    return ret;
}

int kvfs_fill_super(struct super_block *sb, void *data, int silent) {
    struct inode *root;
    struct dentry *root_dentry;

    sb->s_blocksize = VMACACHE_SIZE;
    sb->s_blocksize_bits = VMACACHE_SIZE;
    sb->s_magic = KVFS_MAGIC;
    sb->s_op = &kvfs_sops;

    root = mkinode(sb, S_IFDIR | 0775, &simple_dir_operations);
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

    return 0;
}