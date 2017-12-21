#include "core/pre_process.h"
#include <math.h>
#include <stdlib.h>

/* 预加重 pref 默认 0.9375*/
void pre_emphasise(const double *in,int offset,int len,double *out,double pref){
	int i = 0;
	if(offset == 0){
		for(i = len - 1; i >= 1;i--){
			out[i] = in[i] - pref * in[i - 1];
		}
		out[0] = in[0];
	}else{
		for(i = offset + len - 1; i >= offset;i--){
			out[i] = in[i] - pref * in[i - 1];
		}
	}
}

/* 给每一帧加窗 */
void hamming(double *win,int len){
	int i = 0;
	double PI = 3.14159265358979323846;
	for(i = 0;i < len;i++){
		double hamm = 0.54 - 0.46 * cos (2 * PI *  (double)i / (len-1) );
		win[i] = win[i] * hamm;
	}
}

int* enframe(int len,int frame_size,int frame_inc){
	/* 计算可以分多少帧*/
	int frame_len = floor((len - frame_size + frame_inc) / frame_inc);
	if(frame_len > 0){
		int* frames = calloc(sizeof(int),frame_len + 1);
		frames[0] = frame_len;

		int i,step = 0;
		for(i = 1;i <= frame_len;i++){
			frames[i] = step;
			step = step + frame_inc;
		}
		return frames;
	}

	return NULL;
}

/* 对数据进行归一化操作 结果返回在 f中*/
void normalization(int n,double* f){

	int i = 0;
	double max = 2< 10 * -1,min = 2< 10;
	for(i = 0;i < n;i++){
		if(max <= f[i]){
			max = f[i];
		}
		if(min >= f[i]){
			min = f[i];
		}
	}
	double t = max - min;
	for(i = 0;i < n;i++){
		f[i] = (f[i] - min) / t;
	}
}
