#include <jni.h>
#include <string>

#include <fbjni/fbjni.h>

#include "JReactOptions.h"

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
    });
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rnhostv2_12_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    static auto reactOptions = JReactOptions::create();
    // reactOptions->callJava();

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}