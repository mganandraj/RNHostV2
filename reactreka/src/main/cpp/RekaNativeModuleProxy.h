#pragma once

#include "ReactNativeHeaders.h"

#include <fbjni/fbjni.h>
#include <reka/Reka.h>
#include <react/jni/WritableNativeMap.h>
#include <react/jni/ReadableNativeMap.h>

#include "JRekaBridgeOptions.h"

namespace reactreka {

struct ReactContextHolder : public facebook::jni::JavaClass<ReactContextHolder> {
    constexpr static auto kJavaDescriptor =
            "Lcom/microsoft/office/reactreka/ReactContextHolder;";
};

class RekaNativeToJsBridge : public facebook::jni::HybridClass<RekaNativeToJsBridge> {
public:
    static constexpr const char *const kJavaDescriptor =
            "Lcom/microsoft/office/reactreka/RekaNativeToJsBridge;";

    static facebook::jni::local_ref<jhybriddata>
    initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);

    static void registerNatives();

    static facebook::jni::local_ref<JavaPart>
    create(facebook::jni::alias_ref<reactreka::ReactContextHolder::javaobject>);

    RekaNativeToJsBridge(facebook::jni::alias_ref<RekaNativeToJsBridge::javaobject> jThis);

    void callJSFunction(const std::string &methodName,
                        facebook::jni::local_ref<facebook::react::ReadableNativeMap::jhybridobject>);

private:
    facebook::jni::global_ref<RekaNativeToJsBridge::javaobject> m_javaPart;
};

class RekaNativeModuleProxy : public facebook::jni::HybridClass<RekaNativeModuleProxy> {
public:
    static constexpr const char *const kJavaDescriptor =
            "Lcom/microsoft/office/reactreka/RekaNativeModule;";

    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jclass>,
                                                            facebook::jni::alias_ref<ReactContextHolder::javaobject>,
                                                            facebook::jni::alias_ref<JRekaBridgeOptions::javaobject>);

    static void registerNatives();

    RekaNativeModuleProxy(
            facebook::jni::alias_ref<ReactContextHolder::javaobject> reactContextHolder,
            facebook::jni::alias_ref<JRekaBridgeOptions::javaobject> rekaBridgeOptions);

    void invokeNativeImpl(facebook::react::ReadableNativeMap *);

    facebook::jni::local_ref<facebook::react::ReadableNativeMap::jhybridobject>
    getConstantsImpl();

private:
    Mso::TCntPtr<Mso::JSHost::IRekaInstance> m_rekaInstance;
};

class ReactNativeRekaBridge : public Mso::UnknownObject<Mso::JSHost::IRekaBridge> {
public:
    void Invoke(const Mso::JSHost::RekaBridgeInvokeArgs &args) noexcept override;

private:
    friend MakePolicy; // To use private constructor
    ReactNativeRekaBridge(
            facebook::jni::alias_ref<ReactContextHolder::javaobject> reactContextHolder) noexcept;

    ~ReactNativeRekaBridge() noexcept override;

    facebook::jni::global_ref<RekaNativeToJsBridge::JavaPart> m_jsToNativeBridge;
};

}