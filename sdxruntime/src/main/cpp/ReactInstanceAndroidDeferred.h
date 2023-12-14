#pragma once

#include <ReactNativeHost/React.h>
#include <ReactNativeHost/React_Android.h>
#include "IReactInstanceInternal.h"

namespace Mso::React {
namespace Deferred {

Mso::TCntPtr<IReactInstanceInternal> MakeReactInstance(
    IReactHost& reactHost,
    ReactOptions&& options,
    Mso::JSHost::IRekaContextProxy& rekaContextProxy,
    Mso::Promise<void>&& whenLoaded) noexcept;

} // Deferred
}; // Mso::React