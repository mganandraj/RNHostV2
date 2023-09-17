#include "ReactNativeHeaders.h"

#include "JOfficeExecutorObserver.h"
#include "OfficeExecutor.h"

#include <android/asset_manager_jni.h>
#include <cxxreact/JSBigString.h>
#include <react/jni/JSLoader.h>

#include <android/log.h>

#define LOG_TAG "RNX_HOST::OfficeExecutor"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)


namespace facebook {
namespace react {

void OfficeExecutor::initializeRuntime() {
    m_baseExecutor->initializeRuntime();   
    jni::local_ref<JOfficeExecutorObserver::javaobject> observer = m_observer.lockLocal();
    if(observer) {
        observer->OnInitialized();
    }
}

static bool isFilePath(const std::string& path) {
    return (path[0] == '/');
}

void OfficeExecutor::loadBundle(std::unique_ptr<const facebook::react::JSBigString> script, std::string sourceURL) {
    jni::local_ref<JOfficeExecutorObserver::javaobject> observer = m_observer.lockLocal();

    if(m_preloadBundles.size() > 0) {
        jni::local_ref<JAssetManager::javaobject> jAssetManager = m_assetManager.lockLocal();
        if(jAssetManager) {
            for (auto &preloadBundle : m_preloadBundles) {
                std::unique_ptr<const JSBigString> prescript;
                if(isFilePath(preloadBundle)) {
                    prescript = JSBigFileString::fromPath(preloadBundle);
                }
                else {
                    AAssetManager *assetManager = extractAssetManager(jAssetManager);
                    prescript = loadScriptFromAssets(assetManager, preloadBundle);
                }

                m_baseExecutor->loadBundle(std::move(prescript), preloadBundle);
                if(observer) {
                    observer->OnLoaded(preloadBundle);
                }
            }
        }
    }

    for (auto &platformBundle : m_platformBundles) {
        auto info = platformBundle->Info();
        auto content = platformBundle->Content();
        if(!content.empty()) {
            auto bundleBigString = std::make_unique<facebook::react::JSBigStdString>(std::string(content.data(), content.size()));
            m_baseExecutor->loadBundle(std::move(bundleBigString), info.Id);
            if(observer) {
                observer->OnLoaded(info.Id);
            }
        } else if (!info.FileName.empty()) {
            auto bundleBigString = JSBigFileString::fromPath(info.FileName);
            m_baseExecutor->loadBundle(std::move(bundleBigString), info.Id);
            if(observer) {
                observer->OnLoaded(info.Id);
            }
        } else {
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
        }

        // LOGE("script buffer: %s", platformBundle->Content().data());
    }

    LOGE("sourceURL: %s", sourceURL.c_str());
    LOGE("script: %s", script->c_str());
   
    m_baseExecutor->loadBundle(std::move(script), sourceURL);
    if(observer) {
        observer->OnLoaded(sourceURL);
    }
}

void OfficeExecutor::setBundleRegistry(std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry) {
    m_baseExecutor->setBundleRegistry(std::move(bundleRegistry));
}

void OfficeExecutor::registerBundle(uint32_t bundleId, const std::string &bundlePath) {
    m_baseExecutor->registerBundle(bundleId, bundlePath);
}

void OfficeExecutor::callFunction(const std::string &module, const std::string &method, const folly::dynamic &arguments) {
    m_baseExecutor->callFunction(module, method, arguments);
}

void OfficeExecutor::invokeCallback(double callbackId, const folly::dynamic &arguments) {
    m_baseExecutor->invokeCallback(callbackId, arguments);
}

void OfficeExecutor::setGlobalVariable(std::string propName, std::unique_ptr<const facebook::react::JSBigString> jsonValue) {
    m_baseExecutor->setGlobalVariable(std::move(propName), std::move(jsonValue));
}

std::string OfficeExecutor::getDescription() {
    return m_baseExecutor->getDescription();
}

void* OfficeExecutor::getJavaScriptContext() {
    return m_baseExecutor->getJavaScriptContext();
}

}}