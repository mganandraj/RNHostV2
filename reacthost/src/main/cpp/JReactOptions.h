#ifndef ornhost_JREACTOPTIONS_H
#define ornhost_JREACTOPTIONS_H

#include <fbjni/fbjni.h>
#include <fbjni/NativeRunnable.h>
#include "ReactNativeHost/React.h"
#include "JJSBundle.h"

using namespace Mso::React;

struct JInstanceCreatedCallback;
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
};

#endif //ornhost_JREACTOPTIONS_H
