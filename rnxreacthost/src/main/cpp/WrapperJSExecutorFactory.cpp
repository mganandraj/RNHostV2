#include "ReactNativeHeaders.h"
#include <jsi/jsi.h>

#include "WrapperJSExecutorFactory.h"
#include "WrapperJSExecutor.h"

namespace facebook::react {

WrapperJSExecutorFactory::WrapperJSExecutorFactory(jni::weak_ref<JAssetManager::javaobject> assetManager,
                                                   std::shared_ptr<JSExecutorFactory> baseExecutorFactory,
                                                   std::vector<std::unique_ptr<IJSBundle>> platformBundles,
                                                   jni::weak_ref<JJSExecutorObserver::javaobject> observer) {
    m_baseExecutorFactory = std::move(baseExecutorFactory);
    m_platformBundles = std::move(platformBundles);
    m_assetManager = assetManager;
    m_observer = observer;
}

std::unique_ptr<JSExecutor> WrapperJSExecutorFactory::createJSExecutor(
  std::shared_ptr<ExecutorDelegate> delegate,
  std::shared_ptr<MessageQueueThread> jsQueue) {
    std::unique_ptr<JSExecutor> baseExecutor = m_baseExecutorFactory->createJSExecutor(
      delegate,
      jsQueue);

    std::unique_ptr<JSExecutor> officeExecutor = std::make_unique<WrapperJSExecutor>(m_assetManager,
                                                                                     std::move(baseExecutor),
                                                                                     std::move(m_platformBundles),
                                                                                     m_observer);

    return officeExecutor;
}
} // namespace facebook::react