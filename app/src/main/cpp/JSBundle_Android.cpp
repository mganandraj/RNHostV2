#include <ReactNativeHost/JSBundle.h>
#include "AssetJSBundle_Android.h"

namespace Mso { namespace React {

LIBLET_PUBLICAPI Mso::CntPtr<IJSBundle> MakeFileJSBundle(
	std::string&& id,
	std::string&& fileName) noexcept
{
	return MakeAssetJSBundle(std::move(id), std::move(fileName));
}

LIBLET_PUBLICAPI std::string GetJSBundleFilePath(
	const std::string& /*jsBundleBasePath*/,
	const std::string& /*jsBundleRelativePath*/) noexcept
{
	return "";
}

}} // namespace Mso::React