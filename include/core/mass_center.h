#ifndef __MASS_CENTER_H__
#define __MASS_CENTER_H__

///////////////////////////////////////////
/* 计算一帧的质心 */
double mass_center(int n,double* fdata,int sub_bans);

/* 计算一帧的均方根 */
double rms(int n,double* fdata,int sub_bans);
////////////////////////////////////////////
#endif

