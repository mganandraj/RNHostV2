#include "precomp.h"

#include "AssetJSBundle_Android.h"

namespace Mso { namespace React {

struct AssetJSBundle : public UnknownObject<IJSBundle>
{
public:
	std::string_view Content() noexcept override;
	JSBundleInfo Info() noexcept override;

private:
	friend MakePolicy; // Allows Mso::Make to access private constructor
	AssetJSBundle(
		std::string&& id,
		std::string&& fileName) noexcept;

	uint64_t Timestamp() noexcept;

private:
	std::string m_id;
	std::string m_fileName;
};

AssetJSBundle::AssetJSBundle(
	std::string&& id,
	std::string&& fileName) noexcept
	: m_id(std::move(id))
	, m_fileName(std::move(fileName))
{
}

std::string_view AssetJSBundle::Content() noexcept
{
	return "";
}

JSBundleInfo AssetJSBundle::Info() noexcept
{
	return JSBundleInfo{m_id, m_fileName, Timestamp()};
}

uint64_t AssetJSBundle::Timestamp() noexcept
{
	return 0;
}

Mso::CntPtr<IJSBundle> MakeAssetJSBundle(
	std::string&& id,
	std::string&& fileName) noexcept
{
	return Mso::Make<AssetJSBundle, IJSBundle>(std::move(id), std::move(fileName));
}

}} // namespace Mso::React