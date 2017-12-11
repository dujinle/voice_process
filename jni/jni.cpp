#include <android/native_window_jni.h>
#include "../include/process.h"
#include <jni.h>
#include "log.h"

extern "C" {

static JavaVM *g_vm = NULL;
static jclass native_tm_class = NULL;
static jobject lobj = NULL;

jlong JNICALL nativeCreate(JNIEnv* env, jobject obj,jint size,jint fs,jint fsize,jint fmove){

	lobj = env->NewGlobalRef(obj);
	wav_info* winfo = init_winfo(size,fs,fsize,fmove);
	if(winfo == NULL){
#ifdef ANDROID_DEBUG_LOG
		LOG("creat wav_info failed......");
#endif
		return -1;
	}
	jlong p_tm = reinterpret_cast<jlong>(winfo);
#ifdef ANDROID_DEBUG_LOG
	LOG("creat wav_info finish[%lld]",p_tm);
#endif
	return p_tm;
}

void JNICALL native_process(JNIEnv* env,jobject obj,jlong p_tm,jdoubleArray data){

	jdouble* ddata = env->GetDoubleArrayElements(data,NULL);
	if(bytes == ddata){ return; }

	wav_info* native_tm = reinterpret_cast<wav_info*>(p_tm);
	process_feat(native_tm,ddata);
}

void JNICALL nativeDestroy(JNIEnv* env, jobject obj, jlong p_tm) {

	wav_info* native_tm = reinterpret_cast<wav_info*>(p_tm);
	native_tm->exit();
	delete native_tm;
#ifdef ANDROID_DEBUG_LOG
	LOG("destroy thread_manager finish");
#endif
}

jdoubleArray JNICALL native_compare(JNIEnv* env,jobject obj,jlong p1,jlong p2)
	wav_info* w1 = reinterpret_cast<wav_info*>(p1);
	wav_info* w2 = reinterpret_cast<wav_info*>(p2);
	jdouble* dist = compare_rec(w1,w2);
	jdoubleArray iarr = env->NewDoubleArray(3);
	for(int j = 0; j < 3; j++) {
		iarr[j] = dist[j];
	}
	return iarr;
}

static JNINativeMethod methods[] = {
	{ "create_obj", "()J", reinterpret_cast<void*>(&nativeCreate) },
	{ "destroy", "(J)V", reinterpret_cast<void*>(&nativeDestroy)},
	{ "process_feat", "(J[D)V", reinterpret_cast<void*>(&native_process)},
	{ "compare", "(JJ)[D", reinterpret_cast<void*>(&native_compare)},
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

	g_vm = vm;
	JNIEnv* env = NULL;
	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
#ifdef ANDROID_DEBUG_LOG
		LOG_ERROR("get env failed");
#endif
		return JNI_ERR;
	}

	native_tm_class = env->FindClass("com/cyx/landmark/LandMarkJNI");
	if(native_tm_class == NULL){
#ifdef ANDROID_DEBUG_LOG
		LOG_ERROR("not found java class LandMarkJNI");
#endif
		return JNI_ERR;
	}
	native_tm_class = (jclass)env->NewGlobalRef(native_tm_class);

	int len = sizeof(methods) / sizeof(methods[0]);
	if (env->RegisterNatives(native_tm_class, methods,len) < 0) {
#ifdef ANDROID_DEBUG_LOG
		LOG_ERROR("register natives methods failed");
#endif
		return JNI_ERR;
	}
	return JNI_VERSION_1_6;
}


extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	JNIEnv* env = NULL;
	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
#ifdef ANDROID_DEBUG_LOG
		LOG_ERROR("get env failed");
#endif
		return ;
	}
	if(native_tm_class != NULL){
		env->DeleteLocalRef(native_tm_class);
	}
	if(lobj != NULL){
		env->DeleteLocalRef(lobj);
	}
}
