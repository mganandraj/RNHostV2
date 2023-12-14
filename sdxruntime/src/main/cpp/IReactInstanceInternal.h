#pragma once

#include <Object/QueryCast.h>
#include "ReactNativeHost/React.h"
#include <compilerAdapters/declspecDefinitions.h>

namespace Mso { namespace React {

//! Internal interface to be implemented by ReactInstance
struct IReactInstanceInternal : Mso::QueryCastList<IReactInstance>
{
	virtual Mso::Future<void> Destroy() noexcept = 0;
};

}} // namespace Mso::React