#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "core/fft.h"

///////////////////////////////////////////
/* 复数 转化为 实数 abs(a + bi) = sqrt(a^2 + b^2)*/
int abs_complex(complex* in,int n,double* out);

/* 生成矩阵容器 */
double** calloc_mat(int m, int n);

/* 计算数据的平方*/
void square_array(int n,double*fin);

/* 计算矩阵乘以 数组*/
void mat_mul_array(int x1,int y1,double** a,int x2,double*b,double* out);

void arr_mul_arr(int x,double* a,int y,double* b,double* out);

/* log 对数组的每一个值进行log运算 */
void log_array(int n,double* fin);
////////////////////////////////////////////
#endif

