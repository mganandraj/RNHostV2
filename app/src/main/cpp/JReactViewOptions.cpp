#include "JReactViewOptions.h"

using namespace facebook::jni;
using namespace Mso::React;

local_ref<JReactViewOptions::jhybridobject> JReactViewOptions::create(const Mso::React::ReactViewOptions&& options) {
    return newObjectCxxArgs(std::move(const_cast<Mso::React::ReactViewOptions&&>(options)));
}

/*static*/ local_ref<JReactViewOptions::jhybriddata> JReactViewOptions::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::React::ReactViewOptions options;
    return makeCxxInstance(std::move(options));
}

std::string JReactViewOptions::getComponentName(){
    return options_.ComponentName;
}

void JReactViewOptions::setComponentName(std::string componentName){
    options_.ComponentName = componentName;
}

/*static*/ void JReactViewOptions::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JReactViewOptions::initHybrid),
                           makeNativeMethod("getComponentName", JReactViewOptions::getComponentName),
                           makeNativeMethod("setComponentName", JReactViewOptions::setComponentName)
                   });
}