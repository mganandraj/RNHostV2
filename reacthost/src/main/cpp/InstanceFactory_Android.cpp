#include "precomp.h"

#include "ReactNativeHost/React.h"
#include <smartPtr/cntPtr.h>
#include "InstanceFactory.h"
#include <future/future.h>

#include "ReactInstanceAndroidDeferred.h"

#include "ReactHost.h"

namespace Mso { namespace React {

LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost(ReactOptions&& options) noexcept
{
    return Mso::Make<ReactHost, IReactHost>(std::move(options), nullptr);
}

LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost(
        ReactOptions&& options,
        Mso::Promise<void>&& onInstanceLoaded) noexcept
{
    return Mso::Make<ReactHost, IReactHost>(std::move(options), std::move(onInstanceLoaded));
}

Mso::CntPtr<IReactInstanceInternal> MakeReactInstance(
	IReactHost& reactHost,
	ReactOptions&& options,
	Mso::JSHost::IRekaContextProxy& rekaContextProxy,
	Mso::Promise<void>&& whenLoaded) noexcept
{
	return Deferred::MakeReactInstance(reactHost, std::move(options), rekaContextProxy, std::move(whenLoaded));

}

}} // namespace Mso::React