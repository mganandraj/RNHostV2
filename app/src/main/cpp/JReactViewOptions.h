//
// Created by Anandraj Govindan on 9/4/23.
//

#ifndef RNHOSTV2_2_JREACTVIEWOPTIONS_H
#define RNHOSTV2_2_JREACTVIEWOPTIONS_H

#include "fbjni/fbjni.h"
#include <ReactNativeHost/React.h>

struct JReactViewOptions : facebook::jni::HybridClass<JReactViewOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactViewOptions;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    JReactViewOptions(Mso::React::ReactViewOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(Mso::React::ReactViewOptions&&);
    Mso::React::ReactViewOptions options_;

    const Mso::React::ReactViewOptions& Options() const noexcept;

};


#endif //RNHOSTV2_2_JREACTVIEWOPTIONS_H
