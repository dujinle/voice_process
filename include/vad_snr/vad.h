#ifndef SNR_VAD_HEADER
#define SNR_VAD_HEADER
/*
 * 基于信噪比的vad端点检测
 * 音频数据 8K 16Bit
*/

typedef struct vad_inst{
	short sample_rate;	//记录数据的采样率
	short win_len;		//记录帧长 32ms默认
	short win_inc;		//记录帧移 距离默认10ms
	short findx;		//记录当前处理的是第几帧
	double es_thr;		//语音开始的阈值 1.5默认
	double en_thr;		//语音结束的阈值 0.5默认
	double track;		//语音DB的信号轨迹值 默认会比真实值 延后
	double ave;			//开始的信噪比值 以一帧的平均能量
	short hist_tag[5];	//记录前面五个帧的 检测结果
	short hist_idx;		//记录当前是历史的第几个帧


} vad_stc;

/* 创建一个vad对象句柄 参数必须 */
vad_stc* vad_creat(short sample_rate,short win_len,short win_inc);

/* 初始化 vad对象 包括参数 */
void vad_init(vad_stc* vad_handle,double es_thr,double en_thr);

/* 释放 vad对象句柄*/
void vad_free(vad_stc* vad_handle);

/* vad操作 数据为一帧 音频数据 0:sil 1:start 2:end */
int vad_process(vad_stc* vad_handle,double* data,int offset,int len);
#endif
