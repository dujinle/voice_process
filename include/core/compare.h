#ifndef COMPARE_HEADER
#define COMPARE_HEADER

/*对 frame_num帧的mfcc参数进行距离计算 */
double mfcc_comp(int frame_num,int dsize,double** base,double** feature);

/* 对 质心进行对比计算*/
double massc_comp(int frame_num,double* bmass,double* fmass);
#endif
