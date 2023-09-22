#include "precomp.h"
#include "RekaNativeModuleProxy.h"
#include "ReactNativeRekaBridge.h"

using namespace facebook::jni;
using namespace facebook::react;

namespace reactreka {

/*static */ local_ref<RekaNativeModuleProxy::jhybriddata>
RekaNativeModuleProxy::initHybrid(alias_ref<jclass>,
                                  alias_ref<ReactContextHolder::javaobject> reactContextHolder,
                                  facebook::jni::alias_ref<JRekaBridgeOptions::javaobject> rekaBridgeOptions) {
    return makeCxxInstance(reactContextHolder, rekaBridgeOptions);
}

/*static*/ void RekaNativeModuleProxy::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", RekaNativeModuleProxy::initHybrid),
                           makeNativeMethod("getConstantsImpl",
                                            RekaNativeModuleProxy::getConstantsImpl),
                           makeNativeMethod("invokeNativeImpl",
                                            RekaNativeModuleProxy::invokeNativeImpl)
                   });
}

RekaNativeModuleProxy::RekaNativeModuleProxy(
        alias_ref<ReactContextHolder::javaobject> reactContextHolder,
        facebook::jni::alias_ref<JRekaBridgeOptions::javaobject> rekaBridgeOptions) {
    if (!rekaBridgeOptions) {
        Mso::JSHost::RekaBridgeOptions basicRekaBridgeOptions{};
        std::string dataServiceProviderName(Mso::JSHost::DefaultDataServiceProviderName());
        basicRekaBridgeOptions.RekaServiceProviderFactory =
                [dataServiceProviderName = std::move(dataServiceProviderName)](
                        Mso::JSHost::IRekaContext &context) mutable noexcept {
                    return Mso::JSHost::GetOrMakeRekaServiceProvider(
                            dataServiceProviderName.c_str(), context);
                };

        basicRekaBridgeOptions.OnInitialized = [](Mso::JSHost::IRekaContext &) noexcept {};

        basicRekaBridgeOptions.JsExecutor = Mso::Make<ReactNativeRekaBridge, Mso::JSHost::IRekaBridge>(
                reactContextHolder);
        m_rekaInstance = Mso::JSHost::MakeRekaInstance(std::move(basicRekaBridgeOptions));
    } else {
        Mso::JSHost::RekaBridgeOptions rekaOptions = std::move(rekaBridgeOptions->cthis()->rekaOptions_);
        rekaOptions.JsExecutor = Mso::Make<ReactNativeRekaBridge, Mso::JSHost::IRekaBridge>(
                reactContextHolder);
        m_rekaInstance = Mso::JSHost::MakeRekaInstance(std::move(rekaOptions));
    }
}

local_ref<ReadableNativeMap::jhybridobject> RekaNativeModuleProxy::getConstantsImpl() {
    folly::dynamic constantValues = folly::dynamic::object();
    m_rekaInstance->EnumerateConstants(
            [&constantValues](const char *serviceName, const char *propertyName,
                              const char *serializedJson) noexcept {
                constantValues
                        .setDefault(serviceName, folly::dynamic::object())
                        .setDefault(propertyName, serializedJson);
            });

    return ReadableNativeMap::newObjectCxxArgs(std::move(constantValues));
}

void RekaNativeModuleProxy::invokeNativeImpl(ReadableNativeMap *args) {
    folly::dynamic argsdyn = args->consume();

    Mso::JSHost::RekaBridgeInvokeArgs rekaBridgeInvokeArgs = {
            argsdyn["serviceName"].getString().c_str(),
            argsdyn["methodName"].getString().c_str(),
            argsdyn["value"].getString().c_str(),
            static_cast<int32_t>(argsdyn["promiseId"].asInt())};
    m_rekaInstance->Invoke(std::move(rekaBridgeInvokeArgs));
}

}