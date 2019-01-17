#ifndef AIOUSB_USB_H
#define AIOUSB_USB_H



#ifdef ALT_USB

#else 

#define usb_control_xfer    libusb_control_transfer
#define usb_bulk_xfer       libusb_bulk_transfer
#define usb_open            libusb_open
#define usb_close           libusb_close
#define usb_free_devices    libusb_free_device_list
#define usb_get_devices     libusb_get_device_list

#endif

#endif
