#include "precomp.h"

#include "ReactNativeHost/React_Android.h"
#include "ReactInstanceAndroid.h"

namespace Mso::React {
facebook::jsi::Runtime* GetJsiRuntime(IReactInstance& instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_jReactInstance;
    return JReactInstance::GetJsiRuntime(jReactInstance);
}

std::shared_ptr<facebook::react::CallInvoker> GetJSThreadCallInvoker(IReactInstance& instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_jReactInstance;
    return JReactInstance::getJSCallInvokerHolder(jReactInstance);
}
}
