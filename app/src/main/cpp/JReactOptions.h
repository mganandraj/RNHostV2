#ifndef RNHOSTV2_2_JREACTOPTIONS_H
#define RNHOSTV2_2_JREACTOPTIONS_H

#include <fbjni/fbjni.h>
#include "ReactNativeHost/React.h"

struct JReactOptions : facebook::jni::HybridClass<JReactOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactOptions;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    JReactOptions(Mso::React::ReactOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(Mso::React::ReactOptions&&);
    Mso::React::ReactOptions options_;

    std::string getIdentity();
    void setIdentity(std::string identity);
};

#endif //RNHOSTV2_2_JREACTOPTIONS_H
