all:
	gcc -Wall -g -O0 -o ../bin/usbstoragebackup ../code/detection.c ../code/daemon.c ../code/processArguments.c ../code/bookKeeping.c ../code/util.c
	gcc -Wall -g -O0 -o ../bin/uslist ../code/uslist.c ../code/detection.c ../code/util.c
clean:
	rm -f usbstoragebackup
	rm -f uslist
