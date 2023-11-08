#ifndef ORNHOST_ORNHOSTKTNATIVEOPTIONSFRAGMENT_H
#define ORNHOST_ORNHOSTKTNATIVEOPTIONSFRAGMENT_H

#include <fbjni/fbjni.h>
#include <ReactNativeHost/React_Android.h>

struct ORNHostKtNativeOptionsFragment : facebook::jni::JavaClass<ORNHostKtNativeOptionsFragment> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthostapp/ORNHostKtNativeOptionsFragment;";
    static void registerNatives();
    static facebook::jni::local_ref<Mso::React::JReactOptions_::javaobject> getReactOptions(facebook::jni::alias_ref<ORNHostKtNativeOptionsFragment> thiz);
};


#endif //ORNHOST_ORNHOSTKTNATIVEOPTIONSFRAGMENT_H
