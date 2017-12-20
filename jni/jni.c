#include <android/native_window_jni.h>
#include <jni.h>
#include "process.h"
#include "log.h"

static jclass native_tm_class = NULL;

/* 以下是 release 使用接口 */
jlong JNICALL get_wav_handler(JNIEnv *env, jobject obj,jstring filename){
	const char* str = (*env)->GetStringUTFChars(env,filename, NULL);
	LOG_DEBUG("start gointo get_wav_handler %s\n",str);
	wav_info* winfo = read_handler(str);
	if(winfo == NULL){
		return -1;
	}

	return (jlong)winfo;
}

jlong JNICALL get_write_handler(JNIEnv *env, jobject obj,jstring filename,jint fs,jint bits,jint channels){
	const char* str = (*env)->GetStringUTFChars(env,filename, NULL);
	LOG_DEBUG("start gointo get_write_handler %s\n",str);

	wav_info* winfo = creat_wchar_writer(str,fs,bits,channels);

	if(winfo == NULL){
		return -1;
	}
	return (jlong)winfo;
}

jlong JNICALL cinit_real(JNIEnv *env, jobject obj, jint size, jint fs, jint fsize, jint fmove){

	LOG_DEBUG("start gointo cinit......\n");
	wav_info* winfo = init_winfo(size,fs,fsize,fmove);
	if(winfo == NULL){
		return NULL;
	}
	return (jlong)winfo;
}

jdoubleArray JNICALL native_read_wav(JNIEnv *env, jobject obj,jlong inst,jint size){

	wav_info* winfo = (wav_info*)inst;
	double* data = read_wav(winfo,size);

	jdoubleArray iarr = (*env)->NewDoubleArray(env,size + 1);
	(*env)->SetDoubleArrayRegion(env,iarr,0,size + 1,data);
	return iarr;
}

jint JNICALL native_write_wav(JNIEnv *env, jobject obj,jlong inst,jshortArray data,jint size){

	wav_info* winfo = (wav_info*)inst;
	jshort* sdata = (*env)->GetShortArrayElements(env,data,NULL);
	int rec = write_cdata(winfo,sdata,size);
	(*env)->ReleaseShortArrayElements(env,data,sdata,0);

	return rec;
}

void JNICALL pfeat_real(JNIEnv *env, jobject obj, jlong inst, jdoubleArray data){

	jdouble* ddata = (*env)->GetDoubleArrayElements(env,data,NULL);
	if(NULL == ddata){ return; }
	wav_info* winfo = (wav_info*)inst;

	double* indata = calloc(sizeof(double),winfo->size);
	memcpy(indata,ddata,sizeof(double) * winfo->size);
	process_feat(winfo,indata);
	(*env)->ReleaseDoubleArrayElements(env,data,ddata,0);
}

jdoubleArray JNICALL compare_real(JNIEnv* env, jobject obj, jlong p1, jlong p2) {

	wav_info* w1 = (wav_info*)(p1);
	wav_info* w2 = (wav_info*)(p2);
	double* dist = compare_rec(w1,w2);
	LOG_DEBUG("%f %f %f",dist[0],dist[1],dist[2]);
	jdoubleArray iarr = (*env)->NewDoubleArray(env,3);
	(*env)->SetDoubleArrayRegion(env,iarr,0,3,dist);
	return iarr;
}

void JNICALL native_close_file(JNIEnv *env, jobject obj, jlong inst){
	wav_info* winfo = (wav_info*)(inst);
	close_fd(winfo);
}

static JNINativeMethod methods[] = {
	{ "init_real","(IIII)J",&cinit_real},
	{ "pfeat_real", "(J[D)V", &pfeat_real},
	{ "compare_real", "(JJ)[D", &compare_real},
	{ "get_handler", "(Ljava/lang/String;)J", &get_wav_handler},
	{ "get_write_handler", "(Ljava/lang/String;III)J", &get_write_handler},
	{ "read_wav", "(JI)[D", &native_read_wav},
	{ "write_wav", "(J[SI)I", &native_write_wav},
	{ "close_file", "(J)V", &native_close_file},
};

//extern "C" JNIEXPORT 
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm,(void**)&env, JNI_VERSION_1_6) != JNI_OK) {
#ifdef DEBUG_LOG
		LOG_ERROR("get env failed");
#endif
		return JNI_ERR;
	}

	native_tm_class = (*env)->FindClass(env,"jni/VprocessJNI");
	if(native_tm_class == NULL){
#ifdef DEBUG_LOG
		LOG_ERROR("not found java class VprocessJNI");
#endif
		return JNI_ERR;
	}
	native_tm_class = (jclass)(*env)->NewGlobalRef(env,native_tm_class);

	int len = sizeof(methods) / sizeof(methods[0]);
	if ((*env)->RegisterNatives(env,native_tm_class, methods,len) < 0) {
#ifdef DEBUG_LOG
		LOG_ERROR("register natives methods failed");
#endif
		return JNI_ERR;
	}
	return JNI_VERSION_1_6;
}


//extern "C" JNIEXPORT 
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm,(void**)&env, JNI_VERSION_1_6) != JNI_OK) {
#ifdef DEBUG_LOG
		LOG_ERROR("get env failed");
#endif
		return ;
	}
	if(native_tm_class != NULL){
		(*env)->DeleteLocalRef(env,native_tm_class);
	}
}
