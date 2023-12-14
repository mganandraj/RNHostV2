//
// Created by anandrag on 9/16/2023.
//

#ifndef ORNHOST_PLATFORMJSBUNDLES_H
#define ORNHOST_PLATFORMJSBUNDLES_H


//#include <compileradapters/functiondecorations.h>
#include <ReactNativeHost/JSBundle.h>
#include <string_view>

namespace Mso { namespace React {

// Please add an entry of JSBundle in reactnativehost/androidjava/src/com/microsoft/office/ReactNative/SDXDescriptor.java
// if the bundle is getting used on Android.
        namespace JSBundles
        {
            inline constexpr const char* SDXFoundation{"SDXFoundation"};
            inline constexpr const char* SDXUI{"SDXUI"};
        };

        /*LIBLET_PUBLICAPI*/ std::vector<Mso::CntPtr<IJSBundle>> GetJSBundles(std::initializer_list<std::string_view> jsBundleIds) noexcept;

    }} // namespace Mso::React


#endif //ORNHOST_PLATFORMJSBUNDLES_H
