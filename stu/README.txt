������ʱ���޸�
date 050216372018.10
5��2��16ʱ37��2018��.10��



Դ���룺
	project.c lcd.c common.c rfid.c sqlite3.c
��ִ���ļ���
	project

�����⣺
	./sqlite-arm-lib/libsqlite3.so

���룺
	arm-linux-gcc project.c lcd.c common.c rfid.c sqlite3.c -o project  -I sqlite-arm-lib -L sqlite-arm-lib -lsqlite3  -lpthread



���裺
	�ڿ��������½�һ��car�ļ��У�
		mkdir  /car
		cd /car
	��project��ִ���ļ�  �� ͼƬѹ����pic.tar.gz �ŵ�������/car��

	��ѹѹ������
		tar -zxvf pic.tar.gz

	�޸�Ȩ�ޣ�
		chmod 777 project 


	���У�
		./project

����
	rfid���ڿ����崮��1��
	�������ݿ��ó��⳵��
	ͨ��rfidʶ�𣬵�һ����⣬�жϳ�λ����ʣ�࣬���¼ʱ�䵽���ݿ��У�������1�������޷��ǵ����ݿ���
		      �ڶ��γ��⣬���ʱ�������������ʾ��������1	



