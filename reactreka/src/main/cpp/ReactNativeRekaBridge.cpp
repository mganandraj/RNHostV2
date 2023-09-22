#include "precomp.h"

#include "ReactNativeRekaBridge.h"

using namespace facebook::jni;
using namespace facebook::react;

namespace reactreka {

local_ref<RekaNativeToJsBridge::JavaPart>
RekaNativeToJsBridge::create(alias_ref<ReactContextHolder::javaobject> contextHolder) {
    return newObjectJavaArgs(contextHolder);
}

ReactNativeRekaBridge::ReactNativeRekaBridge(
        alias_ref<ReactContextHolder::javaobject> reactContextHolder) noexcept {
    m_jsToNativeBridge = make_global(RekaNativeToJsBridge::create(reactContextHolder));
};

ReactNativeRekaBridge::~ReactNativeRekaBridge() noexcept {};

void ReactNativeRekaBridge::Invoke(const Mso::JSHost::RekaBridgeInvokeArgs &args) noexcept {
    auto arg = folly::dynamic::object
            ("serviceName", args.ServiceName)
            ("methodName", args.MethodName)
            ("value", args.SerializedValue)
            ("promiseId", args.PromiseId);

    std::string methodName("invokeJavaScript");
    local_ref<ReadableNativeMap::jhybridobject> map = ReadableNativeMap::newObjectCxxArgs(
            std::move(arg));
    cthis(m_jsToNativeBridge)->callJSFunction(methodName, map);
}

RekaNativeToJsBridge::RekaNativeToJsBridge(alias_ref<RekaNativeToJsBridge::javaobject> jThis) {
    m_javaPart = make_global(jThis);
}

void RekaNativeToJsBridge::callJSFunction(const std::string &methodName,
                                          local_ref<ReadableNativeMap::jhybridobject> args) {
    static const auto method = javaClassLocal()->getMethod<void(jstring,
                                                                facebook::react::ReadableNativeMap::javaobject)>(
            "CallJSFunction");
    local_ref<JString> jMethodName = make_jstring(methodName);
    method(m_javaPart.get(), jMethodName.get(), args.get());
}

/*static*/ local_ref<RekaNativeToJsBridge::jhybriddata>
RekaNativeToJsBridge::initHybrid(alias_ref<jhybridobject> jThis) {
    return makeCxxInstance(jThis);
}

/*static*/ void RekaNativeToJsBridge::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", RekaNativeToJsBridge::initHybrid),
                   });
}
}