#ifndef ornhost_JREACTOPTIONS_H
#define ornhost_JREACTOPTIONS_H

#include <fbjni/fbjni.h>
#include <fbjni/NativeRunnable.h>
#include "ReactNativeHost/React.h"
#include "JJSBundle.h"

using namespace Mso::React;

struct JReactDevOptions : facebook::jni::HybridClass<JReactDevOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactDevOptions;";
    static void registerNatives();
    JReactDevOptions(Mso::React::ReactOptions &options) :
        options_(options) {}
    static facebook::jni::local_ref<jhybridobject> create(Mso::React::ReactOptions&);
    Mso::React::ReactOptions& options_;

    bool getIsDevModeEnabled();
    void setIsDevModeEnabled(bool isDevModeEnabled);

    std::string getSourceBundleName();
    void setSourceBundleName(std::string sourceBundleName);
};

struct JInstanceCreatedCallback;
struct JInstanceLoadedCallback;
struct JReactOptions : facebook::jni::HybridClass<JReactOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactOptions;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    JReactOptions(Mso::React::ReactOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(Mso::React::ReactOptions&&);
    Mso::React::ReactOptions options_;

    const Mso::React::ReactOptions& Options() const noexcept;

    std::string getIdentity();
    void setIdentity(std::string identity);

    facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>> getJavaModuleNames();
    void setJavaModuleNames(facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>>);

    facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> getJSBundles();
    void setJSBundles(facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>>);

    void setInstanceCreatedCallback(facebook::jni::alias_ref<JInstanceCreatedCallback>);
    facebook::jni::alias_ref<JInstanceCreatedCallback> getInstanceCreatedCallback();

    void setInstanceLoadedCallback(facebook::jni::alias_ref<JInstanceLoadedCallback>);
    facebook::jni::alias_ref<JInstanceLoadedCallback> getInstanceLoadedCallback();

    facebook::jni::alias_ref<JReactDevOptions::jhybridobject> createDeveloperSettingsPeer();
};

#endif //ornhost_JREACTOPTIONS_H
