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

There are many information nodes or files under the path that we just talked about, one path represent one device, For example, The files below is under the path of "/sys/bus/usb/devices/1-1":

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
  
###  Some data structures about USB ###

* usb_device_descriptor

```c
struct usb_device_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;
    __le16 bcdUSB;
    __u8  bDeviceClass;
    __u8  bDeviceSubClass;
    __u8  bDeviceProtocol;
    __u8  bMaxPacketSize0;
    __le16 idVendor;
    __le16 idProduct;
    __le16 bcdDevice;
    __u8  iManufacturer;
    __u8  iProduct;
    __u8  iSerialNumber;
    __u8  bNumConfigurations;
} __attribute__ ((packed));
```

 It describes general information about a USB device. It includes information that applies globally to the device and all of the device's configurations. A USB device has only one device descriptor. The last field of usb_device_descriptor (bNumConfigurations) tells us the number of configurations the current device has.

*  usb_config_descriptor

```c
struct usb_config_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;
    __le16 wTotalLength;
    __u8  bNumInterfaces;
    __u8  bConfigurationValue;
    __u8  iConfiguration;
    __u8  bmAttributes;
    __u8  bMaxPower;
} __attribute__ ((packed));
```

One device may have many configurations, and every configuration may has many interfaces. 

* usb_interface_descriptor

```c
struct usb_interface_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __u8  bInterfaceNumber;
    __u8  bAlternateSetting;
    __u8  bNumEndpoints;
    __u8  bInterfaceClass;
    __u8  bInterfaceSubClass;
    __u8  bInterfaceProtocol;
    __u8  iInterface;
} __attribute__ ((packed));
```

The USB interfaces allow USB devices to support multipile functions. It defines the number of end points. The end points are the real elements which communicate with the host.

* usb_endpoint_descriptor

```c
struct usb_endpoint_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __u8  bEndpointAddress;
    __u8  bmAttributes;
    __le16 wMaxPacketSize;
    __u8  bInterval;

    /* NOTE:  these two are _only_ in audio endpoints. */
    /* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
    __u8  bRefresh;
    __u8  bSynchAddress;
} __attribute__ ((packed));
```

There are two kinds of end points, one direction that the data come from the device, we call it a "IN transfer", one direction that the datas go to the device, we call it a "OUT transfer".  The bit 7 determine the category of the end point.

Even though these four data structures are distinct, the first two fields are same, like:

```c
 struct usb_descriptor_header {
    __u8  bLength;
    __u8  bDescriptorType;
} __attribute__ ((packed));

```

All standard descriptors have these 2 fields at the beginning.

### About filter_usb_device ###

The source code of filter_usb_device is below:

```c
static int filter_usb_device(char* sysfs_name,
                             char *ptr, int len, int writable,
                             ifc_match_func callback,
                             int *ept_in_id, int *ept_out_id, int *ifc_id)
{
    struct usb_device_descriptor *dev;
    struct usb_config_descriptor *cfg;
    struct usb_interface_descriptor *ifc;
    struct usb_endpoint_descriptor *ept;
    struct usb_ifc_info info;

    int in, out;
    unsigned i;
    unsigned e;
    
    if (check(ptr, len, USB_DT_DEVICE, USB_DT_DEVICE_SIZE))
        return -1;
    dev = (struct usb_device_descriptor *)ptr;
    len -= dev->bLength;
    ptr += dev->bLength;

    if (check(ptr, len, USB_DT_CONFIG, USB_DT_CONFIG_SIZE))
        return -1;
    cfg = (struct usb_config_descriptor *)ptr;
    len -= cfg->bLength;
    ptr += cfg->bLength;

    info.dev_vendor = dev->idVendor;
    info.dev_product = dev->idProduct;
    info.dev_class = dev->bDeviceClass;
    info.dev_subclass = dev->bDeviceSubClass;
    info.dev_protocol = dev->bDeviceProtocol;
    info.writable = writable;

    snprintf(info.device_path, sizeof(info.device_path), "usb:%s", sysfs_name);

    /* Read device serial number (if there is one).
     * We read the serial number from sysfs, since it's faster and more
     * reliable than issuing a control pipe read, and also won't
     * cause problems for devices which don't like getting descriptor
     * requests while they're in the middle of flashing.
     */
    info.serial_number[0] = '\0';
    if (dev->iSerialNumber) {
        char path[80];
        int fd;

        snprintf(path, sizeof(path),
                 "/sys/bus/usb/devices/%s/serial", sysfs_name);
        path[sizeof(path) - 1] = '\0';

        fd = open(path, O_RDONLY);
        if (fd >= 0) {
            int chars_read = read(fd, info.serial_number,
                                  sizeof(info.serial_number) - 1);
            close(fd);

            if (chars_read <= 0)
                info.serial_number[0] = '\0';
            else if (info.serial_number[chars_read - 1] == '\n') {
                // strip trailing newline
                info.serial_number[chars_read - 1] = '\0';
            }
        }
    }

    for(i = 0; i < cfg->bNumInterfaces; i++) {

        while (len > 0) {
	        struct usb_descriptor_header *hdr = (struct usb_descriptor_header *)ptr;
            if (check(hdr, len, USB_DT_INTERFACE, USB_DT_INTERFACE_SIZE) == 0)
                break;
            len -= hdr->bLength;
            ptr += hdr->bLength;
        }

        if (len <= 0)
            return -1;

        ifc = (struct usb_interface_descriptor *)ptr;
        len -= ifc->bLength;
        ptr += ifc->bLength;

        in = -1;
        out = -1;
        info.ifc_class = ifc->bInterfaceClass;
        info.ifc_subclass = ifc->bInterfaceSubClass;
        info.ifc_protocol = ifc->bInterfaceProtocol;

        for(e = 0; e < ifc->bNumEndpoints; e++) {
            while (len > 0) {
	            struct usb_descriptor_header *hdr = (struct usb_descriptor_header *)ptr;
                if (check(hdr, len, USB_DT_ENDPOINT, USB_DT_ENDPOINT_SIZE) == 0)
                    break;
                len -= hdr->bLength;
                ptr += hdr->bLength;
            }
            if (len < 0) {
                break;
            }

            ept = (struct usb_endpoint_descriptor *)ptr;
            len -= ept->bLength;
            ptr += ept->bLength;

            if((ept->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) != USB_ENDPOINT_XFER_BULK)
                continue;

            if(ept->bEndpointAddress & USB_ENDPOINT_DIR_MASK) {
                in = ept->bEndpointAddress;
            } else {
                out = ept->bEndpointAddress;
            }

            // For USB 3.0 devices skip the SS Endpoint Companion descriptor
            if (check((struct usb_descriptor_hdr *)ptr, len,
                      USB_DT_SS_ENDPOINT_COMP, USB_DT_SS_EP_COMP_SIZE) == 0) {
                len -= USB_DT_SS_EP_COMP_SIZE;
                ptr += USB_DT_SS_EP_COMP_SIZE;
            }
        }

        info.has_bulk_in = (in != -1);
        info.has_bulk_out = (out != -1);

        if(callback(&info) == 0) {
            *ept_in_id = in;
            *ept_out_id = out;
            *ifc_id = ifc->bInterfaceNumber;
            return 0;
        }
    }

    return -1;
}
```

Firstly,  from the source code above, we can know that there are some data structures mentioned in last section, and they are following the linear layout from usb_device_descriptor, usb_config_descriptor, usb_interface_descriptor, then usb_endpoint_descriptor, like the graph below:

![usb_dev_structure.png](usb_dev_structure.png  "usb_dev_structure.png")

The main routine of filter_usb_device is traversing every interface inside the config(This scenario has only one config), and traversing every end point inside one interface, then using callback to filter out the desired end point, but in usbtest, the callback function don't filter any one, it dumps all end points' information.

### USB I/O interfaces ###

After introduction of  the USB layer internals, let's focus on the interfaces exported to the upper layer(declared in usb.h):

```c
usb_handle *usb_open(ifc_match_func callback);
int usb_close(usb_handle *h);
int usb_read(usb_handle *h, void *_data, int len);
int usb_write(usb_handle *h, const void *_data, int len);
int usb_wait_for_disconnect(usb_handle *h);
```

# The Architecture of fastboot #

Now, we will get the main running routines via functions profiling just as usbtest. I choose the "fastboot oem lock", the calling graph is below:

![fastboot_oem_lock.png](fastboot_oem_lock.png  "fastboot_oem_lock.png")

According to the graph, there are three main parts in the fastboot running routine: 

![fastboot_main_routine.png](fastboot_main_routine.png  "fastboot_main_routine.png")

  


