#ifndef _RFID_H_
#define _RFID_H_

//头文件
#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h>
#include <termios.h> 
#include <sys/types.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <time.h>

//宏定义
#define KEYA		0x60
#define KEYB		0x61
#define SECTOR	1

//函数声明
unsigned char CalBCC(unsigned char *buf, int n);
int PiccRequest(int fd);
int PiccAnticoll(int fd);
int PiccSelect(int fd);
int PiccAuthKey(int fd,int sector,unsigned char KeyAB);
int PiccRead(int fd,unsigned char sector);


unsigned int rfid_get_id(int fd);
int rfid_open(char *rfid_path);
void rfid_close(int fd);
#endif