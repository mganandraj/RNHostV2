#include "JReactViewOptions.h"

using namespace facebook::jni;
using namespace Mso::React;

local_ref<JReactViewOptions::jhybridobject> JReactViewOptions::create(Mso::React::ReactViewOptions&& options) {
    return newObjectCxxArgs(std::move(options));
}

/*static*/ local_ref<JReactViewOptions::jhybriddata> JReactViewOptions::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::React::ReactViewOptions options;
    return makeCxxInstance(std::move(options));
}