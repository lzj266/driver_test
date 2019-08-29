/******************************** description *********************************

	ioctl_test

 ******************************************************************************
  Filename : test.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月28日
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include "chrdev.h"

#define LED_NUM 3
const char *pathname = "/dev/chrdev0";

int main(void)
{
	int fd, i;
	led_node_t led;

	fd = open(pathname, O_RDWR, 0666);
	if (fd < 0)
	{
		printf("open failed.\n");
		return -1;
	}
	printf("fd = %d.\n", fd);

	for (i=0; i<LED_NUM; i++)
	{
		led.which  = i;	//第几盏灯
		led.status = 0;	//灯的状态，0表示灭
		ioctl(fd, LEDOFF, &led);
		sleep(1);

		led.which  = i;
		led.status = 1;	//1表示亮
		ioctl(fd, LEDON, &led);
		sleep(1);
	}

	close(fd);
	
	return 0;
}
