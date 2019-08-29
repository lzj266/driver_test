/******************************** description *********************************

	ioctl_test

 ******************************************************************************
  Filename : demo.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月28日
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#include "chrdev.h"

int major;
const char *name = "demoname";

int demo_open(struct inode *inode, struct file *filep)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}

int demo_release(struct inode *inode, struct file *filep)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}

ssize_t demo_read(struct file *filep, char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}
ssize_t demo_write(struct file *filep, const char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}

/*
	应用层: int ioctl(int fd, unsigned long request, ...);
		@param1: 文件描述符
		@param2: 请求cmd
		@param3: 可变参数   char *p
		@return: 成功返回0,失败返回-1

	驱动层: long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
		@param1: 打开的文件的file结构指针
		@param2: 和应用层ioctl()的第二个参数对应
		@param3: 和应用层ioctl()的第三个参数对应，只能传递四个字节，传地址
		@return: long

	命令格式: linux-4.9.28/Documentation/ioctl/ioctl-decoding.txt 
		bits	meaning
 		31-30  	00 - no parameters: uses _IO macro	不带参数
				10 - read: _IOR						命令从驱动中读取数据，读方向
				01 - write: _IOW					命令把数据写入驱动中，写方向
				11 - read/write: _IOWR				读写方向
		29-16	size of arguments					如果命令带参数，则指定参数所占内存空间的大小
		15-8	ascii character supposedly			每个驱动全局唯一的幻数
				unique to each driver
		7-0		function							命令码
		一个命令由四部分组成，这样做是为了避免命令定义的重复。

	内核中的宏定义:        linux-4.9.28/include/uapi/asm-generic/ioctl.h
		#define _IOC(dir, type, nr, size) \
 			( ((dir)  << _IOC_DIRSHIFT)  | \
 			  ((type) << _IOC_TYPESHIFT) | \
 			  ((nr)   << _IOC_NRSHIFT)   | \
			  ((size) << _IOC_SIZESHIFT) )   	
		dir : 2位   方向: 不带参数_IO;       读_IOR; 写_IOW;   可读可写_IOWR    ;
		size: 14位 指定参数所占内存空间的大小
		type: 8位   类型----> 给定一个ASCII码， a-z  =====>幻数 借助这个幻数让cmd更加唯一 ==== ftok 
		nr  : 8位   命令码的序号: 2^8 = 256; 对设备的控制状态
		
		#define _IOC_NRSHIFT    0
		#define _IOC_TYPESHIFT  (_IOC_NRSHIFT+_IOC_NRBITS)
		#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT+_IOC_TYPEBITS)
		#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT+_IOC_SIZEBITS)
		=====>
		#define _IOC_NRSHIFT	0        == 0位
		#define _IOC_TYPESHIFT	(0+8)    == 8位
		#define _IOC_SIZESHIFT	(8+8)    == 16位
		#define _IOC_DIRSHIFT	(16+13)  == 29位

		通过下面的四个宏可以方便的定义宏_IOC
		#define _IO(type, nr)		    _IOC(_IOC_NONE, (type), (nr), 0)
		#define _IOR(type, nr, size)	_IOC(_IOC_READ, (type), (nr), (_IOC_TYPECHECK(size)))
		#define _IOW(type, nr, size)	_IOC(_IOC_WRITE,(type), (nr), (_IOC_TYPECHECK(size)))
		#define _IOWR(type, nr, size)	_IOC(_IOC_READ|_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
		#define _IOC_NONE  0U
 		#define _IOC_WRITE 1U
 		#define _IOC_READ  2U
		#define _IOC_TYPECHECK(t) (sizeof(t))
*/
long demo_ioctl(struct file *filep, unsigned int cmd, unsigned long args)
{
	led_node_t *led = (led_node_t *)args;
	int lednum;
	
	switch (cmd)
	{
	case LEDOFF:
		lednum = led->which;
		if (lednum == 0)
		{
		    printk("led0 off.\n");
			// 对硬件的控制
		}
		else if (lednum == 1)
		{
		    printk("led1 off.\n");
			// 对硬件的控制
		}
		else if (lednum == 2)
		{
			printk("led2 off.\n");
			// 对硬件的控制
		}
		break;
	case LEDON:
		lednum = led->which;
		if (lednum == 0)
		{
		    printk("led0 on.\n");
		}
		else if (lednum == 1)
		{
		    printk("led1 on.\n");
		}
		else if (lednum == 2)
		{
			printk("led2 on.\n");
		}
		break;
	default:
		printk("cmd error.\n");
	}

	return 0;
}

const struct file_operations fops = {
	.open = demo_open,
	.release = demo_release,
	.read = demo_read,
	.write = demo_write,
	.unlocked_ioctl = demo_ioctl,
};

struct class *cls;
struct device *dev;

static int __init demo_init(void)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	major = register_chrdev(0, name, &fops);
	if (major <= 0)
	{
		printk("register_chrdev failed.\n");
		goto err0;
	}

	cls = class_create(THIS_MODULE, "char_class");
	if (cls == NULL)
	{
		printk("class_create failed.\n");
		goto err1;
	}

	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "chrdev%d", 0);
	if (dev == NULL)
	{
		printk("device_create failed.\n");
		goto err2;
	}
	
	return 0;

err2:
	class_destroy(cls);
err1:
	unregister_chrdev(major, name);	
err0:
	return major;
}

static void __exit demo_exit(void)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, name);	
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");







