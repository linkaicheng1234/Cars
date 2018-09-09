all:
	arm-linux-gcc lcd.c sqlite3.c mybee.c rfid.c common.c project.c   -o project -I sqlite-arm-lib/ -L sqlite-arm-lib/ -lsqlite3 -lpthread
clean:
	rm *.o project
