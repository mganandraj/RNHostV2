#pragma once

#include <ReactNativeHost/React.h>
#include <fbjni/fbjni.h>

namespace facebook {
    namespace jsi {
        class Runtime;
    }
    namespace react {
        class CallInvoker;
    }
}

namespace Mso::React {
    LIBLET_PUBLICAPI facebook::jsi::Runtime* GetJsiRuntime(IReactInstance& instance) noexcept;
    LIBLET_PUBLICAPI std::shared_ptr<facebook::react::CallInvoker> GetJSThreadCallInvoker(IReactInstance& instance) noexcept;
    LIBLET_PUBLICAPI Mso::TCntPtr<Mso::React::IReactViewInstance> GetReactViewInstanceFromView(jobject view);

    struct JReactOptions_ : facebook::jni::HybridClass<JReactOptions_> {
        static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactOptions;";
    };
    LIBLET_PUBLICAPI JReactOptions_::JavaPart::javaobject ToJReactOptions(ReactOptions&& options);
    LIBLET_PUBLICAPI ReactOptions ToReactOptions(JReactOptions_::JavaPart::javaobject);

    struct JReactHost_ : facebook::jni::HybridClass<JReactHost_> {
        static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHost;";
    };
    LIBLET_PUBLICAPI JReactHost_::JavaPart::javaobject ToJReactHost(Mso::TCntPtr<IReactHost> reactHost);

}
