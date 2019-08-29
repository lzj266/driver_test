#include <linux/module.h>

/*
	  函数名可以任意指定又带来了一个新的问题，那就是有可能和内核中的已有函数重
	名，为了避免重名而带来的重复定义问题，在函数名前加static关键字修饰，经static
	修饰的函数链接属性为内部，从而解决了上述问题。这就是几乎所有的驱动函数都要
	加static修饰的原因。
	  模块的初始化函数会且仅会被调用一次，在调用完成后，该函数不应该被再次调用，
	所以该函数的内存应该被释放掉，在函数前加__init可以达到此目的。
	  __init是把标记的函数放在ELF文件(可执行文件)的特定代码段，在模块加载这些时
	将会单独分配内存，这些函数调用成功后，模块的加载程序会释放这部分的内存空间。
	  __exit用来修饰清除函数，和__init的作用类似，但用于模块的卸载，如果模块不允
	许卸载，那么这段代码完全就不用加载。
*/
static int __init demo_init(void)
{
	printk("%s -- %s -- %d\n", __FILE__, __FUNCTION__, __LINE__);

	return 0;
}

static void __exit demo_exit(void)
{
	printk("%s -- %s -- %d\n", __FILE__, __FUNCTION__, __LINE__);
}


/* 
	模块的入口函数 
	  module_init 是一个宏，用于指定 init_module 的函数别名是 demo_init,这样在
	模块的初始化时就可用别名来定义了，module_exit同样。
*/
module_init(demo_init);

/* 模块的出口函数 */
module_exit(demo_exit);

/* 
	  GPL协议。Linux是一个开源的项目，为了使Linux在发展的过程中不成为一个闭源
	的项目，这就要求任何使用Linux源码的个人或组织在免费获得源码并可针对源码做
	任何修改和再发布的同时，必须将修改后的源码发布，这就是GPL许可证协议。
*/
MODULE_LICENSE("GPL");


