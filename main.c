#include <linux/module.h>

#include "kvfs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacob Garby <j4cobgarby@gmail.com>");
MODULE_DESCRIPTION("A simple key-value store.");

static int __init kvfs_init(void) 
{
    printk(KERN_INFO "Registering kvfs :)\n");
    return register_filesystem(&kvfs_type);
}

static void __exit kvfs_exit(void)
{
    printk(KERN_INFO "Unregistering kvfs, bye!\n");
    unregister_filesystem(&kvfs_type);
}

module_init(kvfs_init);
module_exit(kvfs_exit);