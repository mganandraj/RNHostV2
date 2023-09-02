//
// Created by anandrag on 9/2/2023.
//

#ifndef RNHOSTV2_2_JREACTVIEWINSTANCE_H
#define RNHOSTV2_2_JREACTVIEWINSTANCE_H

#include <fbjni/fbjni.h>

class JReactViewInstance : facebook::jni::HybridClass<JReactViewInstance> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactViewInstance;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();
};


#endif //RNHOSTV2_2_JREACTVIEWINSTANCE_H
