#ifndef RNHOSTV2_2_REACTINSTANCEANDROID_H
#define RNHOSTV2_2_REACTINSTANCEANDROID_H

#include "ReactNativeHost/React.h"
#include "IReactInstanceInternal.h"
#include "activeObject/activeObject.h"
#include <fbjni/fbjni.h>

#include "JReactOptions.h"

namespace Mso::React {

struct JReactInstance  : facebook::jni::HybridClass<JReactInstance> {
    static constexpr auto kJavaDescriptor = "Lcom/example/rnhostv2_2/ReactInstance;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(facebook::jni::alias_ref<JReactOptions::jhybridobject> );
};

MSO_CLASS_GUID(ReactInstanceAndroid, "36788a2a-5b5e-4ec4-b8de-21bee9534721")
class ReactInstanceAndroid final : public Mso::ActiveObject<IReactInstanceInternal>{
    using Super = ActiveObjectType;

public: // IReactInstance
    const std::string& JsBundleName() const noexcept override;
    std::string SDXBasePath() const noexcept override;
    bool NeedsReload() const noexcept override { VerifyElseCrashSzTag(false, "Not implemented", 0x025d3396 /* tag_cxtow */); }
    void SetAsNeedsReload() noexcept override { VerifyElseCrashSzTag(false, "Not implemented", 0x025d3397 /* tag_cxtox */); }
    bool GetQuirk(SDXQuirk /*quirk*/) const noexcept override { VerifyElseCrashSzTag(false, "Not implemented", 0x025d3398 /* tag_cxtoy */); }
    const ReactOptions& Options() const noexcept override;

public: // IReactInstanceInternal
    Mso::Future<void> Destroy() noexcept override;

private:
    friend MakePolicy;
    ReactInstanceAndroid(IReactHost& reactHost, ReactOptions&& options) noexcept;
    void Initialize() noexcept override;
    ~ReactInstanceAndroid() noexcept;

public:
//private:
    const Mso::WeakPtr<IReactHost> m_weakReactHost;

    ReactOptions m_options;
    facebook::jni::global_ref<JReactOptions::jhybridobject> m_jOptions;
    facebook::jni::global_ref<JReactInstance::jhybridobject> m_jReactInstance;
};

}

#endif //RNHOSTV2_2_REACTINSTANCEANDROID_H
