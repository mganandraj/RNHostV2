#ifndef ornhost_REACTINSTANCEANDROID_H
#define ornhost_REACTINSTANCEANDROID_H

#include <ReactNativeHost/React.h>
#include "IReactInstanceInternal.h"

#ifdef USE_OPENSOUCE_MSO
#include <activeObject/activeObject.h>
#include <object/refCountedObject.h>
#include <smartPtr/cntPtr.h>
#endif

#ifdef USE_OPENSOUCE_MSO
namespace Mso {
    template<typename T>
    using TCntPtr = CntPtr<T>;
}
#endif

namespace Mso::React {

class ReactInstanceAndroidInternal;

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

    void onInitialized(/*facebook::jni::alias_ref<JReactContextHolder> contextHolder*/) noexcept;
    void onBundleLoaded(std::string&& bundleName) noexcept;

    Mso::JSHost::RekaBridgeOptions createRekaBridgeOptions() noexcept;

private:
    friend MakePolicy;
    ReactInstanceAndroid(IReactHost& reactHost, ReactOptions&& options) noexcept;
    void Initialize() noexcept override;
    void Finalize() noexcept override;
    ~ReactInstanceAndroid() noexcept;

    void NotifyRekaInitialized() noexcept;
    void NotifyRekaDestroyed() noexcept;

    const Mso::Promise<void> m_whenLoaded;
    const Mso::Promise<void> m_whenDestroyed;
    const Mso::TCntPtr<Mso::JSHost::IRekaContextProxy> m_rekaContextProxy;

    std::atomic<bool> m_isLoaded { false };
    std::atomic<bool> m_isDestroyed { false };
    std::atomic<bool> m_isRekaInitialized { false };

private: // fields controlled by queue
    const Mso::ActiveField<Mso::TCntPtr<Mso::JSHost::IRekaContext>> m_rekaContext { Queue() };

public:
//private:
    const Mso::WeakPtr<IReactHost> m_weakReactHost;

    ReactOptions m_options;

    Mso::TCntPtr<ReactInstanceAndroidInternal> m_internalState;
};

}

#endif //ornhost_REACTINSTANCEANDROID_H
