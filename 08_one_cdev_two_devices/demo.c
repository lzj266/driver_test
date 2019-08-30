/******************************** description *********************************

	一个cdev实现对多个设备的支持

	  如果一个设备有多个个体（比如系统上有两个串口），那么我们就应该写一个
	驱动来支持这几个设备，而不是每一个设备都写一个驱动。
	  我们可以向内核注册多个设备号，在添加cdev对象时指名该cdev对象管理了多
	少个设备；或者添加多个cdev对象，每个cdev对象管理一个设备。
	  比较麻烦的在于读写操作，因为设备是多个那么设备对应的资源也是多个（比
	如虚拟串口驱动中的FIFO），所以在读写操作时应该区分对哪个设备进行操作。
	观察读写函数，没有发现能够区别设备的形参，观察open接口，里面有一个inode
	形参，包含了对应设备的设备号和cdev对象的地址。因此可以在open函数中取出
	这些信息，并存放在file结构体的某个成员中，再在读写的接口函数中获取该file
	结构的成员，从而区分对哪个设备进行操作。

	本实例以一个cdev控制两个LED设备为例

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

#include "led.h"

#define LED_NUM (2)

int major;
const char * name = "led";

struct class *cls;
struct device *dev;

#define GPX1CON  0x11000C20
#define GPX1DAT  0x11000C24
void __iomem *gpx1con_vir;
void __iomem *gpx1dat_vir;

static int led_open(struct inode *inode, struct file *filp)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	int minor = MINOR(inode->i_rdev);
	filp->private_data = (void *)&minor;

	return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	return 0;

}

static long led_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
#if 0
	// 不在open里面获取次设备号，在此处获取此设备号
	struct inode *inode = filp->f_path.dentry->d_inode;	//获取inode对象   
	int minor = MINOR(inode->i_rdev);	//获取次设备号
#endif

	int minor = *(int *)(filp->private_data);
	
	switch(cmd)
	{
	case LEDON:
		if (minor == 0)
		{
			printk("led0 on.\n");
			// 硬件操作
		}
		else if (minor == 1)
		{
			printk("led1 on.\n");
			writel(readl(gpx1dat_vir) | (0X1 << 0), gpx1dat_vir);	// 置1，高电平
		}
		break;
	case LEDOFF:
		if (minor == 0)
		{
			printk("led0 off.\n");
			// 硬件操作
		}
		else if (minor == 1)
		{
			printk("led1 off.\n");
			writel(readl(gpx1dat_vir) | (0X1 << 0), gpx1dat_vir);	// 清0，低电平
		}
		break;
	default:
		printk("cmd error.\n");
	}

	return 0;
}

const struct file_operations led_ops = {
	.open = led_open,
	.release = led_release,
	.unlocked_ioctl = led_ioctl,
};

void gpio_iounmap(void)
{
	iounmap(gpx1con_vir);
}

void gpio_ioremap(void)
{
	gpx1con_vir = (unsigned int *)ioremap(GPX1CON, 4);
	gpx1dat_vir = gpx1con_vir + 4;
}

static void led_init(void)
{
	gpio_ioremap();

	writel(( readl(gpx1con_vir) & (~(0xF << 0)) ) | (0x1 << 0),gpx1con_vir);
}

static int __init demo_init(void)
{
	int i;
	
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	major = register_chrdev(0, name, &led_ops);
	if (major <= 0)
	{
		printk("register_chrdev failed.\n");
		goto err0;
	}

	cls = class_create(THIS_MODULE, "serial_port_clss");
	if (cls == NULL)
	{
		printk("class_create failed.\n");
		goto err1;
	}

	for (i=0; i<LED_NUM; i++)	//创建两个设备
	{
		dev = device_create(cls, NULL, MKDEV(major, i), NULL, "led_device%d", i);
		if (dev == NULL)
		{
			printk("device_create failed.\n");
			goto err2;
		}
	}

	led_init();

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
	int i;

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	gpio_iounmap();

	for (i=0; i<LED_NUM; i++)
	{
		device_destroy(cls, MKDEV(major, i));
	}
	
	class_destroy(cls);
	unregister_chrdev(major, name);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");




