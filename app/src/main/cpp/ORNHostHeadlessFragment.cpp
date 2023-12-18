#include "precomp.h"
#include "ORNHostHeadlessFragment.h"

#include <jshostheadless/JSHeadlessRuntime.h>

#include <android/log.h>

#define LOG_TAG "ORNHostHeadlessFragment"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)

void ORNHostHeadlessFragment::registerNatives() {
    javaClassStatic()->registerNatives({
               makeNativeMethod("runNative", ORNHostHeadlessFragment::runNative)
    });
}

/*static*/ void ORNHostHeadlessFragment::runNative(facebook::jni::alias_ref<ORNHostHeadlessFragment> thiz) {
    Mso::JSHost::Headless::JSRuntimeOptions options;
    options.DeveloperSettings.IsDevModeEnabled = true;

    auto runtime = Mso::JSHost::Headless::CreateJSRuntime(std::move(options));
    runtime->Post([runtime](facebook::jsi::Runtime& jsiRuntime){
        auto global = jsiRuntime.global();
    });
}
