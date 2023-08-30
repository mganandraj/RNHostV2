#include "ReactNativeHost/React.h"
#include <smartPtr/cntPtr.h>
#include "InstanceFactory.h"
#include <future/future.h>

namespace Mso { namespace React {

Mso::CntPtr<IReactInstanceInternal> MakeReactInstance(
	IReactHost& reactHost,
	ReactOptions&& options,
	/*Mso::JSHost::IRekaContextProxy& rekaContextProxy*/
	Mso::Promise<void>&& /*whenLoaded*/) noexcept
{
	// return Mso::Make<CJSInstance, IReactInstanceInternal>(reactHost, std::move(options));
	return nullptr;
}

}} // namespace Mso::React