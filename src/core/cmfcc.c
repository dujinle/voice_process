#include <math.h>
#include "core/cmfcc.h"
#include "core/tools.h"

/*
 * 对每一帧计算一组mfcc参数 k = 12个
 *@param
 * p     fbank的滤波器个数 24
 * fsize 每个滤波器的参数个数
 * eoff  滤波器的参数值
 * fft   进行fft变化之后的值
 * fftn  fft的个数
 *@return
 * int mfcc个数 12
 * out 参数值
 */
int mfcc(int p,int fsize,double** eoff,double* fft,int fftn,int k,double* mout){

	//获取12个mfcc参数
	int i,j;
	double maxw = 0;
	double* outp = calloc(sizeof(double),p);
	double* outk = calloc(sizeof(double),k);
	double* w = calloc(sizeof(double),k);
	double** dctcoef = calloc_mat(k,p);
	// init dctcoef params
	for(i = 0;i < k;i++){
		for(j = 0;j < p;j++){
			dctcoef[i][j] = cos( (2 * j + 1) * (i + 1) * PI / (2 * p));
		}
		w[i] = 1 + 6 * sin(PI * (i + 1)/k);
		if(w[i] > maxw){
			maxw = w[i];
		}
	}
	for(i = 0;i < k;i++){
		w[i] = w[i] / maxw;
	}

	mat_mul_array(p,fsize,eoff,fsize,fft,outp);
	/*
	for(i = 0;i < p;i++){
		printf("%4.10f\t",outp[i]);
	}
	*/
	log_array(p,outp);
	mat_mul_array(k,p,dctcoef,p,outp,outk);

	/*
	for(i = 0;i < k;i++){
		printf("%4.10f\t",w[i]);
	}
	printf("\n");
	*/
	arr_mul_arr(k,w,k,outk,mout);
	return k;
}
