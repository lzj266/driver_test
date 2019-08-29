#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	char *pathname = "/dev/chrdev0";
	int fd;
	fd = open(pathname, O_RDWR, 0666);
	if (fd == -1)
	{
		printf("open failed\n");
		return -1;
	}

	sleep(3);

	close(fd);

	return 0;
}













