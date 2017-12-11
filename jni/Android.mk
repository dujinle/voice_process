#==================== compile the maxent core =============

LOCAL_PATH := $(call my-dir)
$(info [${APP_ABI}] cur dir:${LOCAL_PATH})

P_INCLUDE_PATH := ${LOCAL_PATH}/../include/
P_LIB_PATH     := ${LOCAL_PATH}/../lib/android/

$(info [${APP_ABI}] NDK_ROOT dir ${NDK_ROOT})

include $(CLEAR_VARS)
$(info [${APP_ABI}] include dir:${P_INCLUDE_PATH})
$(info [${APP_ABI}] libs dir:${P_LIB_PATH})
LOCAL_MODULE    := landmarkreg

LOCAL_C_INCLUDES:= $(P_INCLUDE_PATH) \
	${LOCAL_PATH}/../src/

# The order of these libraries is often important.

LOCAL_LDFLAGS += \
	"${P_LIB_PATH}libopencv_imgcodecs.a" \
	"${P_LIB_PATH}libopencv_imgproc.a" \
	"${P_LIB_PATH}libopencv_highgui.a" \
	"${P_LIB_PATH}libopencv_core.a" \
	"${P_LIB_PATH}libopencv_hal.a" \
	"${P_LIB_PATH}liblibwebp.a" \
	"${P_LIB_PATH}liblibpng.a" \
	"${P_LIB_PATH}liblibjasper.a" \
	"${P_LIB_PATH}liblibtiff.a" \
	"${P_LIB_PATH}liblibjpeg.a" \
	"${P_LIB_PATH}libIlmImf.a" \
	"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/thumb/libgnustl_static.a" \
	"$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/thumb/libsupc++.a" \

LOCAL_CPPFLAGS := -Wall -g -fpic \
	-DDEBUG_LOG \

LOCAL_SRC_FILES := $(wildcard ../src/*.cpp) \
	jni.cpp

LOCAL_LDLIBS    := -llog -lm -lz


include $(BUILD_SHARED_LIBRARY)
