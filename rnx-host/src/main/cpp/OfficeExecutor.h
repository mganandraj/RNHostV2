#pragma once 

#include <cxxreact/JSExecutor.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/RAMBundleRegistry.h>
#include <fbjni/fbjni.h>
#include <react/jni/JSLoader.h>
#include "JSBundle.h"

#include "JOfficeExecutorObserver.h"

namespace facebook {
namespace react {

struct OfficeExecutor : facebook::react::JSExecutor {
	OfficeExecutor(jni::weak_ref<JAssetManager::javaobject> assetManager
		, std::unique_ptr<facebook::react::JSExecutor> &&baseExecutor
		, std::vector<std::string> &&preloadBundles
		, std::vector<std::unique_ptr<IJSBundle>> &&platformBundles
		, jni::weak_ref<JOfficeExecutorObserver::javaobject> observer) noexcept
	    : m_assetManager(assetManager), m_baseExecutor{std::move(baseExecutor)}, m_preloadBundles(std::move(preloadBundles)), m_platformBundles(std::move(platformBundles)), m_observer(observer)
	{}

	void initializeRuntime() override;
	void loadBundle(std::unique_ptr<const facebook::react::JSBigString> script, std::string sourceURL) override;
	void setBundleRegistry(std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry) override;
	void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;
	void callFunction(const std::string &module, const std::string &method, const folly::dynamic &arguments) override;
	void invokeCallback(double callbackId, const folly::dynamic &arguments) override;
	void setGlobalVariable(std::string propName, std::unique_ptr<const facebook::react::JSBigString> jsonValue) override;
	std::string getDescription() override;
	void* getJavaScriptContext() override;

private:
	jni::weak_ref<JAssetManager::javaobject> m_assetManager;
	std::unique_ptr<facebook::react::JSExecutor> m_baseExecutor;
	std::vector<std::string> m_preloadBundles;
	std::vector<std::unique_ptr<IJSBundle>> m_platformBundles;
	jni::weak_ref<JOfficeExecutorObserver::javaobject> m_observer;
};

}}