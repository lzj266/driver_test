#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "led.h"

const char *pathname0 = "/dev/led_device0";
const char *pathname1 = "/dev/led_device1";


int main()
{
	int fd0, fd1;

	fd0 = open(pathname0, O_RDWR, 0666);
	if (fd0 < 0)
	{
		printf("open fd0 failed.\n");
		return -1;
	}

	fd1 = open(pathname1, O_RDWR, 0666);
	if (fd1 < 0)
	{
		printf("open fd1 failed.\n");
		return -1;
	}

	while (1)
	{
		ioctl(fd0, LEDON);
		sleep(1);
		ioctl(fd0, LEDOFF);
		sleep(1);

		ioctl(fd1, LEDON);
		sleep(1);
		ioctl(fd1, LEDOFF);
		sleep(1);
	}

	close(fd0);
	close(fd1);
	
	return 0;
}


