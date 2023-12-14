/**
	IJsonReader API parses Json data (see http://Json.org) in a pull-model (simliar to IXmlReader / XmlReader).

	auto reader = Mso::Json::Utf8::MakeReader(...);
	
	Typical flow: call ReadNext, check ParseState, perform an action, repeat

	The TryGet* methods are used to retrieve data for the Name + Value states.
*/
#pragma once
#include <memory>
// #include <msoplatformfix.h>
#include <object/refcounted.h>
#include <stlextensions/string_adapter.h>
#include <optional>
// #include <safeint/safeint.h>

namespace msl::utilities {

template<typename To, typename From>
constexpr std::optional<To> TrySafeCast(From from) noexcept
{
	To to;
	if (SafeCast(from, to))
		return to;
	return {};
}

} // namespace msl::utilities

namespace Mso::Json {

/**
	Last token read from parser
	FUTURE: enum class
*/
namespace ParseState {
enum Enum
{
	StartArray,
	StartObject,
	Name,
	Value,
	EndObject,
	EndArray,

	// Returned for invalid data
	ErrorInvalidData,

	// End of input reached before end of Json.
	ErrorEndOfData,
};
};

inline bool IsParseError(ParseState::Enum state) noexcept
{
	return state == ParseState::ErrorInvalidData || state == ParseState::ErrorEndOfData;
}

/**
	Abtraction around input source for parsing json
	Populate m_json in the constructor and FillBuffer.
*/
template<typename TChar>
struct TIInput
{
	virtual ~TIInput() = default;

	bool IsEndOfData() const noexcept
	{
		return m_json.empty();
	}

	TChar PeekChar() const noexcept
	{
		return m_json.front();
	}

	void Inc() noexcept
	{
		m_json.remove_prefix(1);
		FillBuffer();
	}

	virtual void FillBuffer() noexcept
	{
	}

protected:
	TIInput() noexcept = default;

	Mso::basic_string_view<TChar> m_json;
};

/**
	FUTURE: does this really need to be ref-counted?
*/
template<typename TChar>
struct TIReader : Mso::IRefCounted
{
	// Read the next token
	virtual ParseState::Enum ReadNext() noexcept = 0;

	// Returns {} if the current state is not a string value
	// NOTE: string_view is only valid until ReadNext is called again
	virtual std::optional<Mso::basic_string_view<TChar>> TryGetString() const noexcept = 0;

	// Returns {} if the current state is not a bool (true, false)
	virtual std::optional<bool> TryGetBool() const noexcept = 0;

	// Returns {} if the current state is not a valid int32_t
	virtual std::optional<int32_t> TryGetInt32() const noexcept = 0;

	// Returns {} if the current state is not a valid uint32_t
	std::optional<uint32_t> TryGetUInt32() const noexcept
	{
		auto opt64 = TryGetUInt64();
		if (opt64)
			return TrySafeCast<uint32_t>(*opt64);
		return {};
	}

	// Returns {} if the current state is not a valid int64_t
	virtual std::optional<int64_t> TryGetInt64() const noexcept = 0;

	// Returns {} if the current state is not a valid uint64_t
	virtual std::optional<uint64_t> TryGetUInt64() const noexcept = 0;

	// Returns {} if the current state is not a valid double
	virtual std::optional<double> TryGetDouble() const noexcept = 0;

	// Returns {} if the current state is not a null value
	virtual bool IsNull() const noexcept = 0;

	// Returns the raw string value and whether it was quoted or not
	virtual std::optional<std::pair<Mso::basic_string_view<TChar>, bool>> TryGetRawValue() const noexcept = 0;
	
	/**
		Deprecated APIs
	*/

	bool GetString(_Out_ Mso::basic_string_view<TChar>& value) const noexcept
	{
		auto opt = TryGetString();
		value = opt.value_or(Mso::basic_string_view<TChar>());
		return opt.has_value();
	}

	bool GetBool(_Out_ bool* pValue) const noexcept
	{
		auto opt = TryGetBool();
		*pValue = opt.value_or(false);
		return opt.has_value();
	}

	bool GetInt(_Out_ int32_t* pValue) const noexcept
	{
		auto opt = TryGetInt32();
		*pValue = opt.value_or(0);
		return opt.has_value();
	}

	bool GetInt64(_Out_ int64_t* pValue) const noexcept
	{
		auto opt = TryGetInt64();
		*pValue = opt.value_or(0);
		return opt.has_value();
	}

	bool GetUInt64(_Out_ uint64_t* pValue) const noexcept
	{
		auto opt = TryGetUInt64();
		*pValue = opt.value_or(0);
		return opt.has_value();
	}

	bool GetDouble(_Out_ double* pValue) const noexcept
	{
		auto opt = TryGetDouble();
		*pValue = opt.value_or(0);
		return opt.has_value();
	}

	// Returns false if the current state is not a string
	// NOTE: returned string is not null terminated and is only valid until ReadNext is called again
	bool GetString(_Deref_post_count_(*pcchValue) const TChar** prgtchValue, _Out_ size_t* pcchValue) noexcept
	{
		Mso::basic_string_view<TChar> value;
		auto hasResult = GetString(value);
		*prgtchValue = value.data();
		*pcchValue = value.size();
		return hasResult;
	}
};

} // namespace Mso::Json

/**
	Factory methods for parsing ansi/ascii/utf8 JSON
*/
namespace Mso::Json::Utf8 {

using IInput = Mso::Json::TIInput<char>;
using IReader = Mso::Json::TIReader<char>;

LIBLET_PUBLICAPI Mso::CntPtr<IReader> MakeReader(const Mso::basic_string_view<char>& json) noexcept;

// Parse json from an arbitrary input - the lifetime of the IInput object is the same as IReader
LIBLET_PUBLICAPI Mso::CntPtr<IReader> MakeReader(std::unique_ptr<IInput>&& json) noexcept;

//// Constructs a JsonInput from a sequential stream. Does not fail.
//LIBLET_PUBLICAPI std::unique_ptr<IInput> MakeInput(ISequentialStream& stream) noexcept;

} // namespace Mso::Json::Utf8

/**
	Factory methods for parsing Unicode JSON
*/
namespace Mso::Json::Unicode {

using IInput = Mso::Json::TIInput<wchar_t>;
using IReader = Mso::Json::TIReader<wchar_t>;

LIBLET_PUBLICAPI Mso::CntPtr<IReader> MakeReader(const Mso::basic_string_view<wchar_t>& json) noexcept;

// Parse json from an arbitrary input - the lifetime of the IInput object is the same as IReader
LIBLET_PUBLICAPI Mso::CntPtr<IReader> MakeReader(std::unique_ptr<IInput>&& json) noexcept;

//// Constructs a JsonInput from a sequential stream. Does not fail.
//LIBLET_PUBLICAPI std::unique_ptr<IInput> MakeInput(ISequentialStream& stream) noexcept;

} // namespace Mso::Json::Unicode

namespace Mso::Json::Flighting {

LIBLET_PUBLICAPI void EnableIntOverflowChecks(bool value) noexcept;

} // namespace Mso::Json::Flighting



/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/

namespace Mso::Json {

template<typename TChar>
using TJsonInput = TIInput<TChar>;

template<typename TChar>
using TJsonReader = TIReader<TChar>;

} // namespace Mso::Json

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/

namespace Mso::Json::Unicode {

using Reader = TJsonReader<wchar_t>;

} // namespace Mso::Json::Unicode

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/

namespace Mso::Json {

using IJsonReader = Unicode::Reader;
inline Mso::CntPtr<IJsonReader> CreateJsonReader(_In_count_(cchJson) const wchar_t* rgwchJson, size_t cchJson) noexcept
{
	return Unicode::MakeReader(Mso::basic_string_view<wchar_t>(rgwchJson, cchJson));
}
//inline Mso::CntPtr<IJsonReader> CreateJsonReader(ISequentialStream& stream) noexcept
//{
//	return Unicode::MakeReader(Unicode::MakeInput(stream));
//}

} // namespace Mso::Json

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/

namespace Mso::Json::Ansi {

using Reader = Mso::Json::TIReader<char>;

inline Mso::CntPtr<Reader> MakeReader(const Mso::basic_string_view<char>& json) noexcept
{
	return Mso::Json::Utf8::MakeReader(json);
}

inline Mso::CntPtr<Reader> MakeReader(std::unique_ptr<TJsonInput<char>>&& json) noexcept
{
	return Mso::Json::Utf8::MakeReader(std::move(json));
}

//inline std::unique_ptr<TJsonInput<char>> MakeInput(ISequentialStream& stream) noexcept
//{
//	return Mso::Json::Utf8::MakeInput(stream);
//}

} // namespace Mso::Json::Ansi
