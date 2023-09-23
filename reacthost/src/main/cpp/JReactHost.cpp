#include "precomp.h"
#include "JReactHost.h"

#include "JReactViewHost.h"

using namespace facebook::jni;
using namespace Mso::React;

void JReactHost::registerNatives() {
    registerHybrid({
        makeNativeMethod("MakeViewHost", JReactHost::MakeViewHost),
        makeNativeMethod("ReloadInstance", JReactHost::ReloadInstance),
        makeNativeMethod("ReloadInstanceWithOptions", JReactHost::ReloadInstanceWithOptions),
        makeNativeMethod("UnloadInstance", JReactHost::UnloadInstance)
    });
}

alias_ref<JReactViewHost::jhybridobject> JReactHost::MakeViewHost(alias_ref<JReactViewOptions::jhybridobject> jOptions) {
    ReactViewOptions viewOptions = jOptions->cthis()->Options();
    return JReactViewHost::create(host_->MakeViewHost(std::move(viewOptions))).release();
}

void JReactHost::ReloadInstance() {
    host_->ReloadInstance();
}

void JReactHost::ReloadInstanceWithOptions(facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
    auto options = jOptions->cthis()->Options();
    host_->ReloadInstanceWithOptions(std::move(options));
}

void JReactHost::UnloadInstance() {
    host_->UnloadInstance();
}

/*static*/ facebook::jni::local_ref<JReactHost::jhybridobject> JReactHost::create(Mso::CntPtr<Mso::React::IReactHost> host) {
    return newObjectCxxArgs(std::move(host));
}
