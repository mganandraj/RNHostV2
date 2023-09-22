#pragma once

#include "ReactNativeHeaders.h"

#include <fbjni/fbjni.h>
#include <reka/Reka.h>
#include <react/jni/WritableNativeMap.h>
#include <react/jni/ReadableNativeMap.h>

#include "ReactContextHolder.h"
#include "JRekaBridgeOptions.h"

namespace reactreka {

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

}