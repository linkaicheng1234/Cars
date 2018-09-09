#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <linux/input.h>

#include "rfid.h"
#include "lcd.h"
#include "sqlite3.h" 
//#define DEV_PATH   "/dev/ttySAC2"      //6818设备定义
#define DEV_PATH   "/dev/s3c2410_serial1"		//210设备定义
#define PIC_PATH "/car/pic/"
#define CARS 1

static int carwei = 0; 
extern char* find_in_data(sqlite3* db,int id);

/*函数声明*/
double get_time_interval(char *tim);
int timetomoney(double timecost);
void show_money(int money);
void get_x_y(int *x,int *y);
void * rfid_thread (void *arg);

/*引用外部函数*/
extern char* find_in_data(sqlite3* db,int id);


/*全局变量*/
int rfid_fd;
static sqlite3 *db;
char * errmsg;
char *sql;
char *jtime;
int ts_fd;

int sum()
{
 HEAD
	int c = 100;
 d3b0539f5a41ce313f4fb298ce05eccaacaace9d
	return 0;
}
/*获得时间间隔
参数：进入时间,
返回: 秒数*/
double get_time_interval(char *tim) 
{
	time_t second,first;
	struct tm tm1;  
    sscanf(tim, "%4d-%2d-%2d %2d:%2d:%2d",      
          &tm1.tm_year,   
          &tm1.tm_mon,   
          &tm1.tm_mday,   
          &tm1.tm_hour,   
          &tm1.tm_min,  
          &tm1.tm_sec); 
	tm1.tm_year -= 1900;
	tm1.tm_mon  -= 1; 
	first = mktime(&tm1);

	second = time(NULL);
	char *tim1 = ctime(&first);
	//printf("tm1 : %s\n",tim1);
	char *tim2 = ctime(&second);
	//printf("tm2 : %s\n",tim2);
	double d = difftime(second,first);
	printf("The difference is: %f seconds\n",d);
	return d;
}

/* 时间转换金钱
参数：秒数
返回：金钱 */
int timetomoney(double timecost)
{
	//int hourcost = timecost/3600.0;
	int hourcost = timecost;
	
	int  money = hourcost * 1;
	return money;
}

/* 显示金额 */
void show_money(int money)
{
	char s[10];
	int n = money;
	int T=0;//总位数
	char buf[40];
	while(n)
	{  
		s[T] = (n%10)+48;
		n /= 10;
		T++;
	}  //每一位从个位数到高位数依次存储在数组
	T--;
	int i = 0;
	for(i = T;i>=0;i--) //
	{
		//printf("*(s-i) = %c\n",s[i]);
		sprintf(buf,"%s%c.bmp",PIC_PATH,s[i]);
		lcd_draw_bmp(100*(T-i),0,buf);
			
	}

}

/* 获取x，y坐标 */
void get_x_y(int *x,int *y)
{
	struct input_event ts;
	int count = 0;
	while(1)
	{
		//读取坐标值
		read(ts_fd,&ts,sizeof(ts));
		if(ts.type == EV_ABS && ts.code == ABS_X)
		{
			*x = ts.value;
			count++;
		}
		if(ts.type == EV_ABS && ts.code == ABS_Y)
		{
			*y = ts.value;
			count++;
		}
		if(count == 2)
		{
			//获取x，y后返回
			printf("x = %d, y= %d\n",*x,*y);
			return ;
		}
	}
}

/* 线程创建，等待读取rfid */
void * rfid_thread (void *arg)
{
	int id = 0;
	char buf[50];
	
	while(1)
	{
		//等待获得id号
		if((id = rfid_get_id(rfid_fd) )!= -1)
		{
			sleep(1);
			//判断id号是否存在数据库，
			if((jtime = find_in_data(db,id)) == NULL)//不存在为NULL，第一次刷卡进入
			{
				sprintf(buf,"%smain.bmp",PIC_PATH);
				lcd_draw_bmp(0,0,buf);
				
				
				//判断是否满车
				if(carwei == CARS)
				{
					printf("car full\n");
					sprintf(buf,"%sfull.bmp",PIC_PATH);
					lcd_draw_bmp(50,350,buf);
					//声音提示
					bee_waring();
					continue;
				}
				
				//显示“欢迎光临”
				sprintf(buf,"%sjoin.bmp",PIC_PATH);
				lcd_draw_bmp(50,200,buf);
				sleep(2);
				sprintf(buf,"%sblue.bmp",PIC_PATH);
				lcd_draw_bmp(50,200,buf);
				
				//声音提示
				bee_waring();
				
				//车位加1
				carwei++;
				printf("%d joining \n",id);
				//插入数据库
				insert_msg(db,id);
				
				
			}
			else //存在则第二次刷卡退出
			{
				printf("%d exiting: %s \n",id,jtime);
				//声音提示
				bee_waring();
				
				//获得时间间隔
				double timecost =  get_time_interval(jtime);
				
				//由时间间隔算出消费金额
				int money = timetomoney(timecost);
				printf("total money = %d\n",money);
				
				//显示金额数
				sprintf(buf,"%smain.bmp",PIC_PATH);
				lcd_draw_bmp(0,0,buf);
				show_money(money);
				
				//删除入库记录
				delete_msg(db,id);
				
				//车位数减1
				carwei--;
				/* if(carwei != CARS)
				{
					sprintf(buf,"%sblue.bmp",PIC_PATH);
					lcd_draw_bmp(50,350,buf);
					continue;
				} */
				
				sleep(1);
			}
			
			continue;
		}	
		sleep(1);
	}
}


int main()
{
	//分配内存
	jtime = malloc(100);
	

	//打开显示屏
	lcd_open("/dev/fb0");
	//显示主界面
	char buf[50];
	sprintf(buf,"%smain.bmp",PIC_PATH);
	lcd_draw_bmp(0,0,buf);
	
	system("insmod mybee.ko");
	//初始化蜂鸣器
	bee_init();
	
	//打开串口rfid
	if((rfid_fd = rfid_open(DEV_PATH)) < 0)
	{
		printf("open rfid fail\n");
		return -1;
	}
		
	//打开数据库
	sqlite3_open("cardata.db",&db);
	
	sql = "create table if not exists mytable (Cardid INTEGER PRIMARY KEY,logtime TIMESTAMP default (datetime('now', 'localtime')) );";  
	//执行sql语句,创建表
	int ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("create table error : %s\n",errmsg);  
		return -1;
    }  
	
	//从数据库获得总有几辆车停在车库里
	carwei = carin_num(db);
	//判断是否满车
	if(carwei == CARS)
	{
		printf("car full\n");
		sprintf(buf,"%sfull.bmp",PIC_PATH);
		lcd_draw_bmp(50,350,buf);
	}
	
	
	//打开触摸屏
	ts_fd = open("/dev/event0",O_RDWR);
	if(ts_fd == -1)
	{
		printf("open lcd fail\n");
		return -1;
	}
	
	//创建线程
	pthread_t rfid_pid;
	pthread_create(&rfid_pid,NULL,&rfid_thread,NULL);
	
	printf("init finish\n");
	
	int x,y;
	while(1)
	{
		//获取x,y坐标
		get_x_y(&x,&y);
		//判断区域，按下“确定键”
		if(x>480 && x<700 && y>350 && y<450)
		{

			sprintf(buf,"%smain.bmp",PIC_PATH);
			lcd_draw_bmp(0,0,buf);
	
		}
	}
	
	//关闭显示屏，触摸屏，串口rfid,bee
	close_lcd();
	close(ts_fd);
	rfid_close(rfid_fd);
	bee_uinit();
	return 0;
}


