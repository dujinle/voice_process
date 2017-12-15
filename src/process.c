#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<inttypes.h>
#include	<ctype.h>

#include	"vad_snr/vad.h"
#include	"core/pre_process.h"
#include	"core/fft.h"
#include	"core/tools.h"
#include	"core/mel_bank.h"
#include	"core/cmfcc.h"
#include	"core/mass_center.h"
#include	"process.h"

wav_info* init_winfo(int size,int fs,int fsize,int fmove){
	wav_info* winfo = calloc(sizeof(wav_info),1);
	winfo->size = size;
	winfo->fs = fs;
	winfo->fsize = fsize;
	winfo->fmove = fmove;
	winfo->bank_num = 32;
	winfo->fbank_num = 24;
	winfo->mfcc_size = 12;
	winfo->start = -1;
	winfo->end = -1;

	return winfo;
}

/* 处理被测试音频数据 */
void process_feat(wav_info* winfo,double* data){

	int i = 0;//mfcc参数个数12
	int fbank_size = winfo->fsize /2 + 1;//滤波器的参数个数

	vad_stc* vad = vad_creat(winfo->fs,winfo->fsize,winfo->fmove);

	//printf("pre_emphasise......\n");
	pre_emphasise(data,0,winfo->size,data,0.9375);

	//printf("enframe......\n");
	int* frames = enframe(0,winfo->size,winfo->fsize,winfo->fmove,0);
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
		//printf("mcenter:%4.10f %4.10f\n",mass_arr[i],rms_arr[i]);

		//数据加窗处理 为后面的特征提取做准备
		hamming(fdata,winfo->fsize);

		//进行fft变换
		convert_complex(fdata,winfo->fsize,complexs);
		//对数据实值化 消除 虚数部分
		abs_complex(complexs,winfo->fsize,fdata);

		square_array(winfo->fsize,fdata);
		//构造melbank系数
		mel_fbankm(winfo->fbank_num,winfo->fsize,winfo->fs,0, 0.5,coeff);
		//printf("mel_bank success......\n");
		mfcc(winfo->fbank_num,fbank_size,coeff,fdata,fbank_size,winfo->mfcc_size,mfccs);
		memcpy(mfcc_mat[i],mfccs,sizeof(double) * winfo->mfcc_size);
		//printf("mfcc success......\n");
	}
	winfo->mfccs = mfcc_mat;
	winfo->mass = mass_arr;
	winfo->rms = rms_arr;
}

double* compare_rec(wav_info* w1,wav_info* w2){

	int i = 0;
	double* min_dist = calloc(sizeof(double),3);
	if(w2->frame_num >= w1->frame_num){
		for(i = 0;i < w2->frame_num - w1->frame_num;i++){
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
			printf("%d mfcc:%4.10f mass:%4.10f rms:%4.10f\n",i,mfcc_disp,massc_disp,rms_disp);
			//break
		}
	}
	return min_dist;
}

wav_info* read_handler(char* filename){

	SF_INFO sfinfo;
	memset (&sfinfo, 0, sizeof (SF_INFO));
	SNDFILE* sf = sf_open (filename, SFM_READ, &sfinfo);
	if(sf == NULL){
		printf ("Error opening %s.\n", filename) ;
		return NULL;
	}
	sf_count_t samples = sfinfo.frames / sfinfo.channels;
	wav_info* winfo = init_winfo(samples,sfinfo.samplerate,256,80);
	winfo->sf = sf;
	return winfo;
}

double* read_wav(wav_info* winfo,int size){
	double* data = (double*)calloc(sizeof(double),size + 1);
	if(data == NULL){
		return NULL;
	}
	int nread = sf_readf_double(winfo->sf,data + 1,size);
	data[0] = nread;
	return data;
}
