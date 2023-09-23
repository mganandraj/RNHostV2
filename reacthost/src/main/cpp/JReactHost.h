#ifndef ORNHOST_JREACTHOST_H
#define ORNHOST_JREACTHOST_H

#include <fbjni/fbjni.h>
#include "ReactNativeHost/React.h"

#include "JReactOptions.h"
#include "JReactViewHost.h"
#include "JReactViewOptions.h"

struct JReactHost : facebook::jni::HybridClass<JReactHost> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHost;";
    // static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::CntPtr<Mso::React::IReactHost> host);

    JReactHost(Mso::CntPtr<Mso::React::IReactHost> &&host)
        : host_(std::move(host)) {}

    facebook::jni::alias_ref<JReactViewHost::jhybridobject> MakeViewHost(facebook::jni::alias_ref<JReactViewOptions::jhybridobject> jOptions);
    void ReloadInstance();
    void ReloadInstanceWithOptions(facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
    void UnloadInstance();

    Mso::CntPtr<Mso::React::IReactHost> host_;
};

#endif //ORNHOST_JREACTHOST_H
