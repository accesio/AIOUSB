# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES  := -I${AIO_LIB_DIR} /usr/include/libusb-1.0 ${AIO_LIB_DIR}  ${AIO_LIB_DIR}/ndkbuild/libs/${TARGET_ARCH_ABI}
LOCAL_CFLAGS 	+= -std=c99 -L${AIO_LIB_DIR}/ndkbuild/libs/${TARGET_ARCH_ABI}
LOCAL_MODULE    := AIOUSB
LOCAL_SRC_FILES := AIOUSB_wrap.c
LOCAL_LDLIBS 	:= -lusb-1.0 -laiousb
LOCAL_LDFLAGS	:= -L${AIO_LIB_DIR}/ndkbuild/libs/${TARGET_ARCH_ABI}

include $(BUILD_SHARED_LIBRARY)
