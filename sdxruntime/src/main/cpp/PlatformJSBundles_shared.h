//
// Created by anandrag on 9/16/2023.
//

#ifndef ORNHOST_PLATFORMJSBUNDLES_SHARED_H
#define ORNHOST_PLATFORMJSBUNDLES_SHARED_H

#include <ReactNativeHost/JSBundle.h>
#include <stlextensions/array_view.h>

namespace Mso { namespace React {

        template <typename TInfo>
        Mso::CntPtr<IJSBundle> MakePlatformJSBundle(const TInfo& info) noexcept;

        template <typename TInfo>
        std::vector<Mso::CntPtr<IJSBundle>> GetJSBundlesInternal(std::initializer_list<std::string_view> jsBundleIds, const array_view<TInfo>& jsBundleList) noexcept
        {
            std::vector<Mso::CntPtr<IJSBundle>> result;

            for (const auto& jsBundleId : jsBundleIds)
            {
                bool bundleFound = false;

                // `jsBundleList` is a very small list so no need to worry about n*m loop here.
                for (size_t i = 0; i < jsBundleList.size(); ++i)
                {
                    if (jsBundleId == jsBundleList[i].Id)
                    {
                        result.push_back(MakePlatformJSBundle(jsBundleList[i]));
                        bundleFound = true;
                        break;
                    }
                }

                AssertSz1Tag(bundleFound, "Attempting to retrieve non-platform JS bundle '%s' via GetJSBundles()", jsBundleId.data(), 0x02863510 /* tag_c79uq */);
            }

            return result;
        }

    }} // namespace Mso::React



#endif //ORNHOST_PLATFORMJSBUNDLES_SHARED_H
