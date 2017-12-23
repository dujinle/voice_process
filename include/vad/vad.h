#ifndef SNR_VAD_HEADER
#define SNR_VAD_HEADER
#include "config.h"
/*
 * 基于信噪比的vad端点检测
 * 音频数据 8K 16Bit
*/

typedef struct vad_inst{
	short frame_size;		//记录帧长 32ms默认
	short frame_move;		//记录帧移 距离默认10ms
	short frame_base;		//基于开始的参数
	double fac;				//能量阈值
	double ffac;			//能量阈值2
	double energy;			//一帧的能量
	short voice;			//记录当前帧是否活跃
	short findx;			//记录当前是第几帧
	short speech;			//记录状态延续

} vad_stc;

/* 创建一个vad对象句柄 参数必须 */
vad_stc* vad_creat(config_stc* conf);

/* 初始化 vad对象 包括参数 */
void vad_reset(vad_stc* vad_handle);

/* vad操作 数据为一帧 音频数据 0:sil 1:speech */
int vad_process(vad_stc* vad_handle,double* data);
#endif
