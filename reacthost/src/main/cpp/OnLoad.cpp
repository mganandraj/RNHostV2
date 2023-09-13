#include <fbjni/fbjni.h>

#include "JReactOptions.h"
#include "JReactViewOptions.h"
#include "ReactInstanceAndroid.h"
#include "JReactViewInstance.h"
#include "ReactHost.h"

using namespace Mso::React;

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
        JReactViewOptions::registerNatives();
        JReactInstance::registerNatives();
        JOfficeReactRootView::registerNatives();
    });
}
