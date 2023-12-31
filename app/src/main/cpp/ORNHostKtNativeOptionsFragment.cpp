#include "precomp.h"
#include "ORNHostKtNativeOptionsFragment.h"

#include <android/log.h>

#include <jsi/jsi.h>

#define LOG_TAG "ORNHostKtNativeOptionsFragment"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)

void ORNHostKtNativeOptionsFragment::registerNatives() {
    javaClassStatic()->registerNatives({
                                               makeNativeMethod("getReactOptions", ORNHostKtNativeOptionsFragment::getReactOptions),
                                      });
}

/*static*/ facebook::jni::local_ref<Mso::React::JReactOptions_::javaobject> ORNHostKtNativeOptionsFragment::getReactOptions(facebook::jni::alias_ref<ORNHostKtNativeOptionsFragment> thiz) {

    Mso::React::ReactOptions options;
    options.Identity = "V2App";
    options.DeveloperSettings.IsDevModeEnabled = true;

    options.DataServiceProviderName = "reacthostapp::NativeService";

    options.OnInstanceCreated = [](Mso::React::IReactInstance& instance){
        LOGE("ORNHostKtNativeOptionsFragment::OnInstanceCreated");
    };
    options.OnInstanceLoaded = [](Mso::React::IReactInstance& instance, const Mso::ErrorCode&) {
        LOGE("ORNHostKtNativeOptionsFragment::OnInstanceCreated");
    };

    Mso::React::RuntimeInstallerHolder runtimeInstallerHolder;
    runtimeInstallerHolder.runtimeInstaller = [](facebook::jsi::Runtime &runtime){
        runtime.global().setProperty(runtime, "Home", "Marangattu" );
    };
    options.Properties.Set(Mso::React::RuntimeInstallerProperty, runtimeInstallerHolder);

    options.JavaModuleNames.push_back("com.microsoft.office.reacthostapp.MyReactPackage");
    // On Android, FileJSBundle is mapped to AssetJSBundle
    // options.AddFileJSBundle("index.android.bundle", "index.android.bundle");
    // options.AddRegisteredJSBundle("index.android.bundle");
    options.AddFileJSBundle("index.android.bundle", "index.android.bundle");

    // options.JavaModuleNames.push_back("com.microsoft.office.reactreka.RekaReactPackage");
    // options.DataServiceProviderName = "reacthostapp::NativeService";
    // options.DeveloperSettings.IsDevModeEnabled=true;
    // options.DeveloperSettings.SourceBundleName="index";

    return facebook::jni::make_local(Mso::React::ToJReactOptions(std::move(options)));
}
