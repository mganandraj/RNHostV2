//
// Created by anandrag on 9/2/2023.
//

#include "JReactViewInstance.h"

using namespace facebook::jni;

void JReactViewInstance::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JReactViewInstance::initHybrid),
    });
}

/*static*/ local_ref<JReactViewInstance::jhybriddata> JReactViewInstance::initHybrid(alias_ref<jhybridobject> jThis){
    return makeCxxInstance();
}

/*virtual*/ Mso::Future<void> JReactViewInstance::Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                     Mso::React::ReactViewOptions&& viewOptions) noexcept {

}

/*virtual*/ Mso::Future<void> JReactViewInstance::Unload() noexcept {

}