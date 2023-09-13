#pragma once 

#include <cxxreact/JSExecutor.h>
#include <vector>
#include <fbjni/fbjni.h>
#include <react/jni/JSLoader.h>

#include "JOfficeExecutorObserver.h"

namespace facebook {
namespace react {

class OfficeExecutorFactory : public facebook::react::JSExecutorFactory {
public:
  OfficeExecutorFactory(jni::weak_ref<JAssetManager::javaobject> assetManager
    , std::shared_ptr<facebook::react::JSExecutorFactory> baseExecutorFactory
    , std::vector<std::string> preloadBundles
    , jni::weak_ref<JOfficeExecutorObserver::javaobject> observer);

  std::unique_ptr<facebook::react::JSExecutor> createJSExecutor(
    std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) override;

private:
  std::shared_ptr<facebook::react::JSExecutorFactory> m_baseExecutorFactory;
  std::vector<std::string> m_preloadBundles;
  jni::weak_ref<JAssetManager::javaobject> m_assetManager;
  jni::weak_ref<JOfficeExecutorObserver::javaobject> m_observer;
};

}}