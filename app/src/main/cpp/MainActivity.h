#ifndef reacthostapp_MAINACTIVITY_H
#define reacthostapp_MAINACTIVITY_H

#include <fbjni/fbjni.h>

#include "JReactViewInstance.h"

struct MainActivity : facebook::jni::JavaClass<MainActivity> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthostapp/MainActivity;";
    static void registerNatives();
    static void runReactOnView(facebook::jni::alias_ref<MainActivity>, facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject>);
};


#endif //reacthostapp_MAINACTIVITY_H
