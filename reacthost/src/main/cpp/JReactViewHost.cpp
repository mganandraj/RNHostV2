#include "precomp.h"

#include "JReactViewHost.h"

void JReactViewHost::registerNatives() {
    registerHybrid({
        makeNativeMethod("AttachViewInstance", JReactViewHost::AttachViewInstance)
    });
}

void JReactViewHost::AttachViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jView) {
    viewHost_->AttachViewInstance(jView->cthis()->ViewInstance());
}

/*static*/ facebook::jni::local_ref<JReactViewHost::jhybridobject> JReactViewHost::create(Mso::CntPtr<Mso::React::IReactViewHost> viewHost) {
    return newObjectCxxArgs(std::move(viewHost));
}
