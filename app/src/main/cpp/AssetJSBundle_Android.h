#pragma once

#include <Object/UnknownObject.h>
#include <ReactNativeHost/JSBundle.h>

namespace Mso { namespace React {

Mso::CntPtr<IJSBundle> MakeAssetJSBundle(
	std::string&& id,
	std::string&& fileName) noexcept;

}} // namespace Mso::React