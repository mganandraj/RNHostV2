#ifndef ORNHOST_JREACTVIEWHOST_H
#define ORNHOST_JREACTVIEWHOST_H

#include <fbjni/fbjni.h>
#include <ReactNativeHost/React.h>
#include "JReactViewInstance.h"

struct JReactViewHost : facebook::jni::HybridClass<JReactViewHost>  {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewHost;";
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::CntPtr<Mso::React::IReactViewHost> viewHost);

    JReactViewHost(Mso::CntPtr<Mso::React::IReactViewHost> &&viewHost)
    : viewHost_(std::move(viewHost)) {}

    Mso::CntPtr<Mso::React::IReactViewHost> viewHost_;

    void AttachViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jView);
};


#endif //ORNHOST_JREACTVIEWHOST_H
