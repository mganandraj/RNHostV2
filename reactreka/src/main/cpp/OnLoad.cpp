#include "precomp.h"
#include <jni.h>
#include "RekaNativeModuleProxy.h"
#include "JRekaBridgeOptions.h"
#include "ReactNativeRekaBridge.h"

extern "C"
{

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void * /*reserved*/)
{
    return facebook::jni::initialize(vm, [] {
        reactreka::RekaNativeModuleProxy::registerNatives();
        reactreka::RekaNativeToJsBridge::registerNatives();
    });
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM * /*vm*/, void * /*reserved*/)
{
}

}