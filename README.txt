开发板时间修改
date 050216372018.10
5月2日16时37分2018年.10秒



源代码：
	project.c lcd.c common.c rfid.c sqlite3.c
可执行文件：
	project

依赖库：
	./sqlite-arm-lib/libsqlite3.so

编译：
	arm-linux-gcc project.c lcd.c common.c rfid.c sqlite3.c -o project  -I sqlite-arm-lib -L sqlite-arm-lib -lsqlite3  -lpthread



步骤：
	在开发板下新建一个car文件夹：
		mkdir  /car
		cd /car
	把project可执行文件  和 图片压缩包pic.tar.gz 放到开发板/car下

	解压压缩包：
		tar -zxvf pic.tar.gz

	修改权限：
		chmod 777 project 


	运行：
		./project

运行
	rfid接在开发板串口1上
	查找数据库获得车库车数
	通过rfid识别，第一次入库，判断车位数有剩余，则记录时间到数据库中，车数加1，否则无法记到数据库中
		      第二次出库，求出时间间隔，算出金额，显示金额，车数减1	



