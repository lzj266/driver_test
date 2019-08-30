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

/*
	函数原型：
	static inline int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops)
	{
		return __register_chrdev(major, 0, 256, name, fops);
	}

	int __register_chrdev(unsigned int major, unsigned int baseminor, unsigned int count, const char *name, const struct file_operations *fops)
	{
		struct char_device_struct *cd;
		struct cdev *cdev;	//定义
		int err = -ENOMEM;

		cd = __register_chrdev_region(major, baseminor, count, name);	//申请一个字符设备号
		if (IS_ERR(cd))
			return PTR_ERR(cd);

		cdev = cdev_alloc();	//申请空间
		if (!cdev)
			goto out2;

		cdev->owner = fops->owner;
		cdev->ops = fops;
		kobject_set_name(&cdev->kobj, "%s", name);

		err = cdev_add(cdev, MKDEV(cd->major, baseminor), count);	//添加到内核当中
		if (err)
			goto out;

		cd->cdev = cdev;

		return major ? 0 : cd->major;
	out:
		kobject_put(&cdev->kobj);
	out2:
		kfree(__unregister_chrdev_region(cd->major, baseminor, count));
		return err;
}
*/
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



