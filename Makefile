all:
	arm-linux-gcc *.c -o project  -I sqlite-arm-lib -L sqlite-arm-lib -lsqlite3  -lpthread
clean:
	rm *.o project
