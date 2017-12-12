#ifndef PROCESS_HEADER
#define PROCESS_HEADER
typedef struct wavinfo{
	short size;		//音频数据的大小
	short fs;		//采样率
	short fsize;	//分帧大小256
	short fmove;	//帧移大小80
	short bank_num;	//一帧数据子带的个数 默认32
	short fbank_num;//滤波器的个数 24
	short mfcc_size;//mfcc 参数个数 12
	short frame_num;//帧个数
	short start;	//开始处理的有效帧
	short end;		//结束的有效帧
	double** mfccs;	//返回的结果
	double* mass;	//质心参数存储
	double* rms;	//均方根参数存储
	double* wdata;	//wav数据的首地址
}wav_info;

/* 处理被测试音频数据 */
void process_feat(wav_info* winfo,double* data);
double* compare_rec(wav_info* w1,wav_info* w2);

wav_info* init_winfo(int size,int fs,int fsize,int fmove);

#endif
