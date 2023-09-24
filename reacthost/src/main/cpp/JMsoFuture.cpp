//
// Created by anandrag on 9/23/2023.
//

#include "JMsoFuture.h"

/*static*/ void JMsoFuture::registerNatives() {
    registerHybrid({
        makeNativeMethod("Set", JMsoFuture::Set)
    });
}

void JMsoFuture::Set() {
    promise_.SetValue();
}

/*static*/ facebook::jni::local_ref<JMsoFuture::jhybridobject> JMsoFuture::create() {
    return newObjectCxxArgs();
}

Mso::Future<void> JMsoFuture::getMsoFuture() {
    return promise_.AsFuture();
}
