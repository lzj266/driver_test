#include <linux/module.h>

static int __init demo_init(void)
{
	printk("%s -- %s -- %d\n", __FILE__, __FUNCTION__, __LINE__);

	return 0;
}

static void __exit demo_exit(void)
{
	printk("%s -- %s -- %d\n", __FILE__, __FUNCTION__, __LINE__);
}


/* 模块的入口函数 */
module_init(demo_init);

/* 模块的出口函数 */
module_exit(demo_exit);

/* GPL协议 */
MODULE_LICENSE("GPL");


