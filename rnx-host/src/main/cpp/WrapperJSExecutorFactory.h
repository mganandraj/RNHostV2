#pragma once 

#include <cxxreact/JSExecutor.h>
#include <vector>
#include <fbjni/fbjni.h>
#include <react/jni/JSLoader.h>
#include "JJSBundle.h"

#include "JJSExecutorObserver.h"

namespace facebook::react {

class WrapperJSExecutorFactory : public facebook::react::JSExecutorFactory {
public:
  WrapperJSExecutorFactory(jni::weak_ref<JAssetManager::javaobject> assetManager,
                           std::shared_ptr<facebook::react::JSExecutorFactory> baseExecutorFactory,
                           std::vector<std::unique_ptr<IJSBundle>> platformBundles,
                           jni::weak_ref<JJSExecutorObserver::javaobject> observer);

  std::unique_ptr<facebook::react::JSExecutor> createJSExecutor(
    std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) override;

private:
  std::shared_ptr<facebook::react::JSExecutorFactory> m_baseExecutorFactory;
  std::vector<std::unique_ptr<IJSBundle>> m_platformBundles;
  jni::weak_ref<JAssetManager::javaobject> m_assetManager;
  jni::weak_ref<JJSExecutorObserver::javaobject> m_observer;
};

} // namespace facebook::react