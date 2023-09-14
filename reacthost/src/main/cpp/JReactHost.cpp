#include "JReactHost.h"

#include "JReactViewHost.h"

using namespace facebook::jni;
using namespace Mso::React;

void JReactHost::registerNatives() {
    registerHybrid({
        makeNativeMethod("MakeViewHost", JReactHost::MakeViewHost)
    });
}

alias_ref<JReactViewHost::jhybridobject> JReactHost::MakeViewHost(alias_ref<JReactViewOptions::jhybridobject> jOptions) {
    ReactViewOptions viewOptions = jOptions->cthis()->Options();
    return JReactViewHost::create(host_->MakeViewHost(std::move(viewOptions))).release();
}

/*static*/ facebook::jni::local_ref<JReactHost::jhybridobject> JReactHost::create(Mso::CntPtr<Mso::React::IReactHost> host) {
    return newObjectCxxArgs(std::move(host));
}
