#include "precomp.h"
#include <jni.h>
#include "RekaNativeModuleProxy.h"

extern "C"
{

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void * /*reserved*/)
{
    return facebook::jni::initialize(vm, [] {
        RekaNativeModuleProxy::registerNatives();
        RekaNativeToJsBridge::registerNatives();
    });
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM * /*vm*/, void * /*reserved*/)
{
}

}