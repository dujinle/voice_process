#ifndef __MEL_BANK_H__
#define __MEL_BANK_H__

///////////////////////////////////////////
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
void mel_fbankm( int p, int n,double fs, double fl, double fh, double** coeff );
////////////////////////////////////////////
#endif

