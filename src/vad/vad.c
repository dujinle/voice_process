#include "vad/vad.h"
#include <stdlib.h>

#ifndef ANDROID_DEBUG_LOG
#define ANDROID_DEBUG_LOG
#endif
#ifdef ANDROID_DEBUG_LOG
#include "android_log.h"
#endif
/*
 * 基于信噪比的vad端点检测
 * 音频数据 8K 16Bit
*/

/* 计算一个数组的和 */
static double sum_array(double *data,int offset,int len){
	double sum = 0.0;
	int i = 0;
	for(i = 0;i < len;i++){
		sum += data[(offset + i)];
	}
	return sum;
}

/* 计算一个数组平方和 */
static double sump_array(double *data,int len){
	double sum = 0.0;
	int i = 0;
	for(i = 0;i < len;i++){
		sum += data[i] * data[i];
	}
	return sum;
}

/* 创建一个vad对象句柄 参数必须 */
vad_stc* vad_creat(config_stc* conf){

	vad_stc* vad_handle = calloc(sizeof(vad_stc),1);
	if(vad_handle == NULL){
		return NULL;
	}
	vad_handle->frame_size = conf->frame_size;
	vad_handle->frame_move = conf->frame_move;
	vad_handle->frame_base = conf->vad_base;
	vad_handle->fac = 0;
	vad_handle->ffac = conf->vad_fac;
	vad_handle->energy = 0;
	vad_handle->voice = 0;
	vad_handle->speech = 0;
	vad_handle->findx = 0;
#ifdef ANDROID_DEBUG_LOG
	LOGI("frame_size:%d\n",vad_handle->frame_size);
	LOGI("frame_move:%d\n",vad_handle->frame_move);
	LOGI("findx:%d\n",vad_handle->findx);
#endif

	return vad_handle;
}

void vad_reset(vad_stc* vad_handle){
	vad_handle->fac = 0;
	vad_handle->ffac = 0;
	vad_handle->energy = 0.0;
	vad_handle->voice = 0;
	vad_handle->speech = 0;
	vad_handle->findx = 0;
}

/* vad操作 数据为一帧 音频数据 0:sil 1:start 2:end */
int vad_process(vad_stc* vad_handle,double* data){
	int voice = 0;
	vad_handle->findx++;

	double fsum = sump_array(data,vad_handle->frame_size);

	if(vad_handle->findx < vad_handle->frame_base){
		vad_handle->fac += fsum;
		return vad_handle->voice;
	}
	if(vad_handle->findx == vad_handle->frame_base){
		vad_handle->fac = vad_handle->fac / ((double)(vad_handle->frame_base));
		vad_handle->ffac = vad_handle->fac * vad_handle->ffac;
	}
#ifdef ANDROID_DEBUG_LOG
	LOGI("idx:%d energy:%f fac:%f ffac:%f\n",vad_handle->findx,fsum,vad_handle->fac,vad_handle->ffac);
#endif

	if(vad_handle->voice == 0 && fsum >= vad_handle->fac){
		vad_handle->voice = 1;
		return 0;
	}
	if(vad_handle->voice == 1 && fsum >= vad_handle->ffac){
		vad_handle->voice = 2;
		vad_handle->speech = 1;
		return 1;
	}
	/*
	if(vad_handle->voice == 2 && fsum < vad_handle->ffac){
		vad_handle->voice = 1;
		return 1;
	}
	if(vad_handle->voice == 1 && fsum < vad_handle->fac){
		vad_handle->speech = 0;
		vad_handle->voice = 0;
		return 0;
	}
	*/
	if(vad_handle->voice > 0){
		return vad_handle->speech;
	}

	return voice;
}
