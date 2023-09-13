#include "BundleServerJSBundle.h"


namespace Mso::React {

struct BundleServerBundle : public Mso::UnknownObject<Mso::React::IJSBundle>
{
	BundleServerBundle(const std::string& id) : m_id(id)
	{
	}
public:
	std::string_view Content() noexcept override
	{
		return "throw new Error('No offline bundle file was found');";
	}
	Mso::React::JSBundleInfo Info() noexcept override
	{
		return { m_id, {}, 0 };
	};
private:
	std::string m_id;
};


Mso::CntPtr<IJSBundle> MakeBundleServerJSBundle(
	std::string&& id) noexcept
{
	return Mso::Make<BundleServerBundle, IJSBundle>(std::move(id));
}

}