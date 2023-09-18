#ifndef reacthostapp_MAINACTIVITY_H
#define reacthostapp_MAINACTIVITY_H

#include <fbjni/fbjni.h>

// TODO :: Need to figure out a clean way to include this in the RNHost API.
#include "JReactViewInstance.h"

struct MainActivity : facebook::jni::JavaClass<MainActivity> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthostapp/MainActivity;";
    static void registerNatives();
    static void runReactOnView(facebook::jni::alias_ref<MainActivity>, facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject>);
    static void libletInit(facebook::jni::alias_ref<MainActivity> thiz);
    static void testHeadless(facebook::jni::alias_ref<MainActivity> thiz);
    // static void test(facebook::jni::alias_ref<MainActivity> thiz, facebook::jni::alias_ref<JInstanceCreatedCallback>);
};


#endif //reacthostapp_MAINACTIVITY_H
