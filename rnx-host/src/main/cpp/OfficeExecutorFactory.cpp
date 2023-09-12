#include "ReactNativeHeaders.h"
#include <jsi/jsi.h>
#include <jsireact/JSIExecutor.h>

#include "OfficeExecutorFactory.h"
#include "OfficeExecutor.h"

#include "JOfficeExecutorObserver.h"

namespace facebook {
namespace react {

OfficeExecutorFactory::OfficeExecutorFactory(jni::weak_ref<JAssetManager::javaobject> assetManager, std::shared_ptr<JSExecutorFactory> baseExecutorFactory
  , std::vector<std::string> preloadBundles
  , jni::weak_ref<JOfficeExecutorObserver::javaobject> observer) {
    m_baseExecutorFactory = std::move(baseExecutorFactory);
    m_preloadBundles = std::move(preloadBundles);
    m_assetManager = assetManager;
    m_observer = observer;
}

std::unique_ptr<JSExecutor> OfficeExecutorFactory::createJSExecutor(
  std::shared_ptr<ExecutorDelegate> delegate,
  std::shared_ptr<MessageQueueThread> jsQueue) {
    std::unique_ptr<JSExecutor> baseExecutor = m_baseExecutorFactory->createJSExecutor(
      delegate,
      jsQueue);

    std::unique_ptr<JSExecutor> officeExecutor = std::make_unique<OfficeExecutor>(m_assetManager
      , std::move(baseExecutor)
      , std::move(m_preloadBundles)
      , m_observer);

    return officeExecutor;
}
}}