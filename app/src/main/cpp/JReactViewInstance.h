//
// Created by anandrag on 9/2/2023.
//

#ifndef RNHOSTV2_2_JREACTVIEWINSTANCE_H
#define RNHOSTV2_2_JREACTVIEWINSTANCE_H

#include <fbjni/fbjni.h>
#include "ReactNativeHost/React.h"
#include <object/unknownObject.h>

struct OfficeReactViewInstance;
struct JOfficeReactRootView : facebook::jni::HybridClass<JOfficeReactRootView> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/OfficeReactRootView;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    Mso::Future<void> Reload(const Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance, const Mso::React::ReactViewOptions&& viewOptions) noexcept;
    Mso::Future<void> Unload() noexcept;

    JOfficeReactRootView(facebook::jni::alias_ref<jhybridobject> jThis);

    Mso::CntPtr<OfficeReactViewInstance> m_ReactViewInstance;
    facebook::jni::weak_ref<JOfficeReactRootView::jhybridobject> m_jThis;
};

struct OfficeReactViewInstance : public Mso::UnknownObject<Mso::React::IReactViewInstance> {
    virtual Mso::Future<void> Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                     Mso::React::ReactViewOptions&& viewOptions) noexcept override;
    virtual Mso::Future<void> Unload() noexcept override;

    OfficeReactViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jThis)
            :m_jThis(facebook::jni::make_weak(std::move(jThis))) {}
    facebook::jni::weak_ref<JOfficeReactRootView::jhybridobject> m_jThis;
};


#endif //RNHOSTV2_2_JREACTVIEWINSTANCE_H
