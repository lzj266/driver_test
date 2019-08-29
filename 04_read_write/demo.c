/******************************** description *********************************

	read_write

 ******************************************************************************
  Filename : test.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月27日
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>



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

/*
	应用层: ssize_t read(int fd, void *buf, size_t count);
			@param1: 文件描述符
			@param2: 将读到的信息，存放在buf指定的内存空间中
			@param3: 要读取的字节数(不是读到的数据大小)
			@return: 实际读取到的字节数
	驱动层: ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
			@param1: 文件指针
			@param2: 应用层read函数的参数2，指向持有被写入数据的缓存
			@param3: 应用层read函数的参数3，请求的传输数据大小
			@param4: 一个指针指向一个“long offset type”对象, 它指出用户正在存取的文件位置
			@return: 正确读取的字节数
	实现:  	unsigned long copy_to_user(void __user *to, const void *from, unsigned long n)
			@param1: 用户空间缓存区首地址
			@param2: 内核空间的缓存区首地址
			@param3: 实际拷贝的字节数
			@return: 0成功，非0出错
			该函数调用了access_ok来验证用户空间的内存是否真实可读写，避免在内核中的缺页故障带来的一些问题
*/
char kbuf[4] = {1,2,3,4};
ssize_t demo_read(struct file *filep, char __user *userbuf, size_t size, loff_t *offset)
{
	int rbytes;	/* 定义的变量要写在函数内部最前面，否则会报警告 */

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	printk("size = %d.\n", size);
	
	rbytes = copy_to_user(userbuf, kbuf, size);
	if (rbytes != 0)
	{
		printk("copy_to_user failed.\n");
		return -1;
	}

	return 0;
}

/*
	应用层: ssize_t write(int fd, void *buf, size_t count);
			@param1: 文件描述符
			@param2: 从buf指向的空间中拿取信息，写入fd中
			@param3: 写入的字节数( 通常：sizeof(buf) )
			@return: 成功写入的字节数
	驱动层: ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
			@param1: 文件指针
			@param2: 应用层read函数的参数2，指向持有被写入数据的缓存
			@param3: 应用层read函数的参数3，请求的传输数据大小
			@param4: 一个指针指向一个“long offset type”对象, 它指出用户正在存取的文件位置
			@return: 正确写入的字节数
	实现:  	unsigned long copy_from_user(void __user *to, const void *from, unsigned long n)
			@param1: 内核空间的缓存区首地址
			@param2: 用户空间缓存区首地址
			@param3: 实际拷贝的字节数
			@return: 0成功，非0出错
			该函数调用了access_ok来验证用户空间的内存是否真实可读写，避免在内核中的缺页故障带来的一些问题
*/
ssize_t demo_write(struct file *filep, const char __user *userbuf, size_t size, loff_t *offset)
{
	int i = 0;
	int wbytes;

	printk("%s -- %d.\n", __FUNCTION__, __LINE__);
	printk("size = %d.\n", size);
	
	wbytes = copy_from_user(kbuf, userbuf, size);
	if(wbytes){
		printk("copy_from_user failed.\n");
		return -1;
	}
	
	for(i = 0;i < 4;i ++){
		printk("kbuf[%d] : %d.\n", i, kbuf[i]);
	}

	return 0;
}

int major;
const char * name = "demoname";
const struct file_operations fops = {
	.open = demo_open,
	.release = demo_release,
	.read = demo_read,
	.write = demo_write,
};

struct class *cls;
struct device *dev;


static int __init demo_init(void)
{
	printk("%s -- %d.\n", __FUNCTION__, __LINE__);

	major = register_chrdev(0, name, &fops);		/* /proc/devices */
	if (major <= 0)
	{
		printk("register_chrdev failed.\n");
		goto err0;
	}
	
	cls = class_create(THIS_MODULE, "char_class");	/* sys/class/xxx */
	if (cls == NULL)
	{
		printk("class_create failed.\n");
		goto err1;
	}

	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "chrdev%d", 0);	/* /dev/chrdev0 */
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




