#ifndef RNHOSTV2_2_MAINACTIVITY_H
#define RNHOSTV2_2_MAINACTIVITY_H

#include <fbjni/fbjni.h>

#include

class MainActivity : facebook::jni::JavaClass<MainActivity> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/MainActivity;";
    static void registerNatives();

    static void runReactOnView()
};


#endif //RNHOSTV2_2_MAINACTIVITY_H
