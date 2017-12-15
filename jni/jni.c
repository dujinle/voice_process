#include <android/native_window_jni.h>
#include <jni.h>
#include "process.h"
#include "log.h"

static jclass native_tm_class = NULL;

/* 测试用接口*/
jbyteArray JNICALL cinit_test(JNIEnv *env, jobject obj, jint size, jint fs, jint fsize, jint fmove){

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

jdoubleArray JNICALL pfeat_test(JNIEnv *env, jobject obj, jbyteArray inst, jdoubleArray data){

	LOG_DEBUG("start gointo pfeat_test......\n");

	jbyte* bytes = (*env)->GetByteArrayElements(env,inst,NULL);
	if(bytes == NULL){ return; }

	wav_info* winfo = calloc(sizeof(wav_info),1);
	memcpy(winfo,bytes,sizeof(wav_info));
	LOG_DEBUG("create winfo success......\n");
	LOG_DEBUG("size:%d......\n",winfo->size);
	LOG_DEBUG("fs:%d......\n",winfo->fs);
	LOG_DEBUG("fsize:%d......\n",winfo->fsize);

	jdouble* ddata = (*env)->GetDoubleArrayElements(env,data,NULL);
	if(NULL == ddata){ return; }

	double* indata = calloc(sizeof(double),winfo->size);
	memcpy(indata,ddata,sizeof(double) * winfo->size);
	process_feat(winfo,indata);

	LOG_DEBUG("frame_num:%d......\n",winfo->frame_num);
	LOG_DEBUG("mfcc_size:%d......\n",winfo->mfcc_size);

	int i, j,idx = 0;
	int mfcc_size = winfo->mfcc_size * winfo->frame_num;
	int len = mfcc_size +  2 * winfo->frame_num;
	jdouble *tmp = (jdouble*)calloc(sizeof(jdouble),len);

	jdoubleArray jds = (*env)->NewDoubleArray(env,len);
	for(i = 0;i < winfo->frame_num;i++){
		for(j = 0;j < winfo->mfcc_size;j++){
			tmp[idx++] = winfo->mfccs[i][j];
			LOG_DEBUG("%d %d %f",i,j,winfo->mfccs[i][j]);
		}
		LOG_DEBUG("end........................");
		tmp[idx++] = winfo->mass[i];
		tmp[idx++] = winfo->rms[i];
	}
	LOG_DEBUG("copy mfcc mass rms success [%d]......\n",idx);

	(*env)->SetDoubleArrayRegion(env,jds,0,len,tmp);

	(*env)->ReleaseDoubleArrayElements(env,data,ddata,0);

	return jds;
}

jdoubleArray JNICALL compare_test(JNIEnv* env, jobject obj, jbyteArray p1, jbyteArray p2) {

	//wav_info* w1 = reinterpret_cast<wav_info*>(p1);
	//wav_info* w2 = reinterpret_cast<wav_info*>(p2);
	/*
	wav_info* w1 = (wav_info*)(p1);
	wav_info* w2 = (wav_info*)(p2);
	double* dist = compare_rec(w1,w2);
	jdoubleArray iarr = (*env)->NewDoubleArray(env,3);
	(*env)->SetDoubleArrayRegion(env,iarr,0,3,dist);
	return iarr;
	*/
	return NULL;
}

/* 以下是 release 使用接口 */
jlong JNICALL get_wav_handler(JNIEnv *env, jobject obj,jstring filename){
	const char* str = (*env)->GetStringUTFChars(env,filename, NULL);
	LOG_DEBUG("start gointo get_wav_handler %s\n",str);
	wav_info* winfo = read_handler(str);

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

static JNINativeMethod methods[] = {
	{ "init_test", "(IIII)[B", &cinit_test},
	{ "init_real", "(IIII)J", &cinit_real},
	{ "pfeat_test", "([B[D)[D", &pfeat_test},
	{ "pfeat_real", "(J[D)V", &pfeat_real},
	{ "compare_test", "([B[B)[D", &compare_test},
	{ "compare_real", "(JJ)[D", &compare_real},
	{ "get_handler", "(Ljava/lang/String;)J", &get_wav_handler},
	{ "read_wav", "(JI)[D", &native_read_wav},
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
