#include <math.h>
#include "core/compare.h"

/*
 *对 frame_num帧的mfcc参数进行距离计算
 *@param
 * frame_num	帧数
 * dsize		mfcc参数个数 12
 * base			样本特征数据 大小 frame_num*dsize
 * feature		被测特征数据 大小 frame_num*dsize
 *@return
 * double		距离值
 */
double mfcc_comp(int frame_num,int dsize,double** base,double** feature){

	int i,j = 0;
	double disp_sum = 0;
	double* btmp = (double*)calloc(sizeof(double),dsize);
	double* ftmp = (double*)calloc(sizeof(double),dsize);
	for(i = 0;i < frame_num;i++){
		memcpy(btmp,base[i],sizeof(double) * dsize);
		memcpy(ftmp,feature[i],sizeof(double) * dsize);

		double bmean = 0,fmean = 0;
		for(j = 0;j < dsize;j++){
			bmean = bmean + btmp[j]/dsize;
			fmean = fmean + ftmp[j]/dsize;
		}
		double disp = 0.0;
		for(j = 0;j < dsize;j++){
			btmp[j] = btmp[j] - bmean;
			ftmp[j] = ftmp[j] - fmean;
			disp = disp + (btmp[j] - ftmp[j]) * (btmp[j] - ftmp[j]) / dsize;
		}
		disp_sum = disp_sum + sqrt(disp);
	}
	return disp_sum / frame_num;
}

/* 对 质心进行对比计算*/
double massc_comp(int frame_num,double* bmass,double* fmass){

	int i = 0;
	double* btmp = (double*)calloc(sizeof(double),frame_num);
	double* ftmp = (double*)calloc(sizeof(double),frame_num);

	
	double bsum = 0.0,fsum = 0.0;
	for(i = 0;i < frame_num;i++){
		btmp[i] = bmass[i];
		ftmp[i] = fmass[i];
		bsum = bsum + btmp[i];
		fsum = fsum + ftmp[i];
	}
	double bmean = bsum / frame_num;
	double fmean = fsum / frame_num;
	double disp = 0.0;
	for(i = 0;i < frame_num;i++){
		btmp[i] = btmp[i] - bmean;
		ftmp[i] = ftmp[i] - fmean;
		disp = disp + (btmp[i] - ftmp[i]) * (btmp[i] - ftmp[i]);
	}
	return sqrt(disp) / frame_num / bmean;
}
