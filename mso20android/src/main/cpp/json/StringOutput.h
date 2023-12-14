#pragma once
#include <json/ijsonwriter.h>
#include <stlextensions/string_adapter.h>

namespace Mso::Json {

/**
	Write JSON to a new wstring owned by this class.
	This output object must remain in scope while writing.

	Not recommended for large output:
	- wstring requires a contiguous memory block, this can fail
	- growing the wstring may require memcpy (which is slow)

	Example usage:

	auto output = Mso::Json::MakeStringOutput<wchar_t>();
	auto writer = Mso::Json::Unicode::MakeWriter(*output);
	...
	if (writer->IsValid())
	{
		Use output->Value
	}
*/
template<typename TChar>
class StringOutput final : public Mso::RefCountedObject<TIOutput<TChar>>
{
public:
	bool TryWrite(const Mso::basic_string_view<TChar>& view) noexcept override
	{
		try
		{
			Value += view;
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}

		return true;
	}

	Mso::basic_string<TChar> Value;

private:
	friend struct Mso::MakePolicy::NoThrowCtor;
	StringOutput() noexcept
	{
	}
	virtual ~StringOutput() = default;
};

template<typename TChar>
Mso::CntPtr<StringOutput<TChar>> MakeStringOutput() noexcept
{
	return Mso::Make<StringOutput<TChar>>();
}

} // namespace Mso::Json
