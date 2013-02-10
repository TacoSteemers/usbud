usbud
=====

## USB Storage Back Up Daemon
The daemon will back up usb storage devices the moment they are plugged in.
usbud depends on the df program.
usbud is a work in progress, and not yet functional.
Log output can be found in syslog.
Project is known to work on GNU/Linux systems. Because right know the functionality depends on the output of the 'df' command, and that output could theoretically differ between GNU/Linux systems, it may not work on yours at this moment in time.

## To be implemented:
- Keeping state past each detection run
- The actual backing up
- Configuration options for storage device size limits
- Configuration options for target directory size limits
- Determining the difference between a 'thumb drive' and external storage hard disks
- Configuration options for ignoring external storage hard disks or 'thumb drives'
- Remove dependence on df

## Note on building
When building the project with the attached Makefile, a warning should show, like the following:

    ../code/daemon.c: In function ‘main’:
    ../code/daemon.c:21:2: warning: passing argument 1 of ‘processArguments’ from incompatible pointer type [enabled by default]
    ../code/processArguments.h:1:6: note: expected ‘const char * const*’ but argument is of type ‘char **’

This is not a problem in this case. If you want to know why, you can read about it [here, on StackOverflow](http://stackoverflow.com/questions/12992407/warning-when-passing-non-const-parameter-to-a-function-that-expects-const-parame).
