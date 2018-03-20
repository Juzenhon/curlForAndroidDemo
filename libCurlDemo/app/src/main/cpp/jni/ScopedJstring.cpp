
#include "ScopedJstring.h"
#include "JNILog.h"
#include <string.h>
#include <malloc.h>

ScopedJstring::ScopedJstring(JNIEnv *env, jstring jstr)
        : _env(env), _jstr(0), _char(0), jstr2char_(true) {

    if (env && jstr) {
        if (!_env->ExceptionOccurred()) {

            _jstr = (jstring) _env->NewLocalRef(jstr);
            if (_jstr) {
                jclass stringClass = env->GetObjectClass(jstr);
                jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "()[B");
                jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jstr, getBytes);

                jsize length = env->GetArrayLength(stringJbytes);
                jbyte *pBytes = env->GetByteArrayElements(stringJbytes, 0);
                _char = (char *) malloc(length + 1);
                if(_char){
                    _char[length] = '\0';
                    memcpy(_char, pBytes, length);
                }
                env->ReleaseByteArrayElements(stringJbytes, pBytes, 0);
            } else {
                LOGD("NewLocalRef fail!");
            }
        }
    } else {
        if (jstr == 0) {
            LOGD("ScopeJstring: jstring is 0!!!!");
        } else {
            LOGD("ScopeJstring: env is 0!!!!");
        }
    }

}

ScopedJstring::ScopedJstring(JNIEnv *env, const char *str)
        : _env(env), _jstr(0), _char(0), jstr2char_(false) {

    size_t len = 0;
    if (_env && str && (len = strlen(str))) {

        if (!_env->ExceptionOccurred()) {

            _char = const_cast<char *>(str);
            jclass strClass = _env->FindClass("java/lang/String");
            jmethodID ctorID = _env->GetMethodID(strClass, "<init>", "([B)V");

            jbyteArray bytes = _env->NewByteArray((jsize) len);
            _env->SetByteArrayRegion(bytes, 0, (jsize) len, (jbyte *) _char);
//            jstring encoding = _env->NewStringUTF("utf-8");

            _jstr = (jstring) _env->NewObject(strClass, ctorID, bytes/*, encoding*/);

            _env->DeleteLocalRef(bytes);
//            _env->DeleteLocalRef(encoding);
            _env->DeleteLocalRef(strClass);
        }
    }
}

ScopedJstring::~ScopedJstring() {
    if (_env && !_env->ExceptionOccurred()) {
        if (jstr2char_) {
            if (_jstr) {
                _env->DeleteLocalRef(_jstr);
            }
            if (_char) {
                free(_char);
                _char = 0;
            }
        }

    }
}

const char *ScopedJstring::GetChar() const {
    if (_env->ExceptionOccurred()) {
        return 0;
    }

    return _char ? _char : "";
}

jstring ScopedJstring::GetJstr() const {
    if (_env->ExceptionOccurred()) {
        return 0;
    }

    return _jstr;
}

