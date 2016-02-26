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

