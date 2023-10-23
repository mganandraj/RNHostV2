#include "ReactNativeHeaders.h"

#include "WrapperJSExecutor.h"

#include <android/asset_manager_jni.h>
#include <cxxreact/JSBigString.h>
#include <react/jni/JSLoader.h>

namespace facebook::react {

void WrapperJSExecutor::initializeRuntime() {
    m_baseExecutor->initializeRuntime();   
    jni::local_ref<JJSExecutorObserver::javaobject> observer = m_observer.lockLocal();
    if(observer) {
        observer->OnInitialized();
    }
}

void WrapperJSExecutor::loadBundle(std::unique_ptr<const facebook::react::JSBigString> script, std::string sourceURL) {
    jni::local_ref<JJSExecutorObserver::javaobject> observer = m_observer.lockLocal();
    for (auto &platformBundle : m_platformBundles) {
        auto info = platformBundle->Info();
        auto content = platformBundle->Content();
        if(!content.empty()) {
            auto bundleBigString = std::make_unique<facebook::react::JSBigStdString>(std::string(content.data(), content.size()));
            auto sourceUrl = !info.Id.empty() ? info.Id : std::string("<dynamic>");
            m_baseExecutor->loadBundle(std::move(bundleBigString), info.Id);
            if(observer) {
                observer->OnLoaded(info.Id);
            }
        } else if (!info.Id.empty()){
            // Assuming assets with name as url
            jni::local_ref<JAssetManager::javaobject> jAssetManager = m_assetManager.lockLocal();
            if(jAssetManager) {
                AAssetManager *assetManager = extractAssetManager(jAssetManager);
                auto bundleBigString = loadScriptFromAssets(assetManager, info.Id);
                m_baseExecutor->loadBundle(std::move(bundleBigString), info.Id);
                if(observer) {
                    observer->OnLoaded(info.Id);
                }
            }
        } else if (!info.FileName.empty()) {
            auto bundleBigString = JSBigFileString::fromPath(info.FileName);
            m_baseExecutor->loadBundle(std::move(bundleBigString), info.FileName);
            if(observer) {
                observer->OnLoaded(info.FileName);
            }
        }
    }

    m_baseExecutor->loadBundle(std::move(script), sourceURL);
    if(observer) {
        observer->OnLoaded(sourceURL);
    }
}

void WrapperJSExecutor::setBundleRegistry(std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry) {
    m_baseExecutor->setBundleRegistry(std::move(bundleRegistry));
}

void WrapperJSExecutor::registerBundle(uint32_t bundleId, const std::string &bundlePath) {
    m_baseExecutor->registerBundle(bundleId, bundlePath);
}

void WrapperJSExecutor::callFunction(const std::string &module, const std::string &method, const folly::dynamic &arguments) {
    m_baseExecutor->callFunction(module, method, arguments);
}

void WrapperJSExecutor::invokeCallback(double callbackId, const folly::dynamic &arguments) {
    m_baseExecutor->invokeCallback(callbackId, arguments);
}

void WrapperJSExecutor::setGlobalVariable(std::string propName, std::unique_ptr<const facebook::react::JSBigString> jsonValue) {
    m_baseExecutor->setGlobalVariable(std::move(propName), std::move(jsonValue));
}

std::string WrapperJSExecutor::getDescription() {
    return m_baseExecutor->getDescription();
}

void* WrapperJSExecutor::getJavaScriptContext() {
    return m_baseExecutor->getJavaScriptContext();
}

} // namespace facebook::react