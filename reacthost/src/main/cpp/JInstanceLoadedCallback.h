//
// Created by anandrag on 9/18/2023.
//

#ifndef ORNHOST_JINSTANCELOADEDCALLBACK_H
#define ORNHOST_JINSTANCELOADEDCALLBACK_H


#include <fbjni/fbjni.h>
#include "JReactInstance.h"
#include "JErrorCode.h"

struct JInstanceLoadedCallback : facebook::jni::JavaClass<JInstanceLoadedCallback> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceLoadedCallback;";
    static void run(facebook::jni::alias_ref<JInstanceLoadedCallback> callback,
                    facebook::jni::alias_ref<JReactInstance::jhybridobject>,
                    facebook::jni::alias_ref<JErrorCode::jhybridobject>);
};


#endif //ORNHOST_JINSTANCELOADEDCALLBACK_H
