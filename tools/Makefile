all:
	gcc -Wall -g -O0 -o ../bin/usbstoragebackup ../code/detection.c ../code/daemon.c ../code/processArguments.c ../code/bookKeeping.c ../code/util.c ../code/backup.c
	gcc -Wall -g -O0 -o ../bin/uslist ../code/uslist.c ../code/detection.c ../code/util.c
clean:
	rm -f ../bin/usbstoragebackup
	rm -f ../bin/uslist

allWin:
	gcc -Wall -g -O0 -o ../bin/usbstoragebackup.exe ../code/daemonWindows.c ../code/processArguments.c ../code/bookKeeping.c ../code/util.c ../code/backup.c ../code/loggingWindows.c ../code/detectionWindows.c ../code/windowsAlternatives.c
	gcc -Wall -g -O0 -o ../bin/uslist.exe ../code/uslist.c ../code/detection.c ../code/util.c ../code/loggingWindows.c
cleanWin:
	del ../bin/usbstoragebackup.exe
	del ../bin/uslist.exe