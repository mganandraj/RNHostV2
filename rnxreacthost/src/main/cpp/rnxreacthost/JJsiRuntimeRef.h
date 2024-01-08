#pragma once

#include <fbjni/fbjni.h>
#include <jsi/jsi.h>

namespace facebook::react {

struct JJsiRuntimeRef : public facebook::jni::HybridClass<JJsiRuntimeRef> {
public:
    static auto constexpr kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/JsiRuntimeRef;";
    facebook::jsi::Runtime& get() { return runtime_; }
    static facebook::jni::local_ref<JJsiRuntimeRef::jhybridobject> create(facebook::jsi::Runtime &runtime);

private:
    friend HybridBase;
    JJsiRuntimeRef(facebook::jsi::Runtime &runtime) : runtime_(runtime) {}
    facebook::jsi::Runtime& runtime_;
};

} // namespace facebook::react