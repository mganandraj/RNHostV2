//
// Created by anandrag on 9/18/2023.
//

#include "JInstanceLoadedCallback.h"

using namespace Mso::React;

/*static */void JInstanceLoadedCallback::run(facebook::jni::alias_ref<JInstanceLoadedCallback> callback,
                                             facebook::jni::alias_ref<JReactInstance::jhybridobject> instance,
                                             facebook::jni::alias_ref<JErrorCode::jhybridobject> errorCode) {
    static auto clazz = javaClassStatic();
    auto method = clazz->getMethod<void(facebook::jni::alias_ref<JReactInstance::jhybridobject>, facebook::jni::alias_ref<JErrorCode::jhybridobject>)>("run");
    method(callback, instance, errorCode);
}
