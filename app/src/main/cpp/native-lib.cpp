#include <jni.h>
#include <string>

#include <fbjni/fbjni.h>

#include "JReactOptions.h"
#include "ReactInstanceAndroid.h"
#include "MainActivity.h"
#include "JReactViewInstance.h"

#include <ReactNativeHost/React.h>

#include "ReactHost.h"

using namespace Mso::React;

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
        JReactInstance::registerNatives();
        MainActivity::registerNatives();
        JReactViewInstance::registerNatives();
    });
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_rnhostv2_12_MainActivity_startNative(
        JNIEnv* env,
        jobject /* this */) {

    ReactOptions options;
    options.Identity = "V2App";


    ReactHostRegistry::OnLibletInit();

    static Mso::CntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));

    // auto jOptions = make_global(JReactOptions::create(std::move(options)));
    // auto instance = make_global(JReactInstance::create(jOptions));


    // static auto reactOptions = JReactOptions::create(std::move(options));
    // reactOptions->callJava();

    // std::string hello = "Hello from C++";
    // return env->NewStringUTF(hello.c_str());

    ReactOptions options2;
    return JReactOptions::create(std::move(options2)).release();
}