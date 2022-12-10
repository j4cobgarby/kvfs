#ifndef KVFS_H
#define KVFS_H

#include <linux/fs.h>
#include <linux/mutex.h>

#define KVFS_MAGIC 0x14c0b4b7
#define MAX_KEYLEN 64
#define MAX_VALUELEN 512

#define FILP_SB(filp) (filp->f_inode->i_sb)

struct kv_value {
    char *data;
    size_t len;
    struct mutex mut;
};

extern struct mutex mut_delete;
extern struct mutex mut_create;

extern struct file_system_type kvfs_type;
extern struct super_operations kvfs_sops;

extern struct file_operations keyfile_fops;
extern struct file_operations mkkey_fops;
extern struct file_operations delkey_fops;
extern struct file_operations inckey_fops;
extern struct file_operations deckey_fops;

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
 * @param sb Superblock of the particular mount.
 * @param dir Directory that the file should be in.
 * @param name Name for the file.
 * @return The dentry of the new file.
 * 
 * This function is used both for the keyfile files, as well as the control
 * files `mkkey` and `delkey`, by specifying different fops.
 */
struct dentry *mkfile_generic(struct super_block *sb, struct dentry *dir, 
    const char *name, const struct file_operations *fops, int mode);

/**
 * @brief Opens a keyfile.
 * 
 * @param inode The inode associated with the file.
 * @param filp The file that's being opened.
 * @return 0 if the file was opened successfully, otherwise an error code.
 */
int keyfile_open(struct inode *inode, struct file *filp);

/**
 * @brief Called when a user wants to get the value of a key.
 * 
 * @param filp The file that's being read.
 * @param buf Where to store the value.
 * @param count How many bytes of the value to read.
 * @param offset Where to begin reading in the value.
 * @return ssize_t How many bytes were read, or <0 if an error occurred.
 * 
 * The value of any given key is stored in the inode's i_private field, as a char*.
 */
ssize_t keyfile_read(struct file *filp, char *buf, size_t count, loff_t *offset);

/**
 * @brief Called when the user wants to set the value of a given key.
 * 
 * @param filp The file being written to.
 * @param buf Data to write into the key's value.
 * @param count How many bytes to write.
 * @param offset Unused - we'll read from the start of buf into value.
 * @return ssize_t How many bytes were successfully written, or <0 if an error occurred.
 * 
 * The value of any given key is stored in the inode's i_private field, as a char*.
 */
ssize_t keyfile_write(struct file *filp, const char *buf, size_t count, loff_t *offset);

/**
 * @brief Called when the file's closed.
 * 
 * @param inode Its inode.
 * @param filp The file.
 * @return 0 on success. It should never fail.
 */
int keyfile_release(struct inode *inode, struct file *filp);

/**
 * @brief Opens the mkkey file.
 * 
 * @param inode The inode.
 * @param filp The file.
 * @return 0 on success. It should never fail.
 * 
 * The mkkey file is used to create new keys in the system.
 */
int mkkey_open(struct inode *inode, struct file *filp);

/**
 * @brief Cannot read the mkkey file, so this always fails.
 * 
 * @return -ENXIO
 */
ssize_t mkkey_read(struct file *filp, char *buf, size_t count, loff_t *offset);

/**
 * @brief Create a new key with the data being written as its name.
 * 
 * @param filp The file.
 * @param buf Data for the key name.
 * @param count The length of the name.
 * @param offset Unused, takes name from the beginning of `buf`.
 * @return The amount of bytes taken. This should be count.
 *
 * The brand new key's value is not explicitly initialised. To begin with, it
 * will be NULL, and memory will be allocated as and when needed.
 */
ssize_t mkkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);

/**
 * @brief TODO: Maybe the key should be finalised on _this_ call, since that way a user
 *              could in theory construct the key name in multiple writes.
 * 
 * @param inode The inode
 * @param filp The file
 * @return 0 on success, which should always be the case.
 */
int mkkey_release(struct inode *inode, struct file *filp);

/**
 * @brief Open the file to prepare to delete a key.
 * 
 * @param inode The inode
 * @param filp The file
 * @return 0 on success, which should always be the case.
 */
int delkey_open(struct inode *inode, struct file *filp);

/**
 * @brief Invalid operation, will fail.
 * 
 * @return -ENXIO
 */
ssize_t delkey_read(struct file *filp, char *buf, size_t count, loff_t *offset);

/**
 * @brief Delete a key of the given name.
 * 
 * @param filp This file.
 * @param buf The name of the key.
 * @param count Length of the name.
 * @param offset Unused.
 * @return The length of the name, otherwise a negative value for an error. -ENXIO if key not found.
 */
ssize_t delkey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);

/**
 * @brief TODO: Maybe this should be the function where the key is actually deleted.
 * 
 * @param inode The inode.
 * @param filp This file.
 * @return 0 on success, which should always be the case.
 */
int delkey_release(struct inode *inode, struct file *filp);

int add_to_key(struct file *filp, const char *buf, size_t count, loff_t *offset, int n);

int inckey_open(struct inode *inode, struct file *filp);
ssize_t inckey_read(struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t inckey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
int inckey_release(struct inode *inode, struct file *filp);

int deckey_open(struct inode *inode, struct file *filp);
ssize_t deckey_read(struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t deckey_write(struct file *filp, const char *buf, size_t count, loff_t *offset);
int deckey_release(struct inode *inode, struct file *filp);

#endif