usbud
=====

## USB Storage Back Up Daemon
The daemon will back up USB storage devices the moment they are plugged in.
usbud is a work in progress, and not yet functional.
The daemon supports blacklisting and whitelisting. Either list consists of a file that contains one device id on each line. One can use the 'uslist' executable to find the device ids.
Log output can be found in syslog.
This project is known to work on GNU/Linux systems. usbud depends on the availability of items such as "/sys/block" and "/proc/mounts".

## Instructions on running the daemon
Instructions will be made available when the project has reached it's minimal functionality requirements. Backing up the storage device contents is the last such requirement.

## To be implemented:
- The actual backing up
- Enable better support for 'all-in-one' card readers
- Configuration options for storage device size limits
Low priotity:
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
