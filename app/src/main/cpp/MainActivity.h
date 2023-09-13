#ifndef ornhost_MAINACTIVITY_H
#define ornhost_MAINACTIVITY_H

#include <fbjni/fbjni.h>

#include "JReactViewInstance.h"

struct MainActivity : facebook::jni::JavaClass<MainActivity> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/MainActivity;";
    static void registerNatives();
    static void runReactOnView(facebook::jni::alias_ref<MainActivity>, facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject>);

//
//    void nativeVoidMethod(
//            // All non-static methods receive "this" as a first argument.
//            // facebook::jni::alias_ref<MainActivity> thiz
//            ) {
//        // Make sure we got the right object.
//        assert(thiz->toString() == "instance of DocTests");
//        // thiz->callVoidMethod();
//    }

//    static void nativeVoidMethod(
//            // All non-static methods receive "this" as a first argument.
//            facebook::jni::alias_ref<MainActivity> thiz) {
//        // Make sure we got the right object.
//        assert(thiz->toString() == "instance of DocTests");
//        thiz->callVoidMethod();
//    }
};


#endif //ornhost_MAINACTIVITY_H
