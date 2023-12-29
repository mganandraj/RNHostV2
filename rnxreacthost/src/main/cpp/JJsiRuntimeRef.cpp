#include "rnx-host/JJsiRuntimeRef.h"

namespace facebook::react {

facebook::jni::local_ref<JJsiRuntimeRef::jhybridobject> JJsiRuntimeRef::create(facebook::jsi::Runtime &runtime) {
    return newObjectCxxArgs(runtime);
}

}