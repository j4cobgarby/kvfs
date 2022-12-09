#include "linux/dcache.h"
#include "linux/kern_levels.h"
#include "linux/mm_types_task.h"
#include "linux/printk.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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