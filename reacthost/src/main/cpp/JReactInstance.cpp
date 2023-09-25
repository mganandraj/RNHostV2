#include "precomp.h"
#include "JReactInstance.h"
#include "ReactInstanceAndroid.h"

#include <ReactCommon/RuntimeExecutor.h>
#include <react/jni/JRuntimeExecutor.h>
#include <ReactCommon/CallInvokerHolder.h>

/*static */facebook::jni::local_ref<JReactInstance::jhybriddata> JReactInstance::initHybrid(facebook::jni::alias_ref<jhybridobject> jThis) {
    return makeCxxInstance();
}

/*static */void JReactInstance::onInitialized(facebook::jni::alias_ref<jhybridobject> jThis, facebook::jni::alias_ref<JReactContextHolder> contextHolder) {
    jThis->cthis()->m_wNativeInstance.GetStrongPtr()->onInitialized(contextHolder);
}

/*static*/ void JReactInstance::onBundleLoaded(facebook::jni::alias_ref<jhybridobject> jThis, facebook::jni::alias_ref<JString> bundleName) {
    jThis->cthis()->m_wNativeInstance.GetStrongPtr()->onBundleLoaded(bundleName->toStdString());
}

/*static */facebook::jni::local_ref<reactreka::JRekaBridgeOptions::jhybridobject> JReactInstance::createRekaBridgeOptions(facebook::jni::alias_ref<jhybridobject> jThis) {
    return reactreka::JRekaBridgeOptions::create(jThis->cthis()->m_wNativeInstance.GetStrongPtr()->createRekaBridgeOptions());
}

/*static */void JReactInstance::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JReactInstance::initHybrid),
                           makeNativeMethod("onInitialized", JReactInstance::onInitialized),
                           makeNativeMethod("onBundleLoaded", JReactInstance::onBundleLoaded),
                           makeNativeMethod("createRekaBridgeOptions", JReactInstance::createRekaBridgeOptions),
                   });
}

/*static*/ facebook::jni::local_ref<JReactInstance::jhybridobject> JReactInstance::create(facebook::jni::alias_ref<JReactOptions::jhybridobject> options, Mso::CntPtr<ReactInstanceAndroid> nativeInstance) {

    // Create the java peer.
    auto jInstance = JReactInstance::newObjectJavaArgs(options);

    // Associate the native peer with the native instance.
    jInstance->cthis()->m_wNativeInstance=Mso::WeakPtr(nativeInstance);

    // Initialize the instance
    auto clazz = javaClassLocal();
    auto initializeMethod = clazz->getMethod<void()>("initialize");
    initializeMethod(jInstance);

    return jInstance;
}

// TODO:: Null check
/*static */facebook::react::RuntimeExecutor JReactInstance::GetRuntimeExecutor(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
    auto getRuntimeExecutorMethod = javaClassLocal()->getMethod<facebook::react::JRuntimeExecutor::jhybridobject ()>("getRuntimeExecutor");
    local_ref<facebook::react::JRuntimeExecutor::jhybridobject> jRuntimeExecutor = make_local(getRuntimeExecutorMethod(instance));
    return jRuntimeExecutor->cthis()->get();
}

// TODO:: Null check
/*static */facebook::jsi::Runtime* JReactInstance::GetJsiRuntime(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
    auto getJsiRuntimeMethod = javaClassLocal()->getMethod<jlong()>("getJsiRuntimeRef");
    long jsiRuntimeRef = getJsiRuntimeMethod(instance);
    return reinterpret_cast<facebook::jsi::Runtime*>(jsiRuntimeRef);
}

// TODO:: Null check`
/*static */std::shared_ptr<facebook::react::CallInvoker> JReactInstance::getJSCallInvokerHolder(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
    auto getJSCallInvokerHolderMethod = javaClassLocal()->getMethod<facebook::react::CallInvokerHolder::jhybridobject()>("getJSCallInvokerHolder");
    auto jsCallInvokerHolder = getJSCallInvokerHolderMethod(instance);
    return jsCallInvokerHolder->cthis()->getCallInvoker();
}
