//
// Created by anandrag on 9/2/2023.
//

#ifndef RNHOSTV2_2_JREACTVIEWINSTANCE_H
#define RNHOSTV2_2_JREACTVIEWINSTANCE_H

#include <fbjni/fbjni.h>
#include "ReactNativeHost/React.h"
#include <object/unknownObject.h>

struct JReactViewInstance : facebook::jni::HybridClass<JReactViewInstance>,
                            public Mso::UnknownObject<JReactViewInstance> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactViewInstance;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    virtual Mso::Future<void> Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                         Mso::React::ReactViewOptions&& viewOptions) noexcept override;
    virtual Mso::Future<void> Unload() noexcept override;
};


#endif //RNHOSTV2_2_JREACTVIEWINSTANCE_H
