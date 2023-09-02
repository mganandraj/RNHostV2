#include "MainActivity.h"

using namespace facebook::jni;

void MainActivity::registerNatives() {
    makeNativeMethod("runReactOnView", TestClassResolution),
}
