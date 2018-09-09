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
        printf("insert table error : %s\n",errmsg);  
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

int main()
{
	sqlite3 *db;
	char * errmsg;
	char *sql;
	
	
	//	打开数据库
	sqlite3_open("cardata.db",&db);
	
	sql = "create table if not exists mytable (Cardid INTEGER PRIMARY KEY,logtime TIMESTAMP default (datetime('now', 'localtime')) );";  
	//sql = "create table if not exists mytable (id INTEGER PRIMARY KEY AUTOINCREMENT,Cardid INTEGER ,logtime TIMESTAMP default (datetime('now', 'localtime')) );";  
   //sql = "create table if not exists mytable (Cardid integer ,logtime TIMESTAMP default (datetime('now', 'localtime')) );";  
   //执行sql语句,创建表
	int ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);  
  
    if(ret != SQLITE_OK)  
    {  
        printf("create table error : %s\n",errmsg);  
		return -1;
    }  
	
	char sql2[100];
	//执行sql语句,插入语句
	int id=10;
	sprintf(sql2,"insert into mytable (Cardid)values(%d);",id);  
	insert_date(db,sql2);

	id = 11;
	sprintf(sql2,"insert into mytable (Cardid)values(%d);",id);  
	insert_date(db,sql2);
	
	id = 13;
	sprintf(sql2,"insert into mytable (Cardid)values(%d);",id);  
	insert_date(db,sql2);
	
	//执行sql语句,查询表，回调
	sql = "select * from mytable;";  
	show_date(db,sql);
	
	//取当前插入位置
	int  q = sqlite3_last_insert_rowid(db);
	printf("q = %d\n",q);

	
	id = 11;
	sprintf(sql2,"DELETE FROM mytable WHERE Cardid = %d;",id);  
	//执行sql语句，删除数据
	delete_date(db,sql2); 
	

	char **resultp;
	int nrow;//行
	int ncolumn;//列
	//非回调select查询
	sqlite3_get_table(db,sql,&resultp,&nrow, &ncolumn,&errmsg);
	int index = ncolumn;
	int i,j;
	//resultp：第0行 为 字段名称，之后 为 关键值
	for(i = 0;i<=nrow;i++)
	{
		index = ncolumn*i;
		
		for(j = index;j<index+ncolumn;j++)
		{
			printf("%s\t",resultp[j]);
		}
		printf("\n");
		
	}
	
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放 
	sqlite3_free_table(resultp);
	
 	sql = "select logtime FROM mytable WHERE Cardid = 13;";  
	ret = sqlite3_exec(db,sql,displaycb,NULL,&errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("select error : %s\n",errmsg);  
        return ; 
    } 
 	
	/* id = 13;
	sprintf(sql2,"select * from mytable where Cardid = %d;",id);  
	ret = sqlite3_exec(db,sql2,displaycb,NULL,&errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("select error : %s\n",errmsg);  
        return ; 
    }   */
	
	
	sql = "select logtime from mytable where Cardid = 11;";
	char **resultp1;
//	int nrow;//行
//	int ncolumn;//列
	
	//非回调select查询
	sqlite3_get_table(db,sql,&resultp1,&nrow, &ncolumn,&errmsg);
	index = ncolumn;
	//int i,j;
	//resultp：第0行 为 字段名称，之后 为 关键值
	for(i = 0;i<=nrow;i++)
	{
		index = ncolumn*i;
		
		for(j = index;j<index+ncolumn;j++)
		{
			printf("%d %s\t",j,resultp1[j]);
		}
		printf("\n");
		
	}
	if(nrow != 0)
		printf("hello %s\t",resultp1[1]);

	//关闭数据库
	sqlite3_close(db);
	return 0;
}