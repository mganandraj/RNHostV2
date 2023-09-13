#include "MainActivity.h"

#include "ReactNativeHost/React.h"
#include "ReactHost.h"

using namespace facebook::jni;
using namespace Mso::React;

/*static */void MainActivity::runReactOnView(facebook::jni::alias_ref<MainActivity> thiz,
                                             facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> viewInstance) {

    ReactOptions options;
    options.Identity = "V2App";

    ReactHostRegistry::OnLibletInit();

    static Mso::CntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));
    ReactViewOptions viewOptions;
    viewOptions.ComponentName = "AwesomeProject";
    static auto viewHost = reactHost->MakeViewHost(std::move(viewOptions));

    JOfficeReactRootView* viewInstanceNative =  viewInstance->cthis();
    viewHost->AttachViewInstance(*viewInstanceNative->m_ReactViewInstance);

    // auto unloadMethod = viewInstance->javaClassStatic()->getMethod<void()>("Unload");
    // unloadMethod(viewInstance);
}

void MainActivity::registerNatives() {
    javaClassStatic()->registerNatives({
        makeNativeMethod("runReactOnView", MainActivity::runReactOnView)
    });
}
