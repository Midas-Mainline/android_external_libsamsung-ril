# This file is part of Samsung-RIL.
#
# Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
# Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
#
# Samsung-RIL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Samsung-RIL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Samsung-RIL.  If not, see <http://www.gnu.org/licenses/>.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	samsung-ril.c \
	client.c \
	ipc.c \
	srs.c \
	utils.c \
	power.c \
	call.c \
	sms.c \
	sim.c \
	network.c \
	sound.c \
	misc.c \
	ss.c \
	oem.c \
	data.c \
	rfs.c \
	gen.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/include \
	external/libsamsung-ipc/include \
	hardware/libhardware_legacy/include \
	system/core/include

LOCAL_CFLAGS := -DRIL_SHLIB

LOCAL_SHARED_LIBRARIES := libcutils libnetutils libutils liblog libpower libcrypto
LOCAL_STATIC_LIBRARIES := libsamsung-ipc

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libsamsung-ril

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := srs-client/srs-client.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/srs-client/include \

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libsrs-client

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := tools/srs-test.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/srs-client/include \

LOCAL_SHARED_LIBRARIES := liblog libcutils libsrs-client

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := srs-test

include $(BUILD_EXECUTABLE)
