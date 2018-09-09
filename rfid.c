/************************************************
*  * note: Smart Home RFID test program
*  * date: 2013-05-8
gnd
rx
tx
vcc
************************************************/
#include "rfid.h"



unsigned char cardid[4];
unsigned char KEYA_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char KEYB_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char DataWriteBuf[16];
unsigned char DataReadBuf[16];
unsigned char DataBfr[16];

unsigned int ID;
unsigned char Room_ID[6];
unsigned char Enter_check[16];
unsigned char sector = SECTOR;//选择将数据写到哪个块

static int rfid_fd;

/*计算校验和*/
unsigned char CalBCC(unsigned char *buf, int n)
{
	int i;
	unsigned char bcc=0;
	for(i = 0; i < n; i++)
	{
		bcc ^= *(buf+i);
	}
	return (~bcc);
}

//天线请求
int PiccRequest(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长= 7 Byte
	WBuf[1] = 0x82;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'A';	//命令= 'A'
	WBuf[3] = 0x01;	//信息长度= 1
	WBuf[4] = 0x52;	//请求模式:  ALL=0x52
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[6] = 0x03; 	//结束标志

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, 7);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccRequest select error\n");
			break;
		case  0:
			perror("PiccRequest timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 8);
			if (ret < 0)
			{
				perror("PiccRequest read fail\n");
				break;
			}
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功  返回卡的类型
			{
				DataBfr[0] = RBuf[4];
				DataBfr[1] = RBuf[5];
				FD_CLR(fd,&rdfd);
				return 0;
			}
			break;
	}
	
	FD_CLR(fd,&rdfd);
	
	return 1;
}


/*防碰撞，获取范围内最大ID*/
int PiccAnticoll(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int ret, i;
	fd_set rdfd;
	struct timeval timeout;
	

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x08;	//帧长= 8 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'B';	//命令= 'B'
	WBuf[3] = 0x02;	//信息长度= 2
	WBuf[4] = 0x93;	//防碰撞0x93 --一级防碰撞
	WBuf[5] = 0x00;	//位计数0
	WBuf[6] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[7] = 0x03; 	//结束标志

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	write(fd, WBuf, 8);

	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAnticoll select error\n");
			break;
		case  0:
			perror("PiccAnticoll Timeout:");
			break;
		default:
			ret = read(fd, RBuf, 10);
			if (ret < 0)
			{
				perror("PiccAnticoll read fail\n");
				break;
			}
			if (RBuf[2] == 0x00) //应答帧状态部分为0 则获取ID 成功
			{
				ID = (RBuf[7]<<24) | (RBuf[6]<<16) | (RBuf[5]<<8) | RBuf[4];
				//memcpy(cardid,&RBuf[4],4);
				//cardid = (RBuf[7]<<24) | (RBuf[6]<<16) | (RBuf[5]<<8) | RBuf[4];
				
				return 0;
			}
	}
	
	return -1;
}

//选择
int PiccSelect(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x0B;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'C';	//命令= 'C
	WBuf[3] = 0x05;	//信息长度= 5
	WBuf[4] = 0x93;	//请求模式:  ALL=0x52
    memcpy(&WBuf[5],cardid,4);
	WBuf[9] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[10] = 0x03; 	//结束标志

	timeout.tv_sec = 300;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	
	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccSelect error\n");
			break;
		case  0:
			perror("PiccSelect timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				perror("PiccSelect  read fail\n");
				break;
			}
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
				return 0;
			}
			
			break;
	}
	
	return -1;
}

//验证密钥
int PiccAuthKey(int fd,int sector,unsigned char KeyAB)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x12;	//帧长= 17 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'F';	//命令= 'F'
	WBuf[3] = 12;	//信息长度= 0x0c
	WBuf[4] = KeyAB;
	memcpy(&WBuf[5],cardid,4);
	memcpy(&WBuf[9],KEYA_BUF,6);
	WBuf[15]=sector;   //读写哪个区
	WBuf[16] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[17] = 0x03; 	//结束标志

	timeout.tv_sec = 300;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAuthKey error\n");
			break;
		case  0:
			perror("PiccAuthKey timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				perror("PiccAuthKey  read fail\n");
				break;
			}
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			    printf("PiccAuthKey success\n");
				return 0;
			}
			break;
	}
	return -1;
}

//读取信息
int PiccRead(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'G';	//命令= 'g
	WBuf[3] = 0x01;	//信息长度= 1
	WBuf[4] = sector;	//读哪个扇区
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[6] = 0x03; 	//结束标志

	timeout.tv_sec = 300;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccRead error\n");
			break;
		case  0:
			perror("PiccRead timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 23);
			printf("read ret = %d\n",ret);	//16
			if (ret < 0)
			{
				perror("PiccRead  read fail\n");
				break;
			}
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        usleep(10*1000);
			        read(fd,&RBuf[ret],6);
			        memcpy(DataReadBuf,&RBuf[4],16); //读取到该扇区的数据
					
				return 0;
			}
			break;
	}
	return -1;
}

//写入数据
int PiccWrite(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 23;	//帧长= 23 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'H';	//命令= 'H
	WBuf[3] = 0X11;	//信息长度= 17
	WBuf[4] = sector;   //写哪个扇区
        memcpy(&WBuf[5],DataWriteBuf,16);
	WBuf[21] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[22] = 0x03; 	//结束标志

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
        
	write(fd, WBuf, 23);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccWrite error\n");
			break;
		case  0:
			printf("PiccWrite timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccWrite  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        printf("PiccWrite card success\n");
				return 0;
			}
			break;
	}
	return -1;
}
//-----------------------------
unsigned int rfid_get_id(int fd)
{
	/*请求天线范围的卡*/
	if ( PiccRequest(fd) )
	{
		//printf("The request failed!\n");
		//close(fd);
		return -1;
	}
	/*进行防碰撞，获取天线范围内最大的ID*/
	if( PiccAnticoll(fd) )
	{
		//printf("Couldn't get card-id!\n");
		//close(fd);
		return -1;
	}
	
	if(ID == 0)
	{
		return -1;
	}
	printf("card ID = %d\n", ID);
	return ID;
}

int rfid_open(char *rfid_path)
{
	rfid_fd = open(rfid_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (rfid_fd < 0)
	{
		fprintf(stderr, "Open Gec210_ttySAC1 fail!\n");
		return -1;
	}
	return rfid_fd;
}

void rfid_close(int fd)
{
	close(fd);
}
