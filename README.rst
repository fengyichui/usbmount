========
usbmount
========

A fork of usbmount from https://launchpad.net/ubuntu/+source/usbmount/, solving it can't mount the ``fuse`` file system via `systemd.service`_.


There are the original `README <old/README>`_ file and the `copyright <old/debian/copyright>`_ file.

Thanks very much.

Usage
-----

1. **Mount**

When plug in a usb device, it is mounted at ``/mnt/usb/$LABEL`` by default and the value ``$LABEL`` is the label of usb. You can modify the configure file at ``/etc/usbmount/usbmount.conf``, more detail please see `it <usbmount.conf>`_.

2. **Eject**

There is a small tool ``usbeject`` to help us eject usb devices, more detail please see ``usbeject -h``.

.. _`systemd.service`: usbmount@.service.in

Dependences
-----------

1. ``blkid`` and ``lsblk``
2. ``mount`` and ``umount``

The above all are offered by `util-linux <https://en.wikipedia.org/wiki/Util-linux>`_.

3. [OPT] ``run-parts``, https://packages.qa.debian.org/d/debianutils.html

