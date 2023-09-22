#include "precomp.h"
#include "AwesomeRekaService.h"

#include "reka/AwesomeRekaDefs.impl.g.h"

#define LOG_TAG "AwesomeRekaService"

#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)


/*virtual*/ void AwesomeRekaService::tryCall(std::string&& input1, int32_t&& input2) noexcept {
    LOGI("AwesomeRekaService::tryCall called with %s, %d", input1.c_str(), input2);
}


