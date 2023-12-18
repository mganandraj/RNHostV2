#ifndef ORNHOST_ORNHOSTHEADLESSFRAGMENT_H
#define ORNHOST_ORNHOSTHEADLESSFRAGMENT_H

#include <fbjni/fbjni.h>
#include <ReactNativeHost/React_Android.h>

struct ORNHostHeadlessFragment : facebook::jni::JavaClass<ORNHostHeadlessFragment> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthostapp/ORNHostHeadlessFragment;";
    static void registerNatives();
    static void runNative(facebook::jni::alias_ref<ORNHostHeadlessFragment> thiz);
};


#endif //ORNHOST_ORNHOSTHEADLESSFRAGMENT_H
