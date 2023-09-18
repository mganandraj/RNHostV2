#ifndef ORNHOST_JREACTINSTANCE_H
#define ORNHOST_JREACTINSTANCE_H

#include <fbjni/fbjni.h>
#include <ReactNativeHost/React.h>
#include "JReactOptions.h"

#include <ReactCommon/RuntimeExecutor.h>

using namespace facebook::jni;
using namespace Mso::React;

namespace Mso::React {
class ReactInstanceAndroid;
}

struct JReactInstance  : facebook::jni::HybridClass<JReactInstance> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactInstance;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static void onInitialized(facebook::jni::alias_ref<jhybridobject> jThis);

    static facebook::jni::local_ref<jhybridobject> create(facebook::jni::alias_ref<JReactOptions::jhybridobject>, Mso::CntPtr<ReactInstanceAndroid> nativeInstance);

    static facebook::react::RuntimeExecutor GetRuntimeExecutor(facebook::jni::alias_ref<JReactInstance::javaobject>);

    Mso::WeakPtr<ReactInstanceAndroid> m_wNativeInstance;
};

#endif //ORNHOST_JREACTINSTANCE_H
