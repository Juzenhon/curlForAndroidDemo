#include <memory.h>
#include "JNILog.h"
#include "CommFunction.h"

#include "VarCache.h"

jboolean JNU_GetBoolField(JNIEnv *env, jobject obj, const char *name) {

    ASSERT(NULL != env);
    ASSERT(NULL != obj);
    ASSERT(NULL != name);

    jclass clazz = env->GetObjectClass(obj);
    if (!clazz) {
        LOGE("JNU_GetField: GetObjectClass failed! '%s' ", name);
        return 0;
    }
    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "Z");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGE("JNU_GetBoolField: GetFieldId fail! '%s' ", name);
        return 0;
    }
    return env->GetBooleanField(obj, fid);
}

jint JNU_GetIntField(JNIEnv *env, jobject obj, const char *name) {

    ASSERT(NULL != env);
    ASSERT(NULL != obj);
    ASSERT(NULL != name);

    jclass clazz = env->GetObjectClass(obj);
    if (!clazz) {
        LOGE("JNU_GetField: GetObjectClass failed! '%s' ", name);
        return 0;
    }
    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "I");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGE("JNU_GetIntField: GetFieldId fail! '%s' ", name);
        return 0;
    }
    return env->GetIntField(obj, fid);
}

jlong JNU_GetLongField(JNIEnv *env, jobject obj, const char *name) {
    ASSERT(NULL != env);
    ASSERT(NULL != obj);
    ASSERT(NULL != name);

    jclass clazz = env->GetObjectClass(obj);
    if (0 == clazz) {
        LOGE("JNU_GetLongField: GetObjectClass failed! '%s' ", name);
        return 0;
    }
    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "J");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGE("JNU_GetLongField: GetFieldId fail! '%s' ", name);
        return 0;
    }
    return env->GetLongField(obj, fid);
}

jstring JNU_GetStringField(JNIEnv *env, jobject obj, const char *name) {
    ASSERT(0 != env);
    ASSERT(0 != obj);
    ASSERT(0 != name);

    jclass clazz = env->GetObjectClass(obj);
    if (0 == clazz) {
        LOGE("JNU_GetStringField: GetObjectClass failed! '%s' ", name);
        return 0;
    }
    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "Ljava/lang/String;");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGE("JNU_GetStringField: GetFieldId fail! '%s' ", name);
        return 0;
    }
    return (jstring) env->GetObjectField(obj, fid);
}

void JNU_SetIntField(JNIEnv *env, jobject obj, const char *name, jint value) {

    jclass clazz = env->GetObjectClass(obj);
    if (clazz == 0) {
        LOGE("JNU_SetIntField: GetObjectClass failed! '%s' ", name);
        return;
    }

    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "I");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGD("JNU_SetIntField: field '%s' not found ", name);
        return;
    }
    env->SetIntField(obj, fid, value);
}

void JNU_SetLongField(JNIEnv *env, jobject obj, const char *name, jlong value) {


    jclass clazz = env->GetObjectClass(obj);
    if (clazz == 0) {
        LOGE("JNU_SetLongField: GetObjectClass failed! '%s' ", name);
        return;
    }

    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "J");
    env->DeleteLocalRef(clazz);

    if (NULL == fid) {
        LOGE("JNU_SetLongField: field '%s' not found ", name);
        return;
    }
    env->SetLongField(obj, fid, value);
}

void
JNU_SetStringField(JNIEnv *env, jobject obj, const char *name, jstring value) {


    jclass clazz = env->GetObjectClass(obj);
    if (0 == clazz) {
        LOGE("JNU_SetStringField: GetObjectClass failed! '%s'", name);
        return;
    }

    jfieldID fid = VarCache::Singleton()->GetFieldId(env, clazz, name, "Ljava/lang/String;");
    env->DeleteLocalRef(clazz);

    if (0 == fid) {
        LOGE("JNU_SetStringField: field '%s' not found ", name);
        return;
    }
    env->SetObjectField(obj, fid, value);
}

void JNU_FunCheckException(JNIEnv *env, const char *name, const char *descriptor) {
    jthrowable throwObj = env->ExceptionOccurred();
    if (throwObj) {
        LOGD("exec function %s failed", name);
        env->ExceptionClear();
        char err_msg[512] = {0};
        snprintf(err_msg, sizeof(err_msg), "exec function :%s, sig:%s has a exception!!!", name,
                 descriptor);
        env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
//            jclass clz = env->GetObjectClass(throwobj);
//            env->ThrowNew(clz, name);
    }
}

void JNU_CallVoidMethod(JNIEnv *env, jobject obj, const char *name, const char *descriptor, ...) {
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    ASSERT(descriptor);
    jclass clzz = env->GetObjectClass(obj);
    jmethodID mid = VarCache::Singleton()->GetMethodId(env, clzz, name, descriptor);
    env->DeleteLocalRef(clzz);
    if (!mid) {
        LOGE("JNU_CallVoidMethod:getMethod name '%s' descriptor '%s' not found", name, descriptor);
        return;
    }
    va_list args;
    va_start(args, descriptor);
    env->CallVoidMethodV(obj, mid, args);
    va_end(args);

    JNU_FunCheckException(env, name, descriptor);
}

jboolean
JNU_CallBoolMethod(JNIEnv *env, jobject obj, const char *name, const char *descriptor, ...) {
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    ASSERT(descriptor);
    jclass clzz = env->GetObjectClass(obj);
    jmethodID mid = VarCache::Singleton()->GetMethodId(env, clzz, name, descriptor);
    env->DeleteLocalRef(clzz);
    if (!mid) {
        LOGE("JNU_CallBoolMethod:getMethod name '%s' descriptor '%s' not found", name, descriptor);
        return JNI_FALSE;
    }

    va_list args;
    va_start(args, descriptor);
    jboolean result = env->CallBooleanMethodV(obj, mid, args);
    va_end(args);

    JNU_FunCheckException(env, name, descriptor);
    return result;
}

jint JNU_CallIntMethod(JNIEnv *env, jobject obj, const char *name, const char *descriptor, ...) {
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    ASSERT(descriptor);
    jclass clzz = env->GetObjectClass(obj);
    jmethodID mid = VarCache::Singleton()->GetMethodId(env, clzz, name, descriptor);
    env->DeleteLocalRef(clzz);
    if (0 == mid) {
        LOGE("JNU_CallIntMethod:getMethod name '%s' descriptor '%s' not found", name, descriptor);
        return 0;
    }
    va_list args;
    va_start(args, descriptor);
    jint result = env->CallIntMethodV(obj, mid, args);
    va_end(args);

    JNU_FunCheckException(env, name, descriptor);
    return result;
}

jobject
JNU_CallObjectMethod(JNIEnv *env, jobject obj, const char *name, const char *descriptor, ...) {
    ASSERT(env);
    ASSERT(obj);
    ASSERT(name);
    ASSERT(descriptor);
    jclass clzz = env->GetObjectClass(obj);
    jmethodID mid = VarCache::Singleton()->GetMethodId(env, clzz, name, descriptor);
    env->DeleteLocalRef(clzz);
    if (0 == mid) {
        LOGE("JNU_CallObjectMethod:getMethod name '%s' descriptor '%s' not found", name,
             descriptor);
        return 0;
    }
    va_list args;
    va_start(args, descriptor);
    jobject result = env->CallObjectMethodV(obj, mid, args);
    va_end(args);

    JNU_FunCheckException(env, name, descriptor);
    return result;
}

void
JNU_CallStaticVoidMethod(JNIEnv *env, jclass clzz, const char *name, const char *descriptor, ...) {
    ASSERT(env);
    ASSERT(clzz);
    ASSERT(name);
    ASSERT(descriptor);
    jmethodID mid = VarCache::Singleton()->GetStaticMethodId(env, clzz, name, descriptor);
    if (0 == mid) {
        LOGE("JNU_CallStaticVoidMethod: name '%s' descriptor '%s' not found", name, descriptor);
        return;
    }
    va_list args;
    va_start(args, descriptor);
    env->CallStaticVoidMethodV(clzz, mid, args);
    va_end(args);

    JNU_FunCheckException(env, name, descriptor);
}


/**
 * @param env
 * @param jstr
 * @return
 */
wchar_t *JNU_Jstring2Wchar(JNIEnv *env, const jstring jstr) {
    ASSERT(env != NULL);

    if (jstr == NULL)
        return NULL;

    wchar_t *wchar = (wchar_t *) env->GetStringChars(jstr, NULL);

    return wchar;
}

// char* to jstring
jstring JNU_Chars2Jstring(JNIEnv *env, const char *pat) {
    VarCache *cacheInstance = VarCache::Singleton();
    jclass str_class = cacheInstance->GetClass(env, "java/lang/String");
    jmethodID ctorID = cacheInstance->GetMethodId(env, str_class, "<init>",
                                                  "([BLjava/lang/String;)V");

    jbyteArray bytes;

    if (pat != NULL) {
        bytes = env->NewByteArray((jsize) strlen(pat));
        env->SetByteArrayRegion(bytes, 0, (jsize) strlen(pat), (jbyte *) pat);
    } else {
        bytes = env->NewByteArray(1);
        char ch[1] =
                {0};
        env->SetByteArrayRegion(bytes, 0, 1, (jbyte *) ch);
    }

    jstring encoding = env->NewStringUTF("utf-8");

    jstring jstr = (jstring) env->NewObject(str_class, ctorID, bytes, encoding);
    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(encoding);

    return jstr;
}

