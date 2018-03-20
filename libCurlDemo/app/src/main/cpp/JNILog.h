//
// Created by juzenhon on 2017/10/17.
//

#ifndef TOONANDROID_JNILOG_H
#define TOONANDROID_JNILOG_H

#include <android/log.h>
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"JNI_native-ralph",__VA_ARGS__)

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "JNI_native-ralph",__VA_ARGS__)
#endif //TOONANDROID_JNILOG_H
