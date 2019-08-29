/******************************************************************************

	register_chardev

 ******************************************************************************
  Filename : demo.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月27日
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>


const char * name = "demoname";
int major;

const struct file_operations fops = {

};

static int __init demo_init(void)
{
	printk("%s -- %d\n", __FUNCTION__, __LINE__);


	major = register_chrdev(0, name, &fops);
	if (major <= 0)
	{
		printk("register_chrdev failed\n");
		goto err0;
	}
	printk("major is %d\n", major);
	
	return 0;
	
err0:
	return major;
}

static void __exit demo_exit(void)
{
	printk("%s --%d\n", __FUNCTION__, __LINE__);

	unregister_chrdev(major, name);
}




module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");



