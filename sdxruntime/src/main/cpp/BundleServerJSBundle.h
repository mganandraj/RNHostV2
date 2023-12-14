#pragma once

#include <Object/UnknownObject.h>
#include "ReactNativeHost/JSBundle.h"

namespace Mso { namespace React {

Mso::CntPtr<IJSBundle> MakeBundleServerJSBundle(
	std::string&& id) noexcept;

}} // namespace Mso::React