#include <jni.h>
#include <string>

#include <fbjni/fbjni.h>

#include "JReactOptions.h"
#include <ReactNativeHost/React.h>

#include "ReactHost.h"

using namespace Mso::React;

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
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

    // static auto reactOptions = JReactOptions::create(std::move(options));
    // reactOptions->callJava();

    // std::string hello = "Hello from C++";
    // return env->NewStringUTF(hello.c_str());

    ReactOptions options2;
    return JReactOptions::create(std::move(options2)).release();
}