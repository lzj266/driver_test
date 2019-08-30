/******************************************************************************

	open_close

 ******************************************************************************
  Filename : demo.c
  Version  : 1
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月27日
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>



int demo_open(struct inode *inode, struct file *filp)
{
	printk("%s -- %d\n", __FUNCTION__, __LINE__);

	return 0;
}

int demo_close (struct inode *inode, struct file *filp)
{
	printk("%s -- %d\n", __FUNCTION__, __LINE__);

	return 0;
}

const char * name = "demoname";
const struct file_operations fops = {
	.open = demo_open,
	.release = demo_close,
};

int major;
struct class *class_p;
struct device *device_p;


static int __init demo_init(void)
{
	printk("%s --%d\n", __FUNCTION__, __LINE__);
/*
	register_chrdev  注册设备号
	@param1: 0表示动态申请
	@param2: /proc/devices的name
	@param3: 文件操作方法集
	@return：动态申请到的设备号，负值和错误码表示失败
*/
	major = register_chrdev(0, name, &fops);
	if (major <= 0)
	{
		printk("register_chrdev failed\n");
		goto err0;
	}
	printk("major is %d\n", major);

/*
	class_create  创建设备类
	@param1: THIS_MODULE
	@param2: 目录名字 /sys/class/xxx
	@return：struct class * 句柄
*/
	class_p = class_create(THIS_MODULE, "char_class");
	if (class_p == NULL)
	{
		printk("class_create failed\n");
		goto err1;
	}

/*
	device_create  在刚才创建的目录下创建设备节点
	@param1: class_create返回的句柄
	@param2: 一般写NULL
	@param3: 设备号
	@param4: 一般写NULL
	@param5: 可变参数  ，一般是设备名 /dev/xxx
	@return：struct device *
*/	
	device_p = device_create(class_p, NULL, MKDEV(major, 0), NULL, "chrdev%d", 0);
	if (device_p == NULL)
	{
		printk("device_create failed\n");
		goto err2;
	}
	
	return 0;
	
err2:
	class_destroy(class_p);
err1:
	unregister_chrdev(major, name);
err0:
	return major;
}

static void __exit demo_exit(void)
{
	printk("%s -- %d\n", __FUNCTION__, __LINE__);

	device_destroy(class_p, MKDEV(major, 0));	//释放设备节点
	class_destroy(class_p);						//删除class_create创建的目录
	unregister_chrdev(major, name);				//注销设备号
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");

