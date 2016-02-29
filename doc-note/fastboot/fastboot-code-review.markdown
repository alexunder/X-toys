---
layout: post
title:  fastboot source code analysis
category: code
---

# Overview #

Fastboot is a command line program which runs on host side, whose function is mainly handling communications between the host desktop machine and the android device via USB interface. In this document, I will describe the main architecture and some details. Its source code is located in 

#USB layer #

#Start from USBtest#
I will start with the test program called "usbtest". Because the usbtest program is pretty small to analyze, otherwise the usb function is the most fundamental layer in the enire fastboot source code, therefore we could focus on USB functional details regardless of the whole software framework.  With the help of Graphviz and GCC instrument function, I can get the calling toutine graph under the case of running. When I run the command like below:

```bash
usbtest list
```

Then I  got the calling graph like below:

![usbtest.png](usbtest.png  "usbtest.png")

It's very simple, isn't it? Inside the main function, it just call usb_open:

```c
usb_handle *usb_open(ifc_match_func callback)
{
     return find_usb_device("/dev/bus/usb", callback);
}
```

After calling this function, it will find the most possible USB device attatched with the host machine, commonly, it is usually the android deivice which we really want. Then the function return the strucure usb_handle:

```c
struct usb_handle
{
    char fname[64];
    int desc;
    unsigned char ep_in;
    unsigned char ep_out;
};
```

Let's step into the "find_usb_device" function:

```c
static usb_handle *find_usb_device(const char *base, ifc_match_func callback)
{
    usb_handle *usb = 0;
    char busname[64], devname[64];
    char desc[1024];
    int n, in, out, ifc;

    DIR *busdir, *devdir;
    struct dirent *de;
    int fd;
    int writable;

    busdir = opendir(base);
    if(busdir == 0) return 0;

    while((de = readdir(busdir)) && (usb == 0)) {
        if(badname(de->d_name)) continue;

        sprintf(busname, "%s/%s", base, de->d_name);
        devdir = opendir(busname);
        if(devdir == 0) continue;

//        DBG("[ scanning %s ]\n", busname);
        while((de = readdir(devdir)) && (usb == 0)) {

            if(badname(de->d_name)) continue;
            sprintf(devname, "%s/%s", busname, de->d_name);

//            DBG("[ scanning %s ]\n", devname);
            writable = 1;
            if((fd = open(devname, O_RDWR)) < 0) {
                // Check if we have read-only access, so we can give a helpful
                // diagnostic like "adb devices" does.
                writable = 0;
                if((fd = open(devname, O_RDONLY)) < 0) {
                    continue;
                }
            }

            n = read(fd, desc, sizeof(desc));

            if(filter_usb_device(fd, desc, n, writable, callback,
                                 &in, &out, &ifc) == 0) {
                usb = calloc(1, sizeof(usb_handle));
                strcpy(usb->fname, devname);
                usb->ep_in = in;
                usb->ep_out = out;
                usb->desc = fd;

                n = ioctl(fd, USBDEVFS_CLAIMINTERFACE, &ifc);
                if(n != 0) {
                    close(fd);
                    free(usb);
                    usb = 0;
                    continue;
                }
            } else {
                close(fd);
            }
        }
        closedir(devdir);
    }
    closedir(busdir);

    return usb;
}
```

This function traverse all directories under the directory "/sys/bus/usb/devices", but only the directories whose names are made up pf numbers , dots and dashes, just like these:

```bash
1-0:1.0  1-1  1-1.1  1-1:1.0  1-1.1:1.0  1-1.2  1-1.2:1.0 
```

Then we get the dev names from previous files we just traversed via the calling to convert_to_devfs_name:

```c
static int convert_to_devfs_name(const char* sysfs_name,
                                 char* devname, int devname_size)
{
    int busnum, devnum;

    busnum = read_sysfs_number(sysfs_name, "busnum");
    if (busnum < 0)
        return -1;

    devnum = read_sysfs_number(sysfs_name, "devnum");
    if (devnum < 0)
        return -1;

    snprintf(devname, devname_size, "/dev/bus/usb/%03d/%03d", busnum, devnum);
    return 0;
}
```

There are many information nodes ir files under the path that we just talked about, one path represent one device, For example, The files below is under the path of "/sys/bus/usb/devices/1-1":

```bash
drwxr-xr-x  5 root root     0 Feb 29 11:06 1-1.1
drwxr-xr-x 10 root root     0 Feb 29 10:49 1-1:1.0
drwxr-xr-x  5 root root     0 Feb 29 10:49 1-1.2
-rw-r--r--  1 root root  4096 Feb 29 10:49 authorized
-rw-r--r--  1 root root  4096 Feb 29 10:49 avoid_reset_quirk
-r--r--r--  1 root root  4096 Feb 29 10:49 bcdDevice
-rw-r--r--  1 root root  4096 Feb 29 10:49 bConfigurationValue
-r--r--r--  1 root root  4096 Feb 29 10:49 bDeviceClass
-r--r--r--  1 root root  4096 Feb 29 10:49 bDeviceProtocol
-r--r--r--  1 root root  4096 Feb 29 10:49 bDeviceSubClass
-r--r--r--  1 root root  4096 Feb 29 10:49 bmAttributes
-r--r--r--  1 root root  4096 Feb 29 10:49 bMaxPacketSize0
-r--r--r--  1 root root  4096 Feb 29 10:49 bMaxPower
-r--r--r--  1 root root  4096 Feb 29 10:49 bNumConfigurations
-r--r--r--  1 root root  4096 Feb 29 10:49 bNumInterfaces
-r--r--r--  1 root root  4096 Feb 29 10:49 busnum
-r--r--r--  1 root root  4096 Feb 29 10:49 configuration
-r--r--r--  1 root root 65553 Feb 29 10:49 descriptors
-r--r--r--  1 root root  4096 Feb 29 10:49 dev
-r--r--r--  1 root root  4096 Feb 29 10:49 devnum
-r--r--r--  1 root root  4096 Feb 29 10:49 devpath
lrwxrwxrwx  1 root root     0 Feb 29 10:49 driver -> ../../../../../bus/usb/drivers/usb
drwxr-xr-x  3 root root     0 Feb 29 10:49 ep_00
-r--r--r--  1 root root  4096 Feb 29 10:49 idProduct
-r--r--r--  1 root root  4096 Feb 29 10:49 idVendor
-r--r--r--  1 root root  4096 Feb 29 10:49 ltm_capable
-r--r--r--  1 root root  4096 Feb 29 10:49 maxchild
lrwxrwxrwx  1 root root     0 Feb 29 10:49 port -> ../1-0:1.0/usb1-port1
drwxr-xr-x  2 root root     0 Feb 29 10:49 power
-r--r--r--  1 root root  4096 Feb 29 10:49 quirks
-r--r--r--  1 root root  4096 Feb 29 10:49 removable
--w-------  1 root root  4096 Feb 29 10:49 remove
-r--r--r--  1 root root  4096 Feb 29 10:49 speed
lrwxrwxrwx  1 root root     0 Feb 29 10:49 subsystem -> ../../../../../bus/usb
-rw-r--r--  1 root root  4096 Feb 29 10:49 uevent
-r--r--r--  1 root root  4096 Feb 29 10:49 urbnum
-r--r--r--  1 root root  4096 Feb 29 10:49 version
``` 

After getting values from busnum and devnumber, the dev path can be got. Finally, after calling "open", the File descriptor can be required, then assign it to usb_handle object, and other informations can be got from calling filter_usb_device.

## The details of USB  ##

Actually I know nothing about the USB protocols, but if I want to analyze the fastboot code, I must do some studies for USB. 
  
