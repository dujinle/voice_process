#ifndef PROCESS_HEADER
#define PROCESS_HEADER
#include "sndfile/sndfile.h"
#include "vad/vad.h"
#define ANDROID_DEBUG_LOG

#ifdef ANDROID_DEBUG_LOG
#include "android_log.h"
#endif

typedef struct wavinfo{
	short size;		//音频数据的大小
	short fs;		//采样率
	short fb;		//采样bit大小16
	short fsize;	//分帧大小256
	short fmove;	//帧移大小80
	short bank_num;	//一帧数据子带的个数 默认32
	short fbank_num;//滤波器的个数 24
	short mfcc_size;//mfcc 参数个数 12
	short frame_num;//帧个数
	short start;	//开始处理的有效帧
	short end;		//结束的有效帧
	short left;	//上次处理的遗留数据size
	int start_append;//vad之后开始增加多少静音数据
	int end_append;	//vad之后结束增加多少静音数据
	double** mfccs;	//返回的结果
	double* mass;	//质心参数存储
	double* rms;	//均方根参数存储
	short* ldata;	//上次处理的遗留数据
	short* start_data;	//增加的静音内存
	vad_stc* vad;//vad对象
	SNDFILE* sf;		//读取文件的句柄
}wav_info;

/* 处理被测试音频数据 */
void process_feat(wav_info* winfo,double* data);
double* compare_rec(wav_info* w1,wav_info* w2);

wav_info* init_winfo(int size,int fs,int fsize,int fmove);
wav_info* read_handler(char* filename);
wav_info* creat_wchar_writer(char* filename,int fs,int bits,int channels);
int write_cdata(wav_info* winfo,short* data,int lens,int flg);
int read_double_wav(wav_info* winfo,double* data,int size);
int read_short_wav(wav_info* winfo,short* data,int size);
void close_fd(wav_info* winfo);
#endif
