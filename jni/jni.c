#include <android/native_window_jni.h>
#include <jni.h>
#include "process.h"

static jclass native_tm_class = NULL;

/* 以下是 release 使用接口 */
jint JNICALL set_wav_reader(JNIEnv *env, jobject obj,jlong inst,jstring filename){
	const char* str = (*env)->GetStringUTFChars(env,filename, NULL);
	LOGI("start gointo get_wav_handler %s\n",str);
	int ret = set_reader((wav_info*)inst,str);
	return ret;
}

jint JNICALL set_wav_writer(JNIEnv *env, jobject obj,jlong inst,jstring filename){
	const char* str = (*env)->GetStringUTFChars(env,filename, NULL);
	LOGI("start gointo get_write_handler %s\n",str);

	int ret = set_writer((wav_info*)inst,str);
	return ret;
}

jlong JNICALL cinit_real(JNIEnv *env, jobject obj,jstring conf){

	LOGI("start gointo cinit......\n");
	const char* str = (*env)->GetStringUTFChars(env,conf, NULL);
	wav_info* winfo = creat_winfo(str);
	if(winfo == NULL){
		return NULL;
	}
	return (jlong)winfo;
}

jdoubleArray JNICALL native_read_wav(JNIEnv *env, jobject obj,jlong inst,jint size){

	wav_info* winfo = (wav_info*)inst;
	double* data = calloc(sizeof(double),size + 1);
	if(data == NULL){
		return NULL;
	}
	int nread = read_double_wav(winfo,data + 1,size);
	data[0] = nread;

	jdoubleArray iarr = (*env)->NewDoubleArray(env,size + 1);
	(*env)->SetDoubleArrayRegion(env,iarr,0,size + 1,data);
	return iarr;
}

jint JNICALL native_write_wav(JNIEnv *env, jobject obj,jlong inst,jshortArray data,jint size,jint flg){

	wav_info* winfo = (wav_info*)inst;
	jshort* sdata = (*env)->GetShortArrayElements(env,data,NULL);
	int rec = write_cdata(winfo,sdata,size,flg);
	(*env)->ReleaseShortArrayElements(env,data,sdata,0);

	return rec;
}

void JNICALL pfeat_real(JNIEnv *env, jobject obj, jlong inst, jdoubleArray data){

	LOGI("start go into pfeat_real ......");
	jdouble* ddata = (*env)->GetDoubleArrayElements(env,data,NULL);
	if(NULL == ddata){ return; }
	wav_info* winfo = (wav_info*)inst;

	double* indata = calloc(sizeof(double),winfo->size);
	memcpy(indata,ddata,sizeof(double) * winfo->size);
	process_feat(winfo,indata);
	(*env)->ReleaseDoubleArrayElements(env,data,ddata,0);
}

jdoubleArray JNICALL compare_real(JNIEnv* env, jobject obj, jlong p1, jlong p2) {

	LOGI("start go into compare_real ......");
	wav_info* w1 = (wav_info*)(p1);
	wav_info* w2 = (wav_info*)(p2);
	double* dist = compare_rec(w1,w2);
	LOGI("%f %f %f",dist[0],dist[1],dist[2]);
	jdoubleArray iarr = (*env)->NewDoubleArray(env,3);
	(*env)->SetDoubleArrayRegion(env,iarr,0,3,dist);
	return iarr;
}

void JNICALL native_close_file(JNIEnv *env, jobject obj, jlong inst){
	wav_info* winfo = (wav_info*)(inst);
	close_fd(winfo);
}

static JNINativeMethod methods[] = {
	{ "init_real","(Ljava/lang/String;)J",&cinit_real},
	{ "pfeat_real", "(J[D)V", &pfeat_real},
	{ "compare_real", "(JJ)[D", &compare_real},
	{ "set_wave_reader", "(JLjava/lang/String;)I", &set_wav_reader},
	{ "set_wave_writer", "(JLjava/lang/String;)I", &set_wav_writer},
	{ "read_wav", "(JI)[D", &native_read_wav},
	{ "write_wav", "(J[SII)I", &native_write_wav},
	{ "close_file", "(J)V", &native_close_file},
};

//extern "C" JNIEXPORT 
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm,(void**)&env, JNI_VERSION_1_6) != JNI_OK) {
#ifdef ANDROID_DEBUG_LOG
		LOGE("get env failed");
#endif
		return JNI_ERR;
	}

	native_tm_class = (*env)->FindClass(env,"jni/VprocessJNI");
	if(native_tm_class == NULL){
#ifdef ANDROID_DEBUG_LOG
		LOGE("not found java class VprocessJNI");
#endif
		return JNI_ERR;
	}
	native_tm_class = (jclass)(*env)->NewGlobalRef(env,native_tm_class);

	int len = sizeof(methods) / sizeof(methods[0]);
	if ((*env)->RegisterNatives(env,native_tm_class, methods,len) < 0) {
#ifdef ANDROID_DEBUG_LOG
		LOGE("register natives methods failed");
#endif
		return JNI_ERR;
	}
	return JNI_VERSION_1_6;
}


//extern "C" JNIEXPORT 
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm,(void**)&env, JNI_VERSION_1_6) != JNI_OK) {
#ifdef ANDROID_DEBUG_LOG
		LOGE("get env failed");
#endif
		return ;
	}
	if(native_tm_class != NULL){
		(*env)->DeleteLocalRef(env,native_tm_class);
	}
}
