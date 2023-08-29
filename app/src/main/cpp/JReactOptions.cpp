//
// Created by anandrag on 8/28/2023.
//

#include "JReactOptions.h"

void JReactOptions::callJava() {
    // static auto method = getClass
}

local_ref<JReactOptions::JavaPart> JReactOptions::create() {
    // alias_ref<JClass> cls =  javaClassStatic();
    // cls->getMethod<>()

    return newObjectCxxArgs();
}

/*static*/ local_ref<JReactOptions::jhybriddata> JReactOptions::initHybrid(alias_ref<jhybridobject> jThis){
    return makeCxxInstance();
}

/*static*/ void JReactOptions::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JReactOptions::initHybrid),
    });
}