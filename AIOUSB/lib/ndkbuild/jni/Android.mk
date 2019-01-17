# File: Android.mk

APP_ABI := armeabi armeabi-v7a x86 mips

LOCAL_PATH := $(call my-dir)
MYLOCAL_DIR	:= $(shell pwd)/jni
include $(PATH_TO_LIBUSB_SRC)/android/jni/libusb.mk

#$(warning "THis is $(LOCAL_PATH)")
#$(warning "LOCALMODFILE new= $(LOCAL_MODULE_FILENAME)")
LOCAL_MODULE    := usb-1.0
LOCAL_MODULE_FILENAME := libusb-1.0
include $(BUILD_SHARED_LIBRARY)
#nclude $(CLEAR_VARS)


#$(warning "MYLOCAL_DIR= $(MYLOCAL_DIR)")
LOCAL_C_INCLUDES  := /usr/include/libusb-1.0
LOCAL_CFLAGS += -std=c99
LOCAL_MODULE    := aiousb
LOCAL_MODULE_FILENAME := libaiousb 
#$(warning "LOCAL_MODULE_FILENAME= $(LOCAL_MODULE_FILENAME)")

LOCAL_SRC_FILES :=  $(MYLOCAL_DIR)/ADCConfigBlock.c \
		    $(MYLOCAL_DIR)/AIOBuf.c \
		    $(MYLOCAL_DIR)/AIOChannelMask.c \
		    $(MYLOCAL_DIR)/AIOChannelRange.c \
		    $(MYLOCAL_DIR)/AIOCmd.c \
		    $(MYLOCAL_DIR)/AIOCommandLine.c \
		    $(MYLOCAL_DIR)/AIOConfiguration.c \
		    $(MYLOCAL_DIR)/AIOContinuousBuffer.c \
		    $(MYLOCAL_DIR)/AIOCountsConverter.c \
		    $(MYLOCAL_DIR)/AIODeviceInfo.c \
		    $(MYLOCAL_DIR)/AIODeviceQuery.c \
		    $(MYLOCAL_DIR)/AIODeviceTable.c \
		    $(MYLOCAL_DIR)/AIOEither.c \
		    $(MYLOCAL_DIR)/AIOFifo.c \
		    $(MYLOCAL_DIR)/AIOList.c \
		    $(MYLOCAL_DIR)/AIOProductTypes.c \
		    $(MYLOCAL_DIR)/AIOTuple.c \
		    $(MYLOCAL_DIR)/AIOUSB_ADC.c \
		    $(MYLOCAL_DIR)/AIOUSB_Core.c \
		    $(MYLOCAL_DIR)/AIOUSB_CTR.c \
		    $(MYLOCAL_DIR)/AIOUSB_CustomEEPROM.c \
		    $(MYLOCAL_DIR)/AIOUSB_DAC.c \
		    $(MYLOCAL_DIR)/AIOUSBDevice.c \
		    $(MYLOCAL_DIR)/AIOUSB_DIO.c \
		    $(MYLOCAL_DIR)/AIOUSB_Log.c \
		    $(MYLOCAL_DIR)/AIOUSB_Properties.c \
		    $(MYLOCAL_DIR)/AIOUSB_USB.c \
		    $(MYLOCAL_DIR)/AIOUSB_WDG.c \
		    $(MYLOCAL_DIR)/cJSON.c \
		    $(MYLOCAL_DIR)/CStringArray.c \
		    $(MYLOCAL_DIR)/DIOBuf.c \
		    $(MYLOCAL_DIR)/USBDevice.c \

LOCAL_STATIC_LIBRARIES := usb-1.0
LOCAL_C_INCLUDES	+= $(LIBUSB_ROOT_ABS)
include $(BUILD_SHARED_LIBRARY)

#
# debug version
#
#
include $(CLEAR_VARS)
#$(warning "MYLOCAL_DIR= $(MYLOCAL_DIR)")
LOCAL_C_INCLUDES	:= /usr/include/libusb-1.0
LOCAL_CFLAGS 		+= -std=c99 -g
LOCAL_MODULE    	:= aiousbdbg
LOCAL_MODULE_FILENAME 	:= libaiousbdbg 

LOCAL_SRC_FILES :=  $(MYLOCAL_DIR)/ADCConfigBlock.c \
		    $(MYLOCAL_DIR)/AIOBuf.c \
		    $(MYLOCAL_DIR)/AIOChannelMask.c \
		    $(MYLOCAL_DIR)/AIOChannelRange.c \
		    $(MYLOCAL_DIR)/AIOCmd.c \
		    $(MYLOCAL_DIR)/AIOCommandLine.c \
		    $(MYLOCAL_DIR)/AIOConfiguration.c \
		    $(MYLOCAL_DIR)/AIOContinuousBuffer.c \
		    $(MYLOCAL_DIR)/AIOCountsConverter.c \
		    $(MYLOCAL_DIR)/AIODeviceInfo.c \
		    $(MYLOCAL_DIR)/AIODeviceQuery.c \
		    $(MYLOCAL_DIR)/AIODeviceTable.c \
		    $(MYLOCAL_DIR)/AIOEither.c \
		    $(MYLOCAL_DIR)/AIOFifo.c \
		    $(MYLOCAL_DIR)/AIOList.c \
		    $(MYLOCAL_DIR)/AIOProductTypes.c \
		    $(MYLOCAL_DIR)/AIOTuple.c \
		    $(MYLOCAL_DIR)/AIOUSB_ADC.c \
		    $(MYLOCAL_DIR)/AIOUSB_Core.c \
		    $(MYLOCAL_DIR)/AIOUSB_CTR.c \
		    $(MYLOCAL_DIR)/AIOUSB_CustomEEPROM.c \
		    $(MYLOCAL_DIR)/AIOUSB_DAC.c \
		    $(MYLOCAL_DIR)/AIOUSBDevice.c \
		    $(MYLOCAL_DIR)/AIOUSB_DIO.c \
		    $(MYLOCAL_DIR)/AIOUSB_Log.c \
		    $(MYLOCAL_DIR)/AIOUSB_Properties.c \
		    $(MYLOCAL_DIR)/AIOUSB_USB.c \
		    $(MYLOCAL_DIR)/AIOUSB_WDG.c \
		    $(MYLOCAL_DIR)/cJSON.c \
		    $(MYLOCAL_DIR)/CStringArray.c \
		    $(MYLOCAL_DIR)/DIOBuf.c \
		    $(MYLOCAL_DIR)/USBDevice.c \

LOCAL_STATIC_LIBRARIES := usb-1.0
LOCAL_C_INCLUDES	+= $(LIBUSB_ROOT_ABS)
include $(BUILD_SHARED_LIBRARY)



