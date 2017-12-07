#include "core/tools.h"
#include <math.h>

/* 复数 转化为 实数 abs(a + bi) = sqrt(a^2 + b^2)*/
int abs_complex(complex* in,int n,double* out){
	int i = 0;
	for ( i = 0;i < n;i++){
		out[i] = sqrt(in[i].real * in[i].real + in[i].imag * in[i].imag);
	}
	return 0;
}
/* 生成矩阵容器 */
double** calloc_mat(int m, int n){
	int i = 0;
	double **in = (double**)calloc(sizeof(double*),m);
	double* data = (double*)calloc(sizeof(double),m * n);

	for(i = 1; i <= m; i++){
		in[i-1] = &data[(i-1)*n];
	}
	return in;
}

void square_array(int n,double*fin){
	int i = 0;
	for(i = 0;i < n;i++){
		fin[i] = fin[i] * fin[i];
	}
}

void mat_mul_array(int x1,int y1,double** a,int x2,double*b,double* out){
	int i,j = 0;
	for(i = 0;i < x1;i++){
		for(j = 0;j < y1;j++){
			out[i] = out[i] + a[i][j] * b[j];
		}
	}
}
void arr_mul_arr(int x,double* a,int y,double* b,double* out){
	int i = 0;
	for(i = 0;i < x;i++){
		out[i] = a[i] * b[i];
	}
}

void log_array(int n,double* fin){
	int i = 0;
	for(i = 0;i < n;i++){
		fin[i] = log(fin[i]);
	}
}
