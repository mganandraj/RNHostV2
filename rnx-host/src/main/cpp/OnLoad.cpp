#include "ReactNativeHeaders.h"

#include <fbjni/fbjni.h>
#include <react/jni/JavaScriptExecutorHolder.h>
#include <react/jni/ReadableNativeMap.h>
#include <react/jni/JSLoader.h>
#include "JJSBundle.h"

#include "JOfficeExecutorObserver.h"
#include "OfficeExecutorFactory.h"

#include <vector>

namespace facebook {
namespace react {

class OfficeExecutorHolder
    : public jni::HybridClass<OfficeExecutorHolder, JavaScriptExecutorHolder> {
 public:
  static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/OfficeExecutor;";

  static jni::local_ref<jhybriddata> initHybrid(
      jni::alias_ref<jclass>
      , jni::alias_ref<JAssetManager::javaobject> assetManager
      , jni::alias_ref<JavaScriptExecutorHolder::javaobject> baseExecutorFactory
      , jni::alias_ref<jni::JArrayClass<jni::JString>> prebundles
      , jni::alias_ref<jni::JArrayClass<JJSBundle>> platformbundles
      , jni::alias_ref<JOfficeExecutorObserver::javaobject> observer) {
    auto baseFactory = baseExecutorFactory->cthis()->getExecutorFactory();
    
    std::vector<std::string> preBundlesVector;
    for (size_t i = 0; i < prebundles->size(); ++i) {
      preBundlesVector.push_back(prebundles->getElement(i)->toStdString());
    }

    std::vector<std::unique_ptr<IJSBundle>> platformBundlesVector;
    for (size_t i = 0; i < platformbundles->size(); ++i) {
      platformBundlesVector.push_back(JJSBundle::get(platformbundles->getElement(i)));
    }

    auto factory = std::make_unique<OfficeExecutorFactory>(
      jni::make_weak<JAssetManager::javaobject>(assetManager.get())
      , baseFactory
      , std::move(preBundlesVector)
      , std::move(platformBundlesVector)
      , jni::make_weak<JOfficeExecutorObserver::javaobject>(observer.get()));
    return makeCxxInstance(std::move(factory));
  }

  static void registerNatives() {
    registerHybrid({
      makeNativeMethod("initHybrid", OfficeExecutorHolder::initHybrid),
    });
  }

 private:
  friend HybridBase;
  using HybridBase::HybridBase;
};

} // namespace react
} // namespace facebook

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
  return facebook::jni::initialize(vm, [] {
      facebook::react::OfficeExecutorHolder::registerNatives();
  });
}