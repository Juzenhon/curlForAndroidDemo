
#ifndef _COMM_FUNCTION_H_
#define _COMM_FUNCTION_H_

#include <jni.h>

jboolean JNU_GetBoolField(JNIEnv* _env, jobject obj, const char* _name);
jint JNU_GetIntField(JNIEnv* _env, jobject obj, const char* _name);
jlong JNU_GetLongField(JNIEnv* _env, jobject obj, const char* _name);
jstring JNU_GetStringField(JNIEnv* _env, jobject obj, const char* _name);

void JNU_SetIntField(JNIEnv* _env, jobject obj, const char* _name,jint value);
void JNU_SetLongField(JNIEnv* _env, jobject obj, const char* _name,jlong value);
void JNU_SetStringField(JNIEnv *_env, jobject obj, const char *_name, jstring value);

void JNU_FunCheckException(JNIEnv* env, const char* name, const char* descriptor);

void JNU_CallVoidMethod(JNIEnv*, jobject, const char*, const char* , ...);
jboolean JNU_CallBoolMethod(JNIEnv*, jobject, const char*, const char* , ...);
jint JNU_CallIntMethod(JNIEnv*, jobject, const char*, const char* , ...);
jobject JNU_CallObjectMethod(JNIEnv*, jobject, const char*, const char* , ...);

void JNU_CallStaticVoidMethod(JNIEnv*, jclass , const char* name, const char* descriptor, ...);


wchar_t* JNU_Jstring2Wchar(JNIEnv* _env, const jstring jstr);

// char* to jstring
jstring JNU_Chars2Jstring(JNIEnv* _env, const char* pat);

#endif
