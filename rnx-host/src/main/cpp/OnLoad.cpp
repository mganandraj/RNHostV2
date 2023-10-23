#include "ReactNativeHeaders.h"

#include <fbjni/fbjni.h>
#include <react/jni/JavaScriptExecutorHolder.h>
#include <react/jni/ReadableNativeMap.h>
#include <react/jni/JSLoader.h>
#include "JJSBundle.h"

#include "JJSExecutorObserver.h"
#include "WrapperJSExecutorFactory.h"

#include <vector>

namespace facebook::react {

class WrapperJSExecutorHolder
    : public jni::HybridClass<WrapperJSExecutorHolder, JavaScriptExecutorHolder> {
 public:
  static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/WrapperJSExecutor;";

  static jni::local_ref<jhybriddata> initHybrid(
      jni::alias_ref<jclass>,
              jni::alias_ref<JAssetManager::javaobject> assetManager,
              jni::alias_ref<JavaScriptExecutorHolder::javaobject> baseExecutorFactory,
              jni::alias_ref<jni::JArrayClass<JJSBundle>> platformbundles,
              jni::alias_ref<JJSExecutorObserver::javaobject> observer) {
    auto baseFactory = baseExecutorFactory->cthis()->getExecutorFactory();
    
    std::vector<std::string> preBundlesVector;
    std::vector<std::unique_ptr<IJSBundle>> platformBundlesVector;
    if(platformbundles){
        for (size_t i = 0; i < platformbundles->size(); ++i) {
            platformBundlesVector.push_back(JJSBundle::get(platformbundles->getElement(i)));
        }
    }

    auto factory = std::make_unique<WrapperJSExecutorFactory>(
      jni::make_weak<JAssetManager::javaobject>(assetManager.get()),
              baseFactory,
              std::move(platformBundlesVector),
              jni::make_weak<JJSExecutorObserver::javaobject>(observer.get()));
    return makeCxxInstance(std::move(factory));
  }

  static void registerNatives() {
    registerHybrid({
      makeNativeMethod("initHybrid", WrapperJSExecutorHolder::initHybrid),
    });
  }

 private:
  friend HybridBase;
  using HybridBase::HybridBase;
};

} // namespace facebook::react

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
  return facebook::jni::initialize(vm, [] {
      facebook::react::WrapperJSExecutorHolder::registerNatives();
  });
}