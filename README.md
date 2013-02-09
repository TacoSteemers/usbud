usbud
=====

USB Storage Back Up Daemon
The daemon will back up usb storage devices when they are plugged in.
usbud depends on the df program.
usbud is a work in progress, and not yet functional.
Log output can be found in syslog.

## To be implemented:
- Keeping state past each detection run
- The actual backing up
- Configuration options for the target directory
- Configuration options for storage device size limits
- Configuration options for target directory size limits
- Determining the difference between a 'thumb drive' and external storage hard disks
- Configuration options for ignoring external storage hard disks or 'thumb drives'
- Remove dependence on df
