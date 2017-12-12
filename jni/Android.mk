#==================== compile the maxent core =============

LOCAL_PATH := $(call my-dir)
$(info [${APP_ABI}] cur dir:${LOCAL_PATH})

P_INCLUDE_PATH := ${LOCAL_PATH}/../include/
P_INCLUDE_PATH += ${LOCAL_PATH}

$(info [${APP_ABI}] NDK_ROOT dir ${NDK_ROOT})

include $(CLEAR_VARS)
$(info [${APP_ABI}] include dir:${P_INCLUDE_PATH})
LOCAL_MODULE    := libprocess

# The order of these libraries is often important.

#LOCAL_LDFLAGS += \
#	"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/thumb/libgnustl_static.a" \
#	"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/thumb/libsupc++.a" \

LOCAL_C_INCLUDES = $(P_INCLUDE_PATH)

LOCAL_CPPFLAGS := -Wall -Wint-to-pointer-cast -g -fpic \
	-DANDROID_DEBUG_LOG \

LOCAL_SRC_FILES := \
	../src/core/cmfcc.c \
	../src/core/compare.c \
	../src/core/fft.c \
	../src/core/mass_center.c \
	../src/core/mel_bank.c \
	../src/core/pre_process.c \
	../src/core/tools.c \
	../src/vad_snr/vad.c \
	../src/process.c \
	jni_VprocessJNI.c

LOCAL_LDLIBS    := -llog -lm -lz

include $(BUILD_SHARED_LIBRARY)
