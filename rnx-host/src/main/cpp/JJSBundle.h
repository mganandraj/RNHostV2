#pragma once

#include <fbjni/fbjni.h>
#include "JSBundle.h"

namespace facebook::react {

// Note :: This could be a hybrid object instead. But keeping it simple for now, and copying the object fields across the JNI boundary except the bundle shared buffer
struct JJSBundle : facebook::jni::JavaClass<JJSBundle> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/JSBundle;";
    static std::unique_ptr<IJSBundle> get(facebook::jni::alias_ref<JJSBundle> thizz);
};

struct JJSBundleInfo : facebook::jni::JavaClass<JJSBundleInfo> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/JSBundleInfo;";
    static JSBundleInfo get(facebook::jni::alias_ref<JJSBundleInfo> thizz);
};
} // namespace facebook::react
