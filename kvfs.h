#ifndef KVFS_H
#define KVFS_H

#include <linux/fs.h>

#define KVFS_MAGIC 0x14c0b4b7
#define MAX_KEYLEN 64
#define MAX_VALUELEN 512

#define FILP_SB(filp) (filp->f_inode->i_sb)

/**
 * @brief Creates a kvfs mount.
 * 
 * @param fst Struct describing the kvfs filesystem.
 * @param flags Mount flags.
 * @param dev Name of device for the mount. This isn't used, since this device has no on-disk representation.
 * @param data Mount options provided by the user
 * @return The root dir of the mount.
 */
struct dentry *kvfs_mount(struct file_system_type *fst, int flags, const char *dev, void *data);

/**
 * @brief Sets up the superblock of a mount, and creates the root dir.
 * 
 * This is called by the `mount_nodev` call in `kvfs_mount`. That will make a basic superblock, and
 * then this function fills it in.
 *
 * @param sb The superblock to set up. This should already be initialised by mount_nodev
 * @param data The mount options provided by the user
 * @param silent If true, be silent on any errors.
 * @return 0 if successful, otherwise an error value.
 */
int kvfs_fill_super(struct super_block *sb, void *data, int silent);

/**
 * @brief Makes an inode and a dentry, and connects the two together.
 * 
 * This function is used both for the keyfile files, as well as the control
 * files `mkkey` and `delkey`, by specifying different fops.
 *
 * @param sb Superblock of the particular mount.
 * @param dir Directory that the file should be in.
 * @param name Name for the file.
 * @return The dentry of the new file.
 */
struct dentry *mkfile_generic(struct super_block *sb, struct dentry *dir, 
const char *name, const struct file_operations *fops, int mode);

struct dentry *mksubdir(struct super_block *sb, struct dentry *dir, const char *name, int mode);

int keyfile_open(struct inode *inode, struct file *filp);
ssize_t keyfile_read(struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t keyfile_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
int keyfile_release(struct inode *inode, struct file *filp);

int mkkey_open(struct inode *inode, struct file *filp);
ssize_t mkkey_read(struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t mkkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
int mkkey_release(struct inode *inode, struct file *filp);

int delkey_open(struct inode *inode, struct file *filp);
ssize_t delkey_read(struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t delkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
int delkey_release(struct inode *inode, struct file *filp);

extern struct file_system_type kvfs_type;
extern struct super_operations kvfs_sops;
extern struct file_operations keyfile_fops;
extern struct file_operations mkkey_fops;
extern struct file_operations delkey_fops;

#endif