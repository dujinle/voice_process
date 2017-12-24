#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"vad/vad.h"
#include	"core/pre_process.h"
#include	"core/fft.h"
#include	"core/tools.h"
#include	"core/mel_bank.h"
#include	"core/cmfcc.h"
#include	"core/mass_center.h"
#include	"process.h"
#include	"config.h"
#ifdef ANDROID_DEBUG_LOG
#include	"android_log.h"
#endif

wav_info* creat_winfo(char* conf_file){

	config_stc* conf = calloc(sizeof(config_stc),1);

	int ret = paser_config(conf_file,conf);
	if(ret == -1){
		return NULL;
	}

	wav_info* winfo = calloc(sizeof(wav_info),1);
	//是否开启vad过滤音频数据
	if(conf->vad == 1){
		winfo->vad = vad_creat(conf);
#ifdef ANDROID_DEBUG_LOG
		LOGI("open vad filter wav......");
#endif
	}else{
		winfo->vad = NULL;
	}
	winfo->size = 0;
	winfo->fs = conf->sample_rate;
	winfo->fb = conf->sample_bit;
	winfo->fsize = conf->frame_size;
	winfo->fmove = conf->frame_move;
	winfo->bank_num = conf->bank_num;
	winfo->fbank_num = conf->fbank_num;
	winfo->mfcc_size = conf->mfcc_size;
	winfo->start_append = conf->vad_start_append;
	winfo->end_append = conf->vad_end_append;
	winfo->left = 0;
	winfo->start = -1;
	winfo->end = -1;

	return winfo;
}
/* 处理被测试音频数据 */
void process_feat(wav_info* winfo,double* data){

	int i = 0;//mfcc参数个数12
	int fbank_size = winfo->fsize /2 + 1;//滤波器的参数个数

	//vad_stc* vad = vad_creat(winfo->fs,winfo->fsize,winfo->fmove);

	//printf("pre_emphasise......\n");
	pre_emphasise(data,0,winfo->size,data,0.9375);

	//printf("enframe......\n");
	int* frames = enframe(winfo->size,winfo->fsize,winfo->fmove);
	winfo->frame_num = *frames++;

	double **coeff = calloc_mat(winfo->fbank_num,fbank_size);
	complex* complexs = calloc(sizeof(complex),winfo->fsize);
	double* mfccs = (double*)calloc(sizeof(double),12);

	double* fdata = (double*)calloc(sizeof(double),winfo->fsize);

	double** mfcc_mat = calloc_mat(winfo->frame_num,winfo->mfcc_size);
	double* mass_arr = calloc(sizeof(double),winfo->frame_num);
	double* rms_arr = calloc(sizeof(double),winfo->frame_num);
	for(i = 0;i < winfo->frame_num;i++){
		memcpy(fdata,data + frames[i],winfo->fsize * sizeof(double));

		//通过vad操作 确认当前帧是否是语音帧的开始位置
		//int voice = vad_process(vad,fdata,0,winfo->fsize);

		//计算质心 和 均方差
		mass_arr[i] = mass_center(winfo->fsize,fdata,winfo->bank_num);
		rms_arr[i] = rms(winfo->fsize,fdata,winfo->bank_num);
#ifdef ANDROID_DEBUG_LOG
		LOGI("mcenter:%d %4.10f %4.10f\n",i,mass_arr[i],rms_arr[i]);
		//printf("mcenter:%4.10f %4.10f\n",mass_arr[i],rms_arr[i]);
#endif

		//数据加窗处理 为后面的特征提取做准备
		hamming(fdata,winfo->fsize);

		//进行fft变换
		convert_complex(fdata,winfo->fsize,complexs);
		//对数据实值化 消除 虚数部分
		abs_complex(complexs,winfo->fsize,fdata);

		square_array(winfo->fsize,fdata);
		//构造melbank系数
		mel_fbankm(winfo->fbank_num,winfo->fsize,winfo->fs,0, 0.5,coeff);
#ifdef ANDROID_DEBUG_LOG
		LOGI("mel_bank frame[%d] success......\n",i);
#endif
		mfcc(winfo->fbank_num,fbank_size,coeff,fdata,fbank_size,winfo->mfcc_size,mfccs);
		memcpy(mfcc_mat[i],mfccs,sizeof(double) * winfo->mfcc_size);
#ifdef ANDROID_DEBUG_LOG
		LOGI("mfcc frame[%d] success......\n",i);
#endif
	}
	winfo->mfccs = mfcc_mat;
	winfo->mass = mass_arr;
	winfo->rms = rms_arr;
}

double* compare_rec(wav_info* w1,wav_info* w2){

	int i = 0;
	double* min_dist = calloc(sizeof(double),3);
	if(w2->frame_num >= w1->frame_num){
		for(i = 0;i < w2->frame_num - w1->frame_num + 1;i++){
			double mfcc_disp = mfcc_comp(w1->frame_num,w1->mfcc_size,w1->mfccs,w2->mfccs);
			double massc_disp = massc_comp(w1->frame_num,w1->mass,w2->mass);
			double rms_disp = massc_comp(w1->frame_num,w1->rms,w2->rms);
			if(i == 0){
				min_dist[0] = abs(mfcc_disp);
				min_dist[1] = abs(massc_disp);
				min_dist[2] = abs(rms_disp);
			}else{
				if(min_dist[0] > abs(mfcc_disp)){
					min_dist[0] = abs(mfcc_disp);
					if(min_dist[1] > abs(massc_disp)){
						min_dist[1] = abs(massc_disp);
					}
					if(min_dist[2] > abs(rms_disp)){
						min_dist[2] = abs(rms_disp);
					}
				}
			}
			w2->mass++;
			w2->rms++;
			w2->mfccs++;
#ifdef ANDROID_DEBUG_LOG
			LOGI("%d mfcc:%4.10f mass:%4.10f rms:%4.10f\n",i,mfcc_disp,massc_disp,rms_disp);
#endif
			//break
		}
	}
	return min_dist;
}

int set_reader(wav_info* winfo,char* filename){

	printf("start set reader ......\n");
	SF_INFO sfinfo;
	memset (&sfinfo, 0, sizeof (SF_INFO));
	SNDFILE* sf = sf_open (filename, SFM_READ, &sfinfo);
	if(sf == NULL){
		printf ("Error opening %s.\n", filename) ;
		return -1;
	}
	winfo->sf = sf;
	if(winfo->fs != sfinfo.samplerate){
		winfo->fs = sfinfo.samplerate;
	}
	sf_count_t samples = sfinfo.frames / sfinfo.channels;
	winfo->size = samples;
	winfo->left = 0;
	winfo->start = -1;
	winfo->end = -1;
	winfo->ldata = NULL;
	winfo->start_data = NULL;
	winfo->mfccs = NULL;
	winfo->mass = NULL;
	winfo->rms = NULL;
	winfo->frame_num = 0;
	return winfo->size;
}

int read_double_wav(wav_info* winfo,double* data,int size){
	int nread = sf_readf_double(winfo->sf,data,size);
	return nread;
}

int read_short_wav(wav_info* winfo,short*data,int size){
	int nread = sf_readf_short(winfo->sf,data,size);
	return nread;
}

int set_writer(wav_info* winfo,char* filename){

	SF_INFO sfinfo ;
	sfinfo.samplerate = winfo->fs;
	sfinfo.frames = 0; /* Wrong length. Library should correct this on sf_close. */
	sfinfo.channels = 1;
	if(winfo->fb == 16){
		sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	}else{
		sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_U8;
	}
	SNDFILE* sf = sf_open (filename, SFM_WRITE, &sfinfo);
	if(sf == NULL){
		return -1;
	}
	/*如果获取writer 并且启动了vad则开始分配 内存存储静音数据*/
	if(winfo->vad != NULL){
		winfo->start_data = calloc(sizeof(short),winfo->start_append * winfo->fsize);
		memset(winfo->start_data,0,winfo->start_append * winfo->fsize * sizeof(short));
		vad_reset(winfo->vad);
	}
	winfo->sf = sf;
	winfo->left = 0;
	winfo->start = -1;
	winfo->end = -1;
	winfo->ldata = NULL;
	winfo->start_data = NULL;
	winfo->mfccs = NULL;
	winfo->mass = NULL;
	winfo->rms = NULL;
	winfo->frame_num = 0;
	return 0;
}

int write_cdata(wav_info* winfo,short* sdata,int lens,int flg){
	int i = 0,step = 0;
	short* data = calloc(sizeof(short),lens + winfo->left);
	if(winfo->left > 0){
		memcpy(data,winfo->ldata,sizeof(short) * winfo->left);
		memcpy(data + winfo->left,sdata,sizeof(short) * lens);
		lens = lens + winfo->left;
		winfo->left = 0;
	}else{
		memcpy(data,sdata,sizeof(short) * lens);
	}
	if(winfo->vad != NULL){
		int* frames = enframe(lens,winfo->fsize,winfo->fmove);
		short st[winfo->fsize];
		double dd[winfo->fsize];
		for(i = 1;i< frames[0];i++){
			step = frames[i] + winfo->fsize;
			memcpy(st,data + frames[i],sizeof(short)*winfo->fsize);
			les2d_array(st,winfo->fsize,dd);
			int voice = vad_process(winfo->vad,dd);
			if(voice == 0){
#ifdef ANDROID_DEBUG_LOG
				LOGI("vad filter data size:%d",frames[i]);
#endif
				printf("vad filter data size:%d\n",frames[i]);
				continue;
			}
			if(winfo->start == -1){
				winfo->start = i;
				sf_write_short (winfo->sf,winfo->start_data, winfo->start_append * winfo->fsize);
			}
			sf_count_t count = sf_write_short (winfo->sf, data + frames[i], winfo->fmove);
#ifdef ANDROID_DEBUG_LOG
			LOGI("write data size:%d",winfo->fmove);
#endif
		}
		if(step < lens){
			winfo->left = lens - step;
#ifdef ANDROID_DEBUG_LOG
			LOGI("left data size:%d",winfo->left);
#endif
			winfo->ldata = calloc(sizeof(short),winfo->left);
			memcpy(winfo->ldata,data + step,sizeof(short) * winfo->left);
		}
		/*音频的结尾了*/
		/*
		if(flg == 1){
			sf_write_short (winfo->sf,winfo->start_data, winfo->start_append * winfo->fsize);
		}
		*/
	}else{
		sf_count_t count ;
		if ((count = sf_write_short (winfo->sf, sdata, lens)) != lens){
			return -1;
		}
#ifdef ANDROID_DEBUG_LOG
		LOGI("write data size:%d",lens);
#endif
	}
	sf_write_sync(winfo->sf);
	return 0;
}

void close_fd(wav_info* winfo){
	winfo->left = 0;
	winfo->start = -1;
	winfo->end = -1;
	winfo->ldata = NULL;
	winfo->start_data = NULL;
	winfo->mfccs = NULL;
	winfo->mass = NULL;
	winfo->rms = NULL;
	winfo->frame_num = 0;
	if(winfo->sf != NULL){
		sf_close (winfo->sf) ;
	}
}
