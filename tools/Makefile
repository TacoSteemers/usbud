CC?=gcc

all:
	$(CC) -Wall -g -O0 -o ../bin/usbstoragebackup ../code/detection.c ../code/daemon.c ../code/processArguments.c ../code/bookKeeping.c ../code/util.c ../code/backup.c
	$(CC) -Wall -g -O0 -o ../bin/uslist ../code/uslist.c ../code/detection.c ../code/util.c
clean:
	rm -f ../bin/usbstoragebackup
	rm -f ../bin/uslist

