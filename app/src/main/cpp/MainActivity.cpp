#include "precomp.h"
#include "MainActivity.h"

#include "ReactNativeHost/React.h"
#include "ReactHost.h"

#include <jshostheadless/JSHeadlessRuntime.h>

#include <android/log.h>

#include <crash/verifyElseCrash.h>

#include "AwesomeRekaService.h"
#include <Reka/RekaServiceRegistration.h>

#define LOG_TAG "ReactHost::MainActivity"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)

using namespace facebook::jni;
using namespace Mso::React;

Mso::JSHost::DataServiceRegistration<AwesomeRekaService> registration;

///*static */void MainActivity::test(facebook::jni::alias_ref<MainActivity> thiz, facebook::jni::alias_ref<JInstanceCreatedCallback> callback) {
//    // LOGE("MainActivity::runReactOnView::OnInstanceCreated");
//    // JInstanceCreatedCallback::run(callback);
//}

/*static*/ void MainActivity::testHeadless(facebook::jni::alias_ref<MainActivity> thiz) {

    Mso::JSHost::Headless::JSRuntimeOptions options;
    auto runtime = Mso::JSHost::Headless::CreateJSRuntime(std::move(options));
    runtime->Post([](facebook::jsi::Runtime& jsiRuntime){
       auto global = jsiRuntime.global();
    });
}

/*static */void MainActivity::libletInit(facebook::jni::alias_ref<MainActivity> thiz) {
    ReactHostRegistry::OnLibletInit();
}

/*static */void MainActivity::runReactOnView(facebook::jni::alias_ref<MainActivity> thiz,
                                             facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> viewInstance) {

    ReactOptions options;
    options.Identity = "V2App";
    options.OnInstanceCreated = [](IReactInstance& instance){
        LOGE("MainActivity::runReactOnView::OnInstanceCreated");
    };
    // options.JavaModuleNames.push_back("com.microsoft.office.reactreka.RekaReactPackage");
    options.DataServiceProviderName = "reacthostapp::NativeService";

    ReactHostRegistry::OnLibletInit();

    static Mso::CntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));
    ReactViewOptions viewOptions;
    viewOptions.ComponentName = "AwesomeProject";
    static auto viewHost = reactHost->MakeViewHost(std::move(viewOptions));

    JOfficeReactRootView* viewInstanceNative =  viewInstance->cthis();
    viewHost->AttachViewInstance(*viewInstanceNative->m_ReactViewInstance);

    // auto unloadMethod = viewInstance->javaClassStatic()->getMethod<void()>("Unload");
    // unloadMethod(viewInstance);
}

void MainActivity::registerNatives() {
    javaClassStatic()->registerNatives({
        makeNativeMethod("runReactOnView", MainActivity::runReactOnView),
        makeNativeMethod("libletInit", MainActivity::libletInit),
        makeNativeMethod("testHeadless", MainActivity::testHeadless),
        // makeNativeMethod("test", MainActivity::test)
    });
}
