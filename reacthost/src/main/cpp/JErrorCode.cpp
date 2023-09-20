#include "precomp.h"
#include "JErrorCode.h"

using namespace facebook::jni;

/*static*/ local_ref<JErrorCode::jhybriddata> JErrorCode::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::ErrorCode errorCode;
    return makeCxxInstance(std::move(errorCode));
}

std::string JErrorCode::toString() {
    return errorCode_.ToString();
}

/*static */facebook::jni::local_ref<JErrorCode::jhybridobject> JErrorCode::create(Mso::ErrorCode&& errorCode) {
    return newObjectCxxArgs(std::move(errorCode));
}

/*static*/ void JErrorCode::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JErrorCode::initHybrid),
                           makeNativeMethod("toString", JErrorCode::toString),
                   });
}