//
// Created by anandrag on 8/28/2023.
//

#ifndef RNHOSTV2_2_JREACTOPTIONS_H
#define RNHOSTV2_2_JREACTOPTIONS_H

#include <fbjni/fbjni.h>
using namespace facebook::jni;

struct JReactOptions : HybridClass<JReactOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactOptions;";
    static local_ref<jhybriddata> initHybrid(alias_ref<jhybridobject> jThis);
    static void registerNatives();

    void callJava();
    static local_ref<JavaPart> create();
    //JReactOptions(alias_ref<JReactOptions::javaobject> jThis):
    //        jThis_(make_global(jThis)){}

    //global_ref<JReactOptions::javaobject> jThis_;
};

#endif //RNHOSTV2_2_JREACTOPTIONS_H
