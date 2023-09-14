#ifndef ORNHOST_JREACTHOSTSTATICS_H
#define ORNHOST_JREACTHOSTSTATICS_H

#include <fbjni/fbjni.h>
#include "JReactHost.h"
#include "JReactOptions.h"

struct JReactHostStatics : facebook::jni::JavaClass<JReactHostStatics>  {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHostStatics;";
    static void registerNatives();

    static facebook::jni::alias_ref<JReactHost::jhybridobject> makeJReactHost(facebook::jni::alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
    // static facebook::jni::local_ref<JReactHost::jhybridobject> test(facebook::jni::alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
};

#endif //ORNHOST_JREACTHOSTSTATICS_H
