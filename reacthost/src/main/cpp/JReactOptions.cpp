#include "JInstanceCreatedCallback.h"
#include "JReactOptions.h"
#include "ReactInstanceAndroid.h"

using namespace facebook::jni;
using namespace Mso::React;

local_ref<JReactOptions::jhybridobject> JReactOptions::create(Mso::React::ReactOptions&& options) {
    return newObjectCxxArgs(std::move(options));
}

/*static*/ local_ref<JReactOptions::jhybriddata> JReactOptions::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::React::ReactOptions options;
    return makeCxxInstance(std::move(options));
}

std::string JReactOptions::getIdentity() {
    return options_.Identity;
}

void JReactOptions::setIdentity(std::string identity) {
    options_.Identity = identity;
}

void JReactOptions::setInstanceCreatedCallback(facebook::jni::alias_ref<JInstanceCreatedCallback> callback) {
    options_.OnInstanceCreated = [callback = make_global(callback)](IReactInstance& instance){
        // TODO :: Make it safer
        ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(&instance);
        JInstanceCreatedCallback::run(callback, instanceImpl->m_jReactInstance);
    };
}

facebook::jni::alias_ref<JInstanceCreatedCallback> JReactOptions::getInstanceCreatedCallback() {

}

const ReactOptions& JReactOptions::Options() const noexcept
{
    return options_;
}

/*static*/ void JReactOptions::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JReactOptions::initHybrid),
        makeNativeMethod("getIdentity", JReactOptions::getIdentity),
        makeNativeMethod("setIdentity", JReactOptions::setIdentity),
        makeNativeMethod("getInstanceCreatedCallback", JReactOptions::getInstanceCreatedCallback),
        makeNativeMethod("setInstanceCreatedCallback", JReactOptions::setInstanceCreatedCallback),
    });
}