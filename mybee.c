#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
int fd;
#define GEC210_BEEP_ON 		_IO('B',0)
#define GEC210_BEEP_OFF  	_IO('B',1)
int bee_init(void)
{
	int ret;
	fd = open("/dev/beep_drv", O_WRONLY);
	if(fd <0 ){
		perror("open");
		return -1;
	}
}
void bee_uinit(void)
{
	close(fd);
}

void bee_on(void)
{
	int ret = ioctl(fd, GEC210_BEEP_ON);
	if(ret < 0)
	{
		perror("ioctl");
		return ;
	}
}

void bee_off(void)
{
	int ret = ioctl(fd, GEC210_BEEP_OFF);
	if(ret < 0)
	{
		perror("ioctl");
		return ;
	}
}

void bee_waring(void)
{
	bee_on();
	sleep(1);
	bee_off();
	
}
