usbud
=====

Did you ever make space on your SD card, but forgot to copy your photos to your harddrive?

Did you ever carry around your important documents an a USB stick, and one day the stick just died... taking your documents with it?

The goal of this project is to make sure your portable files are automatically backed up when you attach your USB device to your computer.

usbud stands for USB Storage Back Up Daemon.
The daemon will back up all mounted partitions on USB storage devices. Daemon is an older word for "background service".
This project is known to work on some GNU/Linux distributions. usbud depends on the availability of items such as '/sys/block' and '/proc/mounts', the application 'rsync'.

**Important note: the USB storage back up daemon should not be confused with a 'full' backup solution. It should be used in conjunction with a 'full' backup solution.**

## Table of Contents:

*  [Features](#features)
*  [Instructions on building the service](#instructions-on-building-the-service)
*  [Instructions on running the service](#instructions-on-running-the-service)
*  [Notes on what is supported, and what isn't](#notes-on-what-is-supported-and-what-isnt)
*  [Notes on device labels and multicard readers](#notes-on-device-labels-and-multicard-readers)
*  [Other details](#other-details)
*  [To be implemented:](#to-be-implemented)
*  [License](#license)

## Features

- Will create a full backup of your USB storage's mounted partitions
- Will not backup data that has not changed
- Supports cardreaders and SD cards
- Supports black- and whitelisting
- Can provide notifications 

The daemon (service) supports per-partition blacklisting and whitelisting. Either list consists of a file that contains one (custom) device id on each line. One can use the included 'uslist' executable to find the device ids.

## Instructions on building the service
This project should build succesfully with any C compiler.
It uses make. The Makefile is located in ./tools/.
The output can be found in ./bin

	taco@computer:~/workspace/usbud$ cd tools
	taco@computer:~/workspace/usbud/tools$ make
	cc -Wall -g -O0 -o ../bin/usbstoragebackup ../code/detection.c ../code/daemon.c ../code/processArguments.c ../code/bookKeeping.c ../code/util.c ../code/backup.c
	cc -Wall -g -O0 -o ../bin/uslist ../code/uslist.c ../code/detection.c ../code/util.c
	taco@computer:~/workspace/usbud/tools$ ls ../bin
	usbstoragebackup  uslist

## Instructions on running the service
The daemon can be run with a command like the following:

    ./usbstoragebackup --target /home/user/backup --blacklist /home/user/backup/blacklist.txt;

The blacklist file might look like the following:

    Western Digital My Book 57442D574341E53443303937383936 My Book
    Kingston DataTraveler 2.0 899000000000000000000073 DataTraveler 2.0
    InphiCHN Mass Storage Device USB Flash Disk

The listed items will not be backed up. These identifiers can be obtained via the uslist application:

	$ ./uslist
    
If you wish to receive notifications on the desktop, you can use one of the following arguments:

    --notification notify-send
    --notification xmessage
    
It is recommended that you use notify-send, if available on your system, because it is integrated with modern distributions. 
When using xmessage, the notification will be shown for three seconds. Any pending backups will start after their related xmessage notification is gone.

## Notes on what is supported, and what isn't

Here are some examples of identifiers:

	Generic MultiCard Device 00000000000006 USB  SD Reader1
	InphiCHN Mass Storage Device USB Flash Disk1
	Kingston DataTraveler 2.0 899000000000000000000073 DataTraveler 2.01
	Kingston DataTraveler 2.0 899000000000000000000094 DataTraveler 2.01
	Kingston DataTraveler 2.0 8998011620080115142551D9 DataTraveler 2.01
	Generic Mass Storage 0ABE6A52 Flash Disk1
    Generic Mass Storage 0ABE6A52 Flash Disk2
	Apple iPod 000A27001CB7AFED iPod1

The first is a card that has been inserted in an 'all-in-one' card reader. This  is supported. You may encounter problems when you use several card readers that were created by the same manufacturer. Maybe they would have to be of the same model, before problems occur.
The second is a cheap thumb drive that does not carry any identifying meta data at all. If you plug in a second one that was made by the same manufacturer, the daemon system will become confused. Most likely it will remove the backup that it has for the first drive, and place the backup of the second drive in it's stead. 
The next five items are regular thumb drives that will work quite well. One of them has two partitions on it.
The last item is an iPod shuffle. Not every audio device is supported. To the best of my knowledge, all Creative Zen devices are not and are unlikely to ever be supported, because they use the 'MTP' protocol (Media Transfer Protocol). I have a Zen that I have tested this with. MTP is related to the Picture Transfer Protocol, and as such I'm quite certain that PTP devices are not supported either.

## Notes on device labels and multi-card readers
If you have several cards that use the same slot in your multi-card reader, the daemon may become confused and overwrite their backups with a backup of the other card.
If this happens, it is because the cards themselves have the same label. The usbud daemon does not have access to the kind of identifying information that it can find on regular thumb drives. As such, it can only depend on the card labels.
To make sure that usbud doesn't confuse your cards with eachother, you can label them. When using a Microsoft Windows or Apple operating system, this should be quite easy.
A the moment of writing, it is not that easy under a GNU/Linux operating system. The documentation on the Ubuntu operating system (a GNU/Linux distribution) provides [details on this page](https://help.ubuntu.com/community/RenameUSBDrive).

## Other details 

You may want to have usbud be started on system startup. In that case, I refer you to your platform's documentation to find the recommended way of doing so. It is safe to kill the daemon while a backup is running - the backup will not be interrupted or corrupted and you will still receive notification after the daemon has ended.

Backups will not be run in parallel. For most users, running multiple backups in parallel will not make sense because one would run in to write performance limits on the backup target directory or read limitations on USB hubs. As a result, it could take longer for both of the backups to finish than it would if they had been run sequentially.

Please note that re-inserting a device, even in a different slot, does not necessarily trigger a new backup. It will only trigger a new backup if the device has been removed longer than the time between each run (30 seconds by default) before being inserted again. Which slot it is inserted in does not matter.

Log output can be found in syslog.

## To be implemented:
- Configuration options for storage device size limits. Devices (or rather, partitions) above a certain size would not be backed up.

Low priotity:
- Remove dependency on rsync
- Configuration options for target directory size limits
- Add the following optional behavior when hitting target directory size limits (if  a target directory size limit is configured): remove the oldest backups until enough space is created. The idea behind this is that the oldest backups should already have been backed up by the system's actual backup system, outside of usbud's control.
- Determining the difference between a 'thumb drive' and external storage hard disks
- Configuration options for ignoring external storage hard disks or 'thumb drives'

## License
This program is distributed under the terms of the GNU General Public License. A version of this license should have been provided with the program. It can also be found on: [http://www.gnu.org/licenses/gpl-3.0.txt](http://www.gnu.org/licenses/gpl-3.0.txt)

