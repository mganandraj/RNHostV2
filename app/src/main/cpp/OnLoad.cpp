#include <fbjni/fbjni.h>

#include "MainActivity.h"

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        MainActivity::registerNatives();
    });
}
