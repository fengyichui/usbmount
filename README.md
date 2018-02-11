# usbmount

A fork of usbmount from https://launchpad.net/ubuntu/+source/usbmount/, solving that can't mount `fuse` file system via [systemd.service](usbmount@.service).

The origin `README` file is [there](old/README) and the `copyright` file is [there](old/debian/copyright).

Thanks very much.

## Usage

1. __Mount__

  when plug in a usb device, it is mounted at `/mnt/usb/$LABEL` by default and the `$LABEL` is the label of usb. you can modify the configure file at `/etc/usbmount/usbmount.conf`, more detail please see [it](usbmount.conf).

2. __Eject__

  there is a script `usbeject` to help us eject usb device, more detail please see `usbeject -h`.
