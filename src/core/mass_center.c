#include <math.h>
#include "core/mass_center.h"
#include "core/tools.h"

/* 计算一帧的质心量
 *@param
 * n		数据的长度
 * fdata	数据的值
 * sub_bans	数据的子带划分
 *@return
 * double	质心的值
 */
double mass_center(int n,double* fdata,int sub_bans){
	int i,j;
	int fsize = floor(n/sub_bans);
	double fas_sum = 0,fbs_sum = 0;
	for(i = 0;i < sub_bans;i++){
		double fb_sum = 0;
		for(j = 0;j < fsize;j++){
			fb_sum += fdata[i * fsize + j] * fdata[i * fsize + j];
		}
		double fbs = sqrt(fb_sum / fsize);
		fbs_sum = fbs_sum + fbs;
		fas_sum = fas_sum + fbs * (i + 1);
	}
	return fas_sum/fbs_sum;
}

/* 计算一帧的均方根
 *@param
 * n		数据的长度
 * fdata	数据的值
 * sub_bans	数据的子带划分
 *@return
 * double	均方根
 */
double rms(int n,double* fdata,int sub_bans){
	int i,j;
	int fsize = floor(n/sub_bans);
	double fas_sum = 0,fbs_sum = 0;
	for(i = 0;i < sub_bans;i++){
		double fb_sum = 0;
		for(j = 0;j < fsize;j++){
			fb_sum += fdata[i * fsize + j] * fdata[i * fsize + j];
		}
		double fbs = sqrt(fb_sum / fsize);
		fbs_sum = fbs_sum + fbs * fbs;
		fas_sum = fas_sum + fbs;
	}
	return sqrt(fbs_sum/(fas_sum * fas_sum)/sub_bans);
}
