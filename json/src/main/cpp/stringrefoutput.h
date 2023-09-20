#pragma once
#include <json/ijsonwriter.h>
#include <stlextensions/string_adapter.h>
#include <object/refcountedobject.h>

namespace Mso::Json {

/**
	TIOutput adapter that writes to an existing string.

	auto str = ...;
	auto writer = Mso::Json::Unicode::MakeWriter(*Mso::Json::MakeStringRefOutput(str));
	...
	if (writer->IsValid())
	{
		Use str
	}
*/
template<typename TChar>
class StringRefOutput final : public Mso::RefCountedObject<TIOutput<TChar>>
{
public:
	bool TryWrite(const Mso::basic_string_view<TChar>& view) noexcept override
	{
		OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
		try
		{
			m_value += view;
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}

		return true;
	}

private:
	friend struct Mso::MakePolicy::NoThrowCtor;
	StringRefOutput(Mso::basic_string<TChar>& value) noexcept : m_value(value)
	{
	}
	virtual ~StringRefOutput() = default;

	Mso::basic_string<TChar>& m_value;
};

template<typename TChar>
Mso::CntPtr<IOutput<TChar>> MakeStringRefOutput(Mso::basic_string<TChar>& value) noexcept
{
	return Mso::Make<StringRefOutput<TChar>, TIOutput<TChar>>(value);
}

} // namespace Mso::Json
