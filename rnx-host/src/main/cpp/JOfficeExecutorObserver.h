#pragma once 

#include <fbjni/fbjni.h>

namespace facebook {
namespace react {

struct JOfficeExecutorObserver : facebook::jni::JavaClass<JOfficeExecutorObserver> {
  static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/jsexecutor/OfficeExecutorObserver;";

  void OnInitialized();
  void OnLoaded(const std::string& bundleUrl);
  
};

}}