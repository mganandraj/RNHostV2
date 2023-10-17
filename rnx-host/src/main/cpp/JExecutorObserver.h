#pragma once 

#include <fbjni/fbjni.h>

namespace facebook {
namespace react {

struct JExecutorObserver : facebook::jni::JavaClass<JExecutorObserver> {
  static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/ExecutorObserver;";

  void OnInitialized();
  void OnLoaded(const std::string& bundleUrl);
  
};

}}