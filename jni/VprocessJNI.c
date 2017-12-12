/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <android/native_window_jni.h>
#include "process.h"
#include "VprocessJNI.h"
#include "log.h"
/* Header for class VprocessJNI */

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     VprocessJNI
 * Method:    create_obj
 * Signature: (IIII)J
 */
JNIEXPORT jlong JNICALL Java_VprocessJNI_create_1obj(JNIEnv *env, jobject obj, jint size, jint fs, jint fsize, jint fmove){

	wav_info* winfo = init_winfo(size,fs,fsize,fmove);
	if(winfo == NULL){
		return -1;
	}
	jlong p_tm = (long)winfo;
	return p_tm;
}

/*
 * Class:     VprocessJNI
 * Method:    process_feat
 * Signature: (J[D)V
 */
JNIEXPORT void JNICALL Java_VprocessJNI_process_1feat(JNIEnv *env, jobject obj, jlong p_tm, jdoubleArray data){

  jdouble* ddata = (*env)->GetDoubleArrayElements(env,data,NULL);
  if(NULL == ddata){ return; }
  wav_info* native_tm = (wav_info*)p_tm;
  process_feat(native_tm,ddata);
}

/*
 * Class:     VprocessJNI
 * Method:    compare
 * Signature: (JJ)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_VprocessJNI_compare(JNIEnv *env, jobject obj, jlong p1, jlong p2){

	wav_info* w1 = (wav_info*)(p1);
	wav_info* w2 = (wav_info*)(p2);
	double* dist = compare_rec(w1,w2);
	jdoubleArray iarr = (*env)->NewDoubleArray(env,3);
	(*env)->SetDoubleArrayRegion(env,iarr,0,3,dist);

	return iarr;
}

/*
 * Class:     VprocessJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_VprocessJNI_destroy(JNIEnv *env, jobject obj, jlong p_tm){
	p_tm = 0;
}

#ifdef __cplusplus
}
#endif
