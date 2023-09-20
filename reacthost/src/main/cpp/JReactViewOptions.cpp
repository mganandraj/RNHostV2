#include "precomp.h"

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

const Mso::React::ReactViewOptions& JReactViewOptions::Options() const noexcept {
    return options_;
}

std::string JReactViewOptions::getComponentName(){
    return options_.ComponentName;
}

void JReactViewOptions::setComponentName(std::string componentName){
    options_.ComponentName = componentName;
}

std::string JReactViewOptions::getInitialProps() {
    return options_.InitialProps;
}

void JReactViewOptions::setInitialProps(std::string initialProps) {
    options_.InitialProps = initialProps;
}

/*static*/ void JReactViewOptions::registerNatives() {
    registerHybrid({
           makeNativeMethod("initHybrid", JReactViewOptions::initHybrid),
           makeNativeMethod("getComponentName", JReactViewOptions::getComponentName),
           makeNativeMethod("setComponentName", JReactViewOptions::setComponentName),
           makeNativeMethod("getInitialProps", JReactViewOptions::getInitialProps),
           makeNativeMethod("setInitialProps", JReactViewOptions::setInitialProps)
    });
}