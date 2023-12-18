#include "precomp.h"
#include <fbjni/fbjni.h>

#include "MainActivity.h"
#include "ORNHostKtNativeOptionsFragment.h"
#include "ORNHostHeadlessFragment.h"

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        MainActivity::registerNatives();
        ORNHostKtNativeOptionsFragment::registerNatives();
        ORNHostHeadlessFragment::registerNatives();
    });
}
