/******************************** description *********************************

	ioctl_waterled_tets

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
	printk("%s --%d.\n", __FUNCTION__, __LINE__);

	return 0;
}

int demo_close(struct inode *inode, struct file *filp)
{
	printk("%s --%d.\n", __FUNCTION__, __LINE__);

	return 0;
}

ssize_t demo_read(struct file *filp, char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s --%d.\n", __FUNCTION__, __LINE__);
	//copy_to_user(void __user * to, const void * from, unsigned long n)

	return 0;
}

ssize_t demo_write(struct file *filp, const char __user *userbuf, size_t size, loff_t *offset)
{
	printk("%s --%d.\n", __FUNCTION__, __LINE__);
	//copy_from_user(void * to, const void __user * from, unsigned long n)

	return 0;
}

long demo_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	led_node_t *led = (led_node_t *)args;
	int led_which;
	
	printk("%s --%d.\n", __FUNCTION__, __LINE__);

	led_which = led->which;
	switch(cmd)
	{
	case LEDON:
		if (led_which == 0)
		{
			printk("led0 on.\n");
		}
		else if (led_which == 1)
		{
			printk("led1 on.\n");
			writel(readl(gpx1dat_vir) | (0X1 << 0), gpx1dat_vir);
		}
		else if (led_which == 2)
		{
			printk("led2 on.\n");
		}
		break;
	case LEDOFF:
		if (led_which == 0)
		{
			printk("led0 off.\n");
		}
		else if (led_which == 1)
		{
			printk("led1 off.\n");
			writel(readl(gpx1dat_vir) & (0X1 << 0), gpx1dat_vir);
		}
		else if (led_which == 2)
		{
			printk("led2 off.\n");
		}
		break;
	default:
		printk("cmd is error.\n");
	}

	return 0;
}


const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = demo_open,
	.release = demo_close,
	.read = demo_read,
	.write = demo_write,
	.unlocked_ioctl = demo_ioctl,
};

static void gpio_iounmap(void)
{
	iounmap(gpx1con_vir);
}

static void gpio_ioremap(void)
{
	gpx1con_vir = ioremap(GPX1CON, 4);
	gpx1dat_vir = gpx1con_vir + 4;
}

static void led_init(void)
{
	gpio_ioremap();

	writel(( readl(gpx1con_vir) & (~(0xF << 0)) ) | (0x1 << 0),gpx1con_vir);
}

static int __init demo_init(void)
{
	printk("%s --%d.\n", __FUNCTION__, __LINE__);

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
	printk("%s --%d.\n", __FUNCTION__, __LINE__);

	gpio_iounmap();
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, name);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");



