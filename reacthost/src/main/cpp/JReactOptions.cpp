#include "precomp.h"

#include "JInstanceCreatedCallback.h"
#include "JInstanceLoadedCallback.h"
#include "JReactOptions.h"
#include "ReactInstanceAndroid.h"
#include "JErrorCode.h"

using namespace facebook::jni;
using namespace Mso::React;

local_ref<JReactOptions::jhybridobject> JReactOptions::create(Mso::React::ReactOptions&& options) {
    return newObjectCxxArgs(std::move(options));
}

/*static*/ local_ref<JReactOptions::jhybriddata> JReactOptions::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::React::ReactOptions options;
    return makeCxxInstance(std::move(options));
}

std::string JReactOptions::getIdentity() {
    return options_.Identity;
}

void JReactOptions::setIdentity(std::string identity) {
    options_.Identity = identity;
}

facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>> JReactOptions::getJavaModuleNames() {
    auto jJavaModuleNames = JArrayList<JString>::create();
    std::for_each(options_.JavaModuleNames.begin(), options_.JavaModuleNames.end(), [&jJavaModuleNames](std::string item) {
        jJavaModuleNames->add(make_jstring(item));
    } );
    return jJavaModuleNames.release();
}

void JReactOptions::setJavaModuleNames(facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>> jJavaModuleNames) {
    for (const auto& elem : *jJavaModuleNames) {
        options_.JavaModuleNames.push_back(elem->toStdString());
    }
}

facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> JReactOptions::getJSBundles() {
    auto jJSBundles = facebook::jni::JArrayList<JJSBundle>::create(options_.JSBundles.size());
    for (auto&jsBundle: options_.JSBundles) {
        jJSBundles->add(JJSBundle::create(*jsBundle));
    }

    // TODO :: Is this the right pattern to return ? What is releaseAlias ?
    return jJSBundles.release();
}

void JReactOptions::setJSBundles(facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> jsBundles) {
    for (const auto& elem : *jsBundles) {
        options_.JSBundles.push_back(JJSBundle::get(make_local(elem)));
    }
}

void JReactOptions::setInstanceCreatedCallback(facebook::jni::alias_ref<JInstanceCreatedCallback> callback) {
    options_.OnInstanceCreated = [callback = make_global(callback)](IReactInstance& instance){
        // TODO :: Make it safer
        ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(&instance);
        JInstanceCreatedCallback::run(callback, instanceImpl->m_jReactInstance);
    };
}

facebook::jni::alias_ref<JInstanceCreatedCallback> JReactOptions::getInstanceCreatedCallback() {

}

void JReactOptions::setInstanceLoadedCallback(facebook::jni::alias_ref<JInstanceLoadedCallback> callback) {
    options_.OnInstanceLoaded = [callback = make_global(callback)](IReactInstance& instance, const Mso::ErrorCode& errorCode){
        // TODO :: Make it safer
        ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(&instance);
        Mso::ErrorCode errorCodeCopy = errorCode;
        JInstanceLoadedCallback::run(callback, instanceImpl->m_jReactInstance, JErrorCode::create(std::move(errorCodeCopy)));
    };
}

facebook::jni::alias_ref<JInstanceLoadedCallback> JReactOptions::getInstanceLoadedCallback() {

}

const ReactOptions& JReactOptions::Options() const noexcept
{
    return options_;
}

facebook::jni::alias_ref<JReactDevOptions::jhybridobject> JReactOptions::createDeveloperSettingsPeer() {
    return JReactDevOptions::create(options_).release();
}

/*static*/ facebook::jni::local_ref<JReactDevOptions::jhybridobject> JReactDevOptions::create(Mso::React::ReactOptions& options) {
    return JReactDevOptions::newObjectCxxArgs(options);
}

bool JReactDevOptions::getIsDevModeEnabled() {
    return options_.DeveloperSettings.IsDevModeEnabled;
}

void JReactDevOptions::setIsDevModeEnabled(bool isDevModeEnabled) {
    options_.DeveloperSettings.IsDevModeEnabled = isDevModeEnabled;
}

std::string JReactDevOptions::getSourceBundleName() {
    return options_.DeveloperSettings.SourceBundleName;
}

void JReactDevOptions::setSourceBundleName(std::string sourceBundleName) {
    options_.DeveloperSettings.SourceBundleName = sourceBundleName;
}

/*static*/ void JReactDevOptions::registerNatives() {
    registerHybrid({
        makeNativeMethod("getIsDevModeEnabled", JReactDevOptions::getIsDevModeEnabled),
        makeNativeMethod("setIsDevModeEnabled", JReactDevOptions::setIsDevModeEnabled),
        makeNativeMethod("getSourceBundleName", JReactDevOptions::getSourceBundleName),
        makeNativeMethod("setSourceBundleName", JReactDevOptions::setSourceBundleName)
    });
}

/*static*/ void JReactOptions::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JReactOptions::initHybrid),
        makeNativeMethod("getIdentity", JReactOptions::getIdentity),
        makeNativeMethod("setIdentity", JReactOptions::setIdentity),
        makeNativeMethod("getJavaModuleNames", JReactOptions::getJavaModuleNames),
        makeNativeMethod("setJavaModuleNames", JReactOptions::setJavaModuleNames),
        makeNativeMethod("getJSBundles", JReactOptions::getJSBundles),
        makeNativeMethod("setJSBundles", JReactOptions::setJSBundles),
        makeNativeMethod("getInstanceCreatedCallback", JReactOptions::getInstanceCreatedCallback),
        makeNativeMethod("setInstanceCreatedCallback", JReactOptions::setInstanceCreatedCallback),
        makeNativeMethod("getInstanceLoadedCallback", JReactOptions::getInstanceLoadedCallback),
        makeNativeMethod("setInstanceLoadedCallback", JReactOptions::setInstanceLoadedCallback),
        makeNativeMethod("createDeveloperSettingsPeer", JReactOptions::createDeveloperSettingsPeer)
                   });
}