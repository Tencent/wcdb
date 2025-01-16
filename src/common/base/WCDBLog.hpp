/**
 * WCDBLog.hpp
 */

#ifndef WCDB2_ANDROID_LOG_H
#define WCDB2_ANDROID_LOG_H


#ifdef __ANDROID__
#include <android/log.h>
#endif

#define TAG "WCDB2_DEBUG"

#ifdef __ANDROID__
#define WCDB2_LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define WCDB2_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define WCDB2_LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#else
#define WCDB2_LOGI(...)  {fprintf(stderr, __VA_ARGS__);fflush(stderr);}
#define WCDB2_LOGD(...)  {fprintf(stderr, __VA_ARGS__);fflush(stderr);}
#define WCDB2_LOGE(...)  {fprintf(stderr, __VA_ARGS__);fflush(stderr);}
#endif

#endif //WCDB2_ANDROID_LOG_H
