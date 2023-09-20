#include "precomp.h"

#include "GetRuntimeExecutor_android.h"
#include "ReactInstanceAndroid.h"

namespace Mso::React {

// TODO :: Handle exceptions
facebook::react::RuntimeExecutor GetRuntimeExecutor2(Mso::React::IReactInstance &instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_jReactInstance;
    return JReactInstance::GetRuntimeExecutor(jReactInstance);
}

}