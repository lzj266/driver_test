/******************************** description *********************************

	ioctl_waterled_tets

 ******************************************************************************
  Filename : test.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月29日
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "chrdev.h"

#define LED_NUM 3
const char *pathname = "/dev/chrdev0";


int main(int argc, const char *argv[])
{
	int fd, i;
	led_node_t led;

	fd = open("pathname", O_RDWR, 0664);
	if (fd <= 0)
	{
		printf("open failed.\n");
		return -1;
	}

	while (1)
	{
		for (i=0; i<LED_NUM;i++)
		{
			led.which = i;

			led.status = 1;
			ioctl(fd, LEDON, &led);
			sleep(1);

			led.status = 0;
			ioctl(fd, LEDOFF, &led);
			sleep(1);
		}
	}

	close(fd);

	return 0;
}



