usbud
=====

## USB Storage Back Up Daemon
The daemon will back up USB storage devices the moment they are plugged in.
The daemon supports blacklisting and whitelisting. Either list consists of a file that contains one device id on each line. One can use the 'uslist' executable to find the device ids.
Log output can be found in syslog.
This project is known to work on GNU/Linux systems. usbud depends on the availability of items such as '/sys/block' and '/proc/mounts' and the application 'rsync'.
**Important note: the USB storage back up daemon should not be confused with a 'full' backup solution. It should be used in conjunction with a 'full' backup solution.**

## Instructions on running the daemon
The daemon can be run with a command like the following:

    ./usbstoragebackup --target /home/user/mobilestoragebackup --blacklist /home/user/mobilestoragebackup/blacklist.txt;

The blacklist file might look like the following:

    Western Digital My Book 57442D574341E53443303937383936 My Book
    Kingston DataTraveler 2.0 899000000000000000000073 DataTraveler 2.0
    InphiCHN Mass Storage Device USB Flash Disk

These identifiers can be obtained via the uslist application:

	$ ./uslist

You may want to have usbud be started on system startup. In that case, I refer you to your platform's documentation to see find the recommended way of doing so.

Please note that re-inserting a device, even in a different slot, does not necessarily trigger a new backup. It will only trigger a new backup if the device has been removed longer than the time between each run (30 seconds by default) before being inserted again. Which slot it is inserted in does not matter.

## Notes on what is supported, and what isn't

Here are some examples of identifiers:

	Generic MultiCard Device 00000000000006 USB  SD Reader
	InphiCHN Mass Storage Device USB Flash Disk
	Kingston DataTraveler 2.0 899000000000000000000073 DataTraveler 2.0
	Kingston DataTraveler 2.0 899000000000000000000094 DataTraveler 2.0
	Kingston DataTraveler 2.0 8998011620080115142551D9 DataTraveler 2.0
	Generic Mass Storage 0ABE6A52 Flash Disk
	Apple iPod 000A27001CB7AFED iPod

The first is a card that has been inserted in an 'all-in-one' card reader. This  is supported. You may encounter problems when you use several card readers that were created by the same manufacturer. Maybe they would have to be of the same model, before problems occur.
The second is a cheap thumb drive that does not carry any identifying meta data at all. If you plug in a second one that was made by the same manufacturer, the daemon system will become confused. Most likely it will remove the backup that it has for the first drive, and place the backup of the second drive in it's stead. 
The next four items are regular thumb drives that will work quite well. 
The last item is an iPod shuffle. Not every audio device is supported. To the best of my knowledge, all Creative Zen devices are not and are unlikely to ever be supported, because they use the 'MTP' protocol (Media Transfer Protocol). I have a Zen that I have tested this with. MTP is related to the Picture Transfer Protocol, and as such I'm quite certain that PTP devices are not supported either.

## Notes on device labels and multi-card readers
If you have several cards that use the same slot in your multi-card reader, the daemon may become confused and overwrite their backups with a backup of the other card.
If this happens, it is because the cards themselves have the same label. The usbud daemon does not have access to the kind of identifying information that it can find on regular thumb drives. As such, it can only depend on the card labels.
To make sure that usbud doesn't confuse your cards with eachother, you can label them. When using a Microsoft Windows or Apple operating system, this should be quite easy.
A the moment of writing, it is not that easy under a GNU/Linux operating system. The documentation on the Ubuntu operating system (a GNU/Linux distribution) provides [details on this page](https://help.ubuntu.com/community/RenameUSBDrive).

## To be implemented:
- Configuration options for storage device size limits
Low priotity:
- Remove dependence on rsync
- Configuration options for target directory size limits
- Add the following optional behavior when hitting target directory size limits (if  a target directory size limit is configured): remove the oldest backups until enough space is created. The idea behind this is that the oldest backups should already have been backed up by the system's actual backup system, outside of usbud's control.
- Determining the difference between a 'thumb drive' and external storage hard disks
- Configuration options for ignoring external storage hard disks or 'thumb drives'

## Note on building
When building the project with the attached Makefile, a warning should show, like the following:

    ../code/daemon.c: In function ‘main’:
    ../code/daemon.c:21:2: warning: passing argument 1 of ‘processArguments’ from incompatible pointer type [enabled by default]
    ../code/processArguments.h:1:6: note: expected ‘const char * const*’ but argument is of type ‘char **’

This is not a problem in this case. If you want to know why, you can read about it [here, on StackOverflow](http://stackoverflow.com/questions/12992407/warning-when-passing-non-const-parameter-to-a-function-that-expects-const-parame).

## License
This program is distributed under the terms of the GNU General Public License. A version of this license should have been provided with the program. It can also be found on: [http://www.gnu.org/licenses/gpl-3.0.txt](http://www.gnu.org/licenses/gpl-3.0.txt)
