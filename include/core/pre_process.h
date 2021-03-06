#ifndef PRE_PROCESS_HEADER
#define PRE_PROCESS_HEADER

/* 预加重 */
void pre_emphasise(const double *in,int offset,int len,double *out,double pref);

/* 给每一帧加窗 */
void hamming(double *win,int len);

/* 分帧操作
 *@param
 * len			本次数据的长度
 * frame_size	设定帧长大小
 * frame_inc	帧移大小
 *@return
 * 返回分帧的帧数
 * int*	指向分帧的开始数组位置
 */

int* enframe(int len,int frame_size,int frame_inc);

/* 对数据进行归一化操作 结果返回在 f中*/
void normalization(int n,double* f);
#endif
