/******************************** description *********************************

	ioctl_led_test

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
#include <linux/io.h>

#include "chrdev.h"

int major;
const char *name = "demoname";

struct class *cls;
struct device *dev;

#define GPX1CON  0x11000C20
#define GPX1DAT  0x11000C24
void __iomem *gpx1con_vir;
void __iomem *gpx1dat_vir; 


int demo_open(struct inode *inode, struct file *filp)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}

int demo_release(struct inode *inode, struct file *filp)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;
}

ssize_t demo_read(struct file *filp, char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	//copy_to_user(void __user * to, const void * from, size_t n)

	return 0;
}

ssize_t demo_write(struct file *filp, const char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	//copy_from_user(void * to, const void __user * from, unsigned long n)

	return 0;
}

long demo_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{	
	led_node_t *led = (led_node_t *)args;
	int led_num;

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	led_num = led->which;
	switch(cmd)
	{
	case LEDON:
		if (led_num == 0)
		{
			printk("led0 on.\n");
			writel(readl(gpx1dat_vir) | (0X1 << 0), gpx1dat_vir);	// 置1，高电平
		}
		else if (led_num == 1)
		{
			printk("led1 on.\n");
			// 硬件操作
			// writel readl
		}
		else if (led_num == 2)
		{
			printk("led2 on.\n");
			// 硬件操作
			// writel readl
		}
		break;
	case LEDOFF:
		if (led_num == 0)
		{
			printk("led0 off.\n");
			writel(readl(gpx1dat_vir) & (0X1 << 0), gpx1dat_vir);	// 清0，低电平
		}
		else if (led_num == 1)
		{
			printk("led1 off.\n");
			// 硬件操作
			// writel readl
		}
		else if (led_num == 2)
		{
			printk("led2 off.\n");
			// 硬件操作
			// writel readl
		}
		break;
	default:
		printk("cmd id error.\n");
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

/*
	led_init函数功能: 配置方向寄存器为输出模式
	ioremap: 将物理地址转化为虚拟地址
		原型     : void __iomem *ioremap(phys_addr_t paddr, unsigned long size)
		@param1: 要映射的起始的IO地址
		@param2: 要映射的空间的大小
		@return: 返回映射后的内核虚拟地址(3G-4G). 接着便可以通过读写该返回的内核虚拟地址去访问之这段I/O内存资源
	readl: 从内存映射的I/O空间上读数据,readl从I/O读取32位数据(4字节)
		原型     ：static inline u32 readl(const volatile void __iomem *addr)
		@param1: 映射后的内核虚拟地址,ioremap的返回值
		@return: 读到的虚拟地址中的值
	writel: 往内存映射的 I/O 空间上写数据
		原型     : static inline void writel(u32 b, volatile void __iomem *addr)
		@param1: 要写入的数据
		@param2: 映射后的内核虚拟地址,ioremap的返回值
		@return: 无返回值
	iounmap: 用于取消ioremap()所做的映射
		原型     : static inline void iounmap(volatile void __iomem *addr)
		@param1: 映射后的内核虚拟地址,ioremap的返回值
		@return: 无返回值
*/
void gpio_iounmap(void)
{
	iounmap(gpx1con_vir);
}

void gpio_ioremap(void)
{
	gpx1con_vir = (unsigned int *)ioremap(GPX1CON, 4);
	gpx1dat_vir = gpx1con_vir + 4;
}

void led_init(void)
{
	gpio_ioremap();

	writel(( readl(gpx1con_vir) & (~(0xF << 0)) ) | (0x1 << 0),gpx1con_vir);
}

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
	if (dev ==NULL)
	{
		printk("device_create failed.\n");
		goto err2;
	}

	led_init();	// 设置为输出模式
	
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

	gpio_iounmap();
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, name);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");

