#include <android/native_window_jni.h>
#include <jni.h>
#include "process.h"
#include "log.h"

static jclass native_tm_class = NULL;

jbyteArray JNICALL cinit(JNIEnv *env, jobject obj, jint size, jint fs, jint fsize, jint fmove){

	LOG_DEBUG("start gointo cinit......\n");
	wav_info* winfo = init_winfo(size,fs,fsize,fmove);
	if(winfo == NULL){
		return NULL;
	}
	int len = sizeof(wav_info);
	LOG_DEBUG("cinit:%d\n",len);
	char* wbytes = calloc(sizeof(char),len);
	memcpy(wbytes,winfo,len);
	jbyteArray jbytes = (*env)->NewByteArray(env,len);
	(*env)->SetByteArrayRegion(env,jbytes,0,len,wbytes);
	return jbytes;
}

jbyteArray JNICALL native_pfeat(JNIEnv *env, jobject obj, jbyteArray inst, jdoubleArray data){

	jdouble* ddata = (*env)->GetDoubleArrayElements(env,data,NULL);
	if(NULL == ddata){ return; }
	wav_info* winfo = calloc(sizeof(wav_info),1);
	memcpy(winfo,inst,sizeof(wav_info));

	process_feat(winfo,ddata);
	(*env)->ReleaseDoubleArrayElements(env,data,ddata,0);

	int len = sizeof(wav_info);
	jbyteArray jbytes = (*env)->NewByteArray(env,len);
	(*env)->SetByteArrayRegion(env,jbytes,0,len,winfo);
	return jbytes;
}

jdoubleArray JNICALL native_compare(JNIEnv* env, jobject obj, jlong p1, jlong p2) {

	//wav_info* w1 = reinterpret_cast<wav_info*>(p1);
	//wav_info* w2 = reinterpret_cast<wav_info*>(p2);
	wav_info* w1 = (wav_info*)(p1);
	wav_info* w2 = (wav_info*)(p2);
	double* dist = compare_rec(w1,w2);
	jdoubleArray iarr = (*env)->NewDoubleArray(env,3);
	(*env)->SetDoubleArrayRegion(env,iarr,0,3,dist);

	return iarr;
}

static JNINativeMethod methods[] = {
	{ "create_obj", "(IIII)[B", &cinit},
	{ "process_feat", "(J[D)[B", &native_pfeat},
	{ "compare", "(JJ)[D", &native_compare},
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
