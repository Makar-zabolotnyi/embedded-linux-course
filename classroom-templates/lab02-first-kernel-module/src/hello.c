#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Makar Zabolotnyi");
MODULE_DESCRIPTION("Lab 02 hello module for BeagleBone Black");
MODULE_VERSION("1.0");

static char *name = "Student";
module_param(name, charp, 0444);
MODULE_PARM_DESC(name, "Name printed when the module loads");

static unsigned int count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print the greeting (1-10)");

static int __init hello_init(void)
{
    unsigned int i;

    if (!name || name[0] == '\0')
        return -EINVAL;

    if (count < 1 || count > 10)
        return -EINVAL;

    for (i = 0; i < count; i++)
        pr_info("hello_module: Hello, %s, from kernel space on BBB!\n", name);

    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("hello_module: Goodbye from kernel space on BBB!\n");
}

module_init(hello_init);
module_exit(hello_exit);