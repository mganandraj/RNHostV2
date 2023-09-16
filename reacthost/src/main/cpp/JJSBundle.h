//
// Created by anandrag on 9/15/2023.
//

#ifndef ORNHOST_JJSBUNDLE_H
#define ORNHOST_JJSBUNDLE_H

#include <fbjni/fbjni.h>
#include <fbjni/ByteBuffer.h>
#include <ReactNativeHost/JSBundle.h>

struct JJSBundleInfo  : facebook::jni::JavaClass<JJSBundleInfo> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundleInfo;";
    static Mso::React::JSBundleInfo get(facebook::jni::alias_ref<JJSBundleInfo> thizz);
    static facebook::jni::local_ref<JJSBundleInfo> create(Mso::React::JSBundleInfo info);
};

// Note :: This could be a hybrid object instead. But keeping it simple for now, and copying the object fields across the JNI boundary except the bundle shared buffer
struct JJSBundle  : facebook::jni::JavaClass<JJSBundle> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundle;";
    static Mso::CntPtr<Mso::React::IJSBundle> get(facebook::jni::alias_ref<JJSBundle> thizz);
    static facebook::jni::local_ref<JJSBundle> create(Mso::React::IJSBundle& bundle);
};

#endif //ORNHOST_JJSBUNDLE_H
