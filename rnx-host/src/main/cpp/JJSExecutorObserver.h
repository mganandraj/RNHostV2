#pragma once 

#include <fbjni/fbjni.h>

namespace facebook::react {

struct JJSExecutorObserver : facebook::jni::JavaClass<JJSExecutorObserver> {
  static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/JSExecutorObserver;";
  void OnInitialized();
  void OnLoaded(const std::string& bundleUrl);
};
} // namespace facebook::react