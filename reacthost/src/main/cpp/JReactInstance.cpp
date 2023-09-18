#include "JReactInstance.h"
#include "ReactInstanceAndroid.h"

#include <ReactCommon/RuntimeExecutor.h>
#include <react/jni/JRuntimeExecutor.h>

/*static */facebook::jni::local_ref<JReactInstance::jhybriddata> JReactInstance::initHybrid(facebook::jni::alias_ref<jhybridobject> jThis) {
    return makeCxxInstance();
}

/*static */void JReactInstance::onInitialized(facebook::jni::alias_ref<jhybridobject> jThis) {
    jThis->cthis()->m_wNativeInstance.GetStrongPtr()->onInitialized();
}

/*static */void JReactInstance::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JReactInstance::initHybrid),
                           makeNativeMethod("onInitialized", JReactInstance::onInitialized),
                   });
}

/*static*/ facebook::jni::local_ref<JReactInstance::jhybridobject> JReactInstance::create(facebook::jni::alias_ref<JReactOptions::jhybridobject> options, Mso::CntPtr<ReactInstanceAndroid> nativeInstance) {
    auto jInstance = JReactInstance::newObjectJavaArgs(options);
    jInstance->cthis()->m_wNativeInstance=Mso::WeakPtr(nativeInstance);
    return jInstance;
}

/*static */facebook::react::RuntimeExecutor JReactInstance::GetRuntimeExecutor(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
    auto clazz = javaClassLocal();
    auto getRuntimeExecutorMethod = clazz->getMethod<facebook::react::JRuntimeExecutor::jhybridobject ()>("getRuntimeExecutor");
    alias_ref<facebook::react::JRuntimeExecutor::jhybridobject> jRuntimeExecutor = getRuntimeExecutorMethod(instance);
    return jRuntimeExecutor->cthis()->get();
}
