#include <jni.h>
#include <string>

#include <fbjni/fbjni.h>

#include "JReactOptions.h"
#include <ReactNativeHost/React.h>

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
    });
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_example_rnhostv2_12_MainActivity_startNative(
        JNIEnv* env,
        jobject /* this */) {

    Mso::React::ReactOptions options;
    options.Identity = "V2App";

    // static auto reactOptions = JReactOptions::create(std::move(options));
    // reactOptions->callJava();

    // std::string hello = "Hello from C++";
    // return env->NewStringUTF(hello.c_str());

    return JReactOptions::create(std::move(options)).release();
}