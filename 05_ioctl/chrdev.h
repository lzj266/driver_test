/******************************** description *********************************

	ioctl_test

 ******************************************************************************
  Filename : chrdev.h
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月28日
 *****************************************************************************/

#ifndef _CHRDEV_H_
#define _CHRDEV_H_

typedef struct led_node{
	int which;	// 那一盏灯
	int status;	// 灯的状态，0表示灭，1表示亮
}led_node_t;

#define led_magic 'w'
#define LEDON  _IOW(led_magic, 0, struct led_node)
#define LEDOFF _IOW(led_magic, 1, struct led_node)
#if 0
	@param1: 幻数 
	@param2: 命令码 
	@param3: 指定参数所占内存空间的大小,会调用#define _IOC_TYPECHECK(t) (sizeof(t))，所以不用写sizeof()
#endif

#endif /* chrdev.h */
