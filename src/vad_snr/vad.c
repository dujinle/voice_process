#include "vad_snr/vad.h"
#include <stdlib.h>
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
static double sump_array(double *data,int offset,int len){
	double sum = 0.0;
	int i = 0;
	for(i = 0;i < len;i++){
		sum += data[(offset + i)] * data[(offset + i)];
	}
	return sum;
}

/* 创建一个vad对象句柄 参数必须 */
vad_stc* vad_creat(short sample_rate,short win_size,short win_inc){

	vad_stc* vad_handle = calloc(sizeof(vad_stc),1);
	if(vad_handle == NULL){
		puts("calloc vad_stc handle failed");
		return NULL;
	}
	vad_handle->sample_rate = sample_rate;
	vad_handle->win_len = win_size;
	vad_handle->win_inc = win_inc;
	vad_handle->es_thr = 1.5;
	vad_handle->en_thr = 0.5;
	vad_handle->ave = 0.0;
	vad_handle->track = 0.0;
	vad_handle->findx = -1;

	return vad_handle;
}

/* 初始化 vad对象 包括参数 */
void vad_init(vad_stc* vad_handle,double es_thr,double en_thr){

	vad_handle->es_thr = es_thr;
	vad_handle->en_thr = en_thr;
}

/* 释放 vad对象句柄*/
void vad_free(vad_stc* vad_handle){
	vad_handle = NULL;
}

static void record_flg(vad_stc* vad_handle,short flg){
	/* 说明已经存储到最后一个 需要把 末尾位置 空出来 存现在的值 */
	if(vad_handle->hist_idx == 4){
		int i = 0;
		for(i = 0;i < vad_handle->hist_idx;i++){
			vad_handle->hist_tag[i] = vad_handle->hist_tag[i + 1];
		}
	}
	vad_handle->hist_tag[vad_handle->hist_idx] = flg;
	vad_handle->hist_idx = vad_handle->hist_idx + 1;
	if(vad_handle->hist_idx >= 4){
		vad_handle->hist_idx = 4;
	}
}

/* vad操作 数据为一帧 音频数据 0:sil 1:start 2:end */
int vad_process(vad_stc* vad_handle,double* data,int offset,int len){
	int voice = 0;
	vad_handle->findx++;

	double fsum = sump_array(data,offset,len);
	double snr = log10(fsum/len) * 10;

	if(vad_handle->findx == 0){
		vad_handle->ave = snr;
		vad_handle->track = snr;
	}
	/* 记录 */
	if(vad_handle->findx <= 1){
		record_flg(vad_handle,0);
		return voice;
	}
	/*更新轨迹信息*/
	vad_handle->track = vad_handle->track * 0.95 + snr * 0.05;
	if(vad_handle->track > vad_handle->ave + vad_handle->es_thr){
		voice = 1;
	}else if(vad_handle->track < vad_handle->ave + vad_handle->en_thr){
		voice = 0;
	}else{
		voice = vad_handle->hist_tag[vad_handle->hist_idx];
	}
	/*更新阈值信息*/
	vad_handle->ave = vad_handle->ave * 0.95 + vad_handle->track * 0.05;

	record_flg(vad_handle,voice);

	return voice;
}
