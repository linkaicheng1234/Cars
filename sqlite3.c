#include<stdio.h>
#include<stdlib.h>
#include"sqlite3.h"

//exec的回调
int displaycb(void *para,int ncolumn,char ** columnvalue,char *columnname[])  
{  
    int i;
    printf("total column is %d\n",ncolumn);  
  
    for(i = 0;i < ncolumn; i++)  
    {  
        printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);  
		
	}  
   
    return 0;  
}  

//插入数据
int insert_date(sqlite3* db,char *sql)
{
	//执行sql语句,插入语句
	char * errmsg;
	int ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(ret != SQLITE_OK)  
    {  
        printf("insert table error : %s\n",errmsg);  
		return -1;
    } 
	return 0;
}

//删除数据
int delete_date(sqlite3* db,char *sql)
{
	//执行sql语句,插入语句
	char * errmsg;
	int ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(ret != SQLITE_OK)  
    {  
        printf("delete table error : %s\n",errmsg);  
		return -1;
    } 
	return 0;
}

//显示表
void show_date(sqlite3 *db,char *sql)
{
	char *errmsg;
	int ret = sqlite3_exec(db,sql,displaycb,NULL,&errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("select error : %s\n",errmsg);  
        return ; 
    }  
}

//查询语句
void find_date(sqlite3 *db,char *sql)
{
	int ret;
	char *errmsg;
	char **resultp;
	int nrow;//行
	int ncolumn;//列
	//int ret = sqlite3_exec(db,sql,,NULL,&errmsg);  
    sqlite3_get_table(db,sql,&resultp,&nrow, &ncolumn,&errmsg);
	
	if(ret != SQLITE_OK)  
    {  
        printf("select error : %s\n",errmsg);  
        return ; 
    }  
}


//插入信息语句
void  insert_msg(sqlite3* db,int id)
{
	char sql[100];

	sprintf(sql,"insert into mytable (Cardid)values(%d);",id);  		
	insert_date(db,sql);
	printf("%d is joining\n",id);
			
}


//删除信息语句
void delete_msg(sqlite3* db,int id)
{
	
	char sql[100];
	
	sprintf(sql,"delete from mytable where Cardid = %d;",id);  
			
	delete_date(db,sql);
	printf("%d is exiting\n",id);
}

//查询车牌是否存在数据库，存在则返回时间，不存在返回NULL
char* find_in_data(sqlite3* db,int id)
{
	char sql[100];
	char **resultp;
	int nrow;//行
	int ncolumn;//列
	char *errmsg;
	char *time;
	sprintf(sql,"SELECT logtime FROM mytable WHERE Cardid  is '%d';",id);  
	
	//非回调select查询
	sqlite3_get_table(db,sql,&resultp,&nrow, &ncolumn,&errmsg);
	//int index = ncolumn;
	//int i,j;
	//resultp：第0行 为 字段名称，之后 为 关键值
	/*  for(i = 0;i<=nrow;i++)
	{
		index = ncolumn*i;
		
		for(j = index;j<index+ncolumn;j++)
		{
			printf("%s\t",resultp[j]);
		}
		printf("\n");
		
	}  */
	if(nrow == 0)
	{
		return NULL;
	}
	else
	{
			
		//printf("%s\n",resultp[1]);
		time = resultp[1];
		return time;
	}
}


//从数据库获得总有几辆车停在车库里
int carin_num(sqlite3* db)
{
	char sql[100];
	char **resultp;
	int nrow;//行
	int ncolumn;//列
	char *errmsg;
	sprintf(sql,"SELECT * FROM mytable");  
	
	//非回调select查询
	sqlite3_get_table(db,sql,&resultp,&nrow, &ncolumn,&errmsg);
	
	
	return nrow;
}