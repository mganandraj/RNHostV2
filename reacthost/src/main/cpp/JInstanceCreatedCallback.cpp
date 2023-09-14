//
// Created by anandrag on 9/14/2023.
//

#include "JInstanceCreatedCallback.h"

using namespace Mso::React;

/*static */void JInstanceCreatedCallback::run(facebook::jni::alias_ref<JInstanceCreatedCallback> callback, facebook::jni::alias_ref<JReactInstance::jhybridobject> instance) {
    static auto clazz = javaClassStatic();
    auto method = clazz->getMethod<void(facebook::jni::alias_ref<JReactInstance::jhybridobject>)>("run");
    method(callback, instance);
}
