all:
	gcc -Wall -g -o ../bin/usbstoragebackup ../code/detection.c ../code/daemon.c ../code/processArguments.c ../code/bookKeeping.c
	gcc -Wall -g -o ../bin/uslist ../code/uslist.c ../code/detection.c
clean:
	rm -f usbstoragebackup
	rm -f uslist
