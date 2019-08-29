/******************************** description *********************************

	read_write

 ******************************************************************************
  Filename : test.c
  Version  : 1.0
  Author   : liuzhijie
  Email    : liuzhijie266@163.com
  Date     : 2019年8月27日
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char *pathname ="/dev/chrdev0";

int main(int argc, const char *argv[])
{	
	int fd, i = 0;
	char buf[4] = {0};
	size_t rbytes;

	fd = open(pathname, O_RDWR, 0664);
	if (fd < 0) 
	{
		printf("open failed\n");
		return -1;
	}

	rbytes = read(fd, buf, 4);
	printf("rbytes is %d\n", rbytes);

	for (i=0; i<4; i++)
	{
		printf("buf[%d] = %d\n", i, buf[i]);
	}

	sleep(3);
	buf[0] = 11;
	buf[2] = 33;
	write(fd, buf, sizeof(buf));

	sleep(3);
	close(fd);
	
	return 0;
}


