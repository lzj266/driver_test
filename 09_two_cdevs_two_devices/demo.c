
/******************************** description *********************************

	本实例以两个cdev控制两个LED设备为例

	  前面一直使用register_chrdev来注册字符设备，方法简单但是无法指定次设备号，
	并且会在内核中连续注册了256（分析内核代码中可知），也就是所以的此设备号都会
	被占用，而在大多数情况下都不会用到这么多次设备号，所以会造成极大的资源浪费。
	  本节采用另一种方法来注册字符设备，该方法可以设置主次设备号及个数但过程比较复杂。

	1. 定义字符设备结构体
		struct cdev cdev; 或者   struct cdev *cdev;
		
	2.1 实例化一个字符设备体，为cdev分配内存
		struct cdev *cdev_alloc(void);
		
	2.2 填充cdev设备体，最主要是将file_operations填充进去
		void cdev_init(struct cdev *cdev, const struct file_operations *fops);
		
		***** 注意：cdev_alloc函数针对于需要空间申请的操作，而cdev_init针对于不需要空间申请的操作；
		***** 因此如果你定义的是一个指针，那么只需要使用cdev_alloc函数并在其后做一个ops的赋值操作就可以了；
			  struct cdev *cdev;
			  cdev = cdev_alloc();  
			  cdev->owner = THIS_MODULE;  
			  cdev->ops = &fops;
		***** 如果你定义的是一个结构体而非指针，那么只需要使用cdev_init函数就可以了。
			  struct cdev cdev;
			  cdev_init(struct cdev * cdev, const struct file_operations * fops);
			  
	3. 向内核申请设备号
		int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
			@功能    : 动态申请设备号
			@param1: 分配得到的第一个设备号
			@param2: 动态分配的设备号的起始次设备号
			@param3: 设备数量
			@param4: 标定主设备号的名称
			@return: 0表示成功，负数表示失败
		int register_chrdev_region(dev_t from, unsigned count, const char *name);
			@功能    : 静态注册一个或多个连续的字符设备号
			@param1: 指定起始的设备号
			@param2: 设备数量
			@param3: 标定主设备号的名称
			@return: 0表示成功，负数表示失败
		注销设备号:  	void unregister_chrdev_region(dev_t from, unsigned count);
		
	4. 将设备体与设备号绑定，并向内核添加一个字符设备
		int cdev_add(struct cdev *cdev, dev_t dev, unsigned count);
			@param3: 指定了被添加的cdev可以管理多少个设备
			@return: 0表示成功
		删除cdev对象: void cdev_del(struct cdev *)	

 ******************************************************************************
  Filename : demo.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月29日
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/io.h>

#define NUM_OF_DEVICES (2)
#define LED_MAJOR (255)
#define LED_MINOR 0
#define LED_DEV_NAME "led_device"

struct led_device {
    dev_t dev_num;
    char data[128];
    char name[16];
	struct cdev cdev;	//字符设备结构体
};
static struct led_device led_dev[NUM_OF_DEVICES];	//定义两个设备

struct class *cls;

ssize_t led_read(struct file *, char __user *, size_t, loff_t *);
ssize_t led_write(struct file *, const char __user *, size_t, loff_t *);
int led_open(struct inode *, struct file *);
int led_release(struct inode *, struct file *);


const struct file_operations led_ops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.read = led_read,
	.write = led_write,
};




static int __init demo_init(void)
{
	int i, ret;
	dev_t dev;

	printk("%s -- %d.\n", __FUNCTION__, __LINE_);

	dev = MKDEV(LED_MAJOR, LED_MINOR);
	ret = register_chrdev_region(dev, 2, LED_DEV_NAME);
	if (ret < 0)
	{
		printk("register_chrdev_region failed.\n");
		goto reg_err;
	}
	
	for (i=0; i<NUM_OF_DEVICES; i++)
	{
		cdev_init(&(led_dev[i].cdev), &led_ops);
		led_dev[i].cdev.owner = THIS_MODULE;

		ret = cdev_add(&(led_dev[i].cdev), dev+i, 1)
		if (ret)
		{
			printk("cdev_add failed.\n");
			goto add_err;
		}
		
	}

	return 0;

add_err:
	for (--i; i>=0; --i)
		cdev_del(&(led_dev[i].cdev));
	unregister_chrdev_region(dev, 2);
reg_err:
	return ret;
}

static void __exit demo_exit(void)
{
	printk("%s --%d.\n", _FUNCTION__, __LINE_);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");


