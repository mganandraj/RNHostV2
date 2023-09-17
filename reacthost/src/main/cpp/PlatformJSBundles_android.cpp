#include <jshostofficepal/PlatformJSBundles.h>
#include <reactnativehost/JSBundle.h>

#include "PlatformJSBundles_shared.h"

namespace Mso { namespace React {

        struct AndroidJSBundle : public UnknownObject<IJSBundle>
        {
        public:
            std::string_view Content() noexcept override
                    {
                            return std::string_view();
                    }

            JSBundleInfo Info() noexcept override
                    {
                            return m_jsBundleInfo;
                    }

        private:
            friend MakePolicy; // Allow Mso::Make to access private ctor
            AndroidJSBundle(std::string&& id, std::string&& fileName)
                    : m_jsBundleInfo{std::move(id), std::move(fileName), /*Timestamp*/ 0}
            {
            }

            JSBundleInfo m_jsBundleInfo;
        };

        struct PlatformJSBundleInfo
        {
            const char* Id;
            const char* FileName;
        };

        const PlatformJSBundleInfo PlatformJSBundleInfos[] = {
                {JSBundles::SDXFoundation, "foundation.android.bundle"},
                {JSBundles::SDXUI, "ui.android.bundle"}
        };

        constexpr array_view<PlatformJSBundleInfo> PlatformJSBundleList{PlatformJSBundleInfos, sizeof(PlatformJSBundleInfos)};

    Mso::CntPtr<IJSBundle> MakePlatformJSBundle(const PlatformJSBundleInfo& info) noexcept
{
    return Mso::Make<AndroidJSBundle, IJSBundle>(info.Id, info.FileName);
}

LIBLET_PUBLICAPI std::vector<Mso::CntPtr<IJSBundle>> GetJSBundles(std::initializer_list<std::string_view> jsBundleIds) noexcept
{
return GetJSBundlesInternal(jsBundleIds, PlatformJSBundleList);
}

}} // namespace Mso::React
