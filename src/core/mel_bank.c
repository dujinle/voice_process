#include "core/mel_bank.h"
#include <math.h>

#define min(a,b) ((a) < (b))?(a):(b)
static double frq2mel(double frq){
		return log(1+frq/700)*1127.01048;
}
static double mel2frq(double mel){
		return 700*(exp(mel/1127.01048)-1);
}

/* 生成mel 滤波器组
 *@param
 * p 滤波器的个数 24
 * n fft之后的数据个数
 * fs 数据的采样数 8000
 * fl 滤波器的下限阈值
 * fh  滤波器的上限阈值
 *@return
 * coeff 滤波器的返回参数p个数组
*/
void mel_fbankm(int p,int n,double fs,double fl,double fh,double **ceoff){
	int i,j,z = 0;
	int fn2 = floor(n / 2);

	double mfl = frq2mel(fl * fs);
	double mfh = frq2mel(fh * fs);
	double melinc = (mfh - mfl)/(p + 1);
	//printf("%f\n",melinc);

	double blim1 = mel2frq(0) * n /fs;
	double blim4 = mel2frq(melinc * (p + 1)) * n / fs;
	int b1 = floor(blim1) + 1;
	int b4 = min(fn2,ceil(blim4) - 1);
	//printf("%d %d\n",b1,b4);

	int k2 = -1,k3 = -1,k4 = -1;
	double pf[b4 + 1],fp[b4 + 1],pm[b4 + 1];
	for (i = b1;i <= b4;i++){
		pf[i] = frq2mel(i * fs / n)/melinc;
		fp[i] = floor(pf[i]);
		pm[i] = pf[i] - fp[i];
		if(i == b4 && pf[i] >= p + 1){
			pf[i] = -1;
			b4 = b4 - 1;
			break;
		}
		if(fp[i] > 0 && k2 == -1){
			k2 = i;
		}
		if(fp[i] < p){
			k3 = i;
		}
		//printf("%f ",pf[i]);
	}
	k4 = b4 - b1 + 1;
	//printf("create pf fp pm succ\n");

	int len = k3 + k4 - k2 + 2;
	//printf("%d %d %d\n",k2,k3,k4);
	double r[len],c[len],v[len];
	for(i = 1,j = 1;i <= k3;j++,i++){
		r[j] = 1 + fp[i];
		c[j] = i;
		v[j] = pm[i];
		//printf("%f=%f=%f\t",r[j],c[j],v[j]);
	}
	//printf("\n");
	for(i = k2;i <= k4;i++,j++){
		r[j] = fp[i];
		c[j] = i;
		v[j] = 1 - pm[i];
		//printf("%f=%f=%f\t",r[j],c[j],v[j]);
	}
	//printf("\n");
	int cl = j;
	double maxv = -1;
	for(j = 1;j < cl;j++){
		if(c[j] + b1 > 1 && c[j] + b1 < n - fn2 + 1){
			v[j] = 2* v[j];
		}
		//printf("%f\t",v[j]);
		int x = r[j];
		int y = c[j] + b1;
		if(maxv < v[j]){
			maxv = v[j];
		}
		ceoff[x - 1][y - 1] = v[j];
		//printf("%d,%d %f\t",x,y,v[j]);
	}
	//printf("\n");

	for(i = 0;i < p;i++){
		for(j = 0;j < fn2 + 1;j++){
			ceoff[i][j] = ceoff[i][j] / maxv;
//			printf("%4.10f ",coeff[i][j]);
		}
//		printf("\n");
	}
}
