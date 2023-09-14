//
// Created by anandrag on 9/14/2023.
//

#ifndef ORNHOST_JINSTANCECREATEDCALLBACK_H
#define ORNHOST_JINSTANCECREATEDCALLBACK_H

#include <fbjni/fbjni.h>
#include "JReactInstance.h"

struct JInstanceCreatedCallback : facebook::jni::JavaClass<JInstanceCreatedCallback> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceCreatedCallback;";
    static void run(facebook::jni::alias_ref<JInstanceCreatedCallback> callback, facebook::jni::alias_ref<JReactInstance::jhybridobject>);
};

#endif //ORNHOST_JINSTANCECREATEDCALLBACK_H
