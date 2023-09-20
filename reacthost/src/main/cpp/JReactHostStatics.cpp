#include "precomp.h"
#include "JReactHostStatics.h"

#include "JReactHost.h"

using namespace facebook::jni;
using namespace Mso::React;

/*static*/ facebook::jni::alias_ref<JReactHost::jhybridobject> JReactHostStatics::makeJReactHost(alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
    auto options = jOptions->cthis()->Options();
    Mso::CntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));
    return JReactHost::create(reactHost).release();
}

/*static*/ void JReactHostStatics::registerNatives() {
    javaClassLocal()->registerNatives({
           makeNativeMethod("makeReactHost", JReactHostStatics::makeJReactHost),
    });
}