//
// Created by anandrag on 9/23/2023.
//

#ifndef ORNHOST_JMSOFUTURE_H
#define ORNHOST_JMSOFUTURE_H

#include <fbjni/fbjni.h>
#include <future/future.h>

struct JMsoFuture : facebook::jni::HybridClass<JMsoFuture> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/MsoFuturePeer;";
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create();
    Mso::Future<void> getMsoFuture();
    void Set();

    JMsoFuture()
    : promise_(Mso::Promise<void>()) {}

    Mso::Promise<void> promise_;
};


#endif //ORNHOST_JMSOFUTURE_H
