#include "precomp.h"

#include <fbjni/fbjni.h>

#include "JReactOptions.h"
#include "JReactViewOptions.h"
#include "ReactInstanceAndroid.h"
#include "JReactViewInstance.h"
#include "JReactHost.h"
#include "JReactHostStatics.h"
#include "JReactViewHost.h"
#include "ReactHost.h"

using namespace Mso::React;

extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
        JReactDevOptions::registerNatives();
        JReactViewOptions::registerNatives();
        JReactHost::registerNatives();
        JReactHostStatics::registerNatives();
        JReactInstance::registerNatives();
        JReactViewHost::registerNatives();
        JOfficeReactRootView::registerNatives();
    });
}
