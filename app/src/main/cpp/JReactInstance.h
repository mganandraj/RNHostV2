#ifndef RNHOSTV2_2_JREACTINSTANCE_H
#define RNHOSTV2_2_JREACTINSTANCE_H

#include <ReactNativeHost/React.h>
#include "IReactInstanceInternal.h"
#include "activeObject/activeObject.h"

namespace Mso::React {

MSO_CLASS_GUID(CJSInstance, "d4fb4643-ec38-4839-be8c-2301d253f859")
class JReactInstance final : public Mso::ActiveObject<IReactInstanceInternal> {
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
    JReactInstance(IReactHost& reactHost, ReactOptions&& options) noexcept;
    void Initialize() noexcept override;
    ~JReactInstance() noexcept;

private:
    const Mso::WeakPtr<IReactHost> m_weakReactHost;
    const ReactOptions m_options;

};

}

#endif //RNHOSTV2_2_JREACTINSTANCE_H
