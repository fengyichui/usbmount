========
usbmount
========

A fork of usbmount from https://launchpad.net/ubuntu/+source/usbmount/, solving that can't mount ``fuse`` file system via `systemd.service`_.


The original ``README`` file is `there <old/README>`_ and the ``copyright`` file is `there <old/debian/copyright>`_.

Thanks very much.
 
Usage
-----

1. **Mount**

  When plug in a usb device, it is mounted at ``/mnt/usb/$LABEL`` by default and the ``$LABEL`` is the label of usb. You can modify the configure file at ``/etc/usbmount/usbmount.conf``, more detail please see `it <usbmount.conf>`_.

2. **Eject**

  There is a small tool ``usbeject`` to help us eject usb devices, more detail please see ``usbeject -h``.
  
.. _`systemd.service`: usbmount@.service
