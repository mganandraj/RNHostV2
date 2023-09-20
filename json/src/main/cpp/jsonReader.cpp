/**
	Terms used in here:

	Group: represents a [] or {}
	isStringContent: is the current value a string? Either a name or quoted value
*/
#include "precomp.h"

#include "jsonReader.h"
#include <stlextensions/algorithms.h>

namespace Mso::Json::Flighting {

static bool s_isEnableIntOverflowChecks = true;
void EnableIntOverflowChecks(bool value) noexcept
{
	s_isEnableIntOverflowChecks = value;
}

} // namespace Mso::Json::Flighting

namespace Mso::Json {

template<typename TChar>
bool IsSimpleWhitespace(TChar c) noexcept
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static uint32_t StringToUI32(const char* tzBegin, _Out_ char** tzEnd, uint32_t base) noexcept
{
	return static_cast<uint32_t>(strtoul(tzBegin, tzEnd, base));
}

static uint32_t StringToUI32(const wchar_t* tzBegin, _Out_ wchar_t** tzEnd, uint32_t base) noexcept
{
	return static_cast<uint32_t>(wcstoul(tzBegin, tzEnd, base));
}

static int32_t StringToI32(const char* tzBegin, _Out_ char** tzEnd, uint32_t base) noexcept
{
	return static_cast<int32_t>(strtol(tzBegin, tzEnd, base));
}

static int32_t StringToI32(const wchar_t* tzBegin, _Out_ wchar_t** tzEnd, uint32_t base) noexcept
{
	return static_cast<int32_t>(wcstol(tzBegin, tzEnd, base));
}

static uint64_t StringToUI64(const char* tzBegin, _Out_ char** tzEnd, uint32_t base) noexcept
{
	errno = 0;
	return strtoull(tzBegin, tzEnd, base);
}

static uint64_t StringToUI64(const wchar_t* tzBegin, _Out_ wchar_t** tzEnd, uint32_t base) noexcept
{
	errno = 0;
	// TODO: return wcstoull(tzBegin, tzEnd, base);
	std::abort();
    return 0;
    // return _wcstoui64(tzBegin, tzEnd, base);
}

static int64_t StringToI64(const char* tzBegin, _Out_ char** tzEnd, uint32_t base) noexcept
{
	errno = 0;
	return strtoll(tzBegin, tzEnd, base);
}

static int64_t StringToI64(const wchar_t* tzBegin, _Out_ wchar_t** tzEnd, uint32_t base) noexcept
{
	errno = 0;
	// TODO: return wcstoll(tzBegin, tzEnd, base);
	// return _wcstoi64(tzBegin, tzEnd, base);
    std::abort();
    return 0;
}

static double StringToDouble(const char* tzBegin, _Out_ char** tzEnd) noexcept
{
	errno = 0;
	return strtod(tzBegin, tzEnd);
}

static double StringToDouble(const wchar_t* tzBegin, _Out_ wchar_t** tzEnd) noexcept
{
	errno = 0;
	return wcstod(tzBegin, tzEnd);
}

/**
	Main processing loop: reads tokens until end of stream, error, or
	a valid state transition is reached.
*/
template<typename TChar>
ParseState::Enum JsonReader<TChar>::ReadNext() noexcept
{
	m_input->FillBuffer();

	for (;; m_input->Inc())
	{
		if (m_input->IsEndOfData())
		{
			// Handle the single top-level non-quoted Value scenario
			if (!m_isStringContent && m_states.size() == 1 && HandleEndGroupOrValue(ParseState::Value))
			{
				return ParseState::Value;
			}

			return ParseState::ErrorEndOfData;
		}

		TChar wch = m_input->PeekChar();

		if (m_isAllowed.fStringChar)
		{
			if (m_isAllowed.fEscapeChar)
			{
				if (HandleEscapeChar(wch))
					continue;
				return HandleInvalidData();
			}

			if (m_isAllowed.fEscapeCharHex)
			{
				if (HandleEscapeCharHex(wch))
					continue;
				return HandleInvalidData();
			}

			if (wch == '\\')
			{
				m_isAllowed.fEscapeChar = 1;
				continue;
			}

			if (wch == '\"')
			{
				if (HandleEndName())
				{
					m_input->Inc();
					return ParseState::Name;
				}

				if (HandleEndGroupOrValue(ParseState::Value))
				{
					m_input->Inc();
					return ParseState::Value;
				}

				// VerifyElseCrashSzTag(false, "This should be impossible to hit", 0x01256808 /* tag_bjw6i */);
			}

			// Regular string character
			m_textBuffer += wch;
			continue;
		}

		// All whitespace outside of strings is ignored
		if (IsSimpleWhitespace(wch))
			continue;

		switch (wch)
		{
			case '[':
				return HandleStartGroup(ParseState::StartArray);
			case ']':
				return HandleEndGroup(ParseState::StartArray, ParseState::EndArray);
			case '{':
				return HandleStartGroup(ParseState::StartObject);
			case '}':
				return HandleEndGroup(ParseState::StartObject, ParseState::EndObject);

			case ',':
			{
				if (m_isAllowed.fEndComma)
				{
					if (HandleEndGroupOrValue(ParseState::Value))
						return ParseState::Value;

					ResetGroupState();
					continue;
				}
				break;
			}

			case '"':
			{
				if (HandleBeginString())
					continue;
				break;
			}

			case ':':
			{
				if (m_isAllowed.fNameDelim)
				{
					m_isAllowed.all = 0;
					OnValueExpected();
					continue;
				}
				break;
			}

			default:
			{
				// Regular non-string character
				if (HandleNonStringChar(wch))
					continue;

				break;
			}
		}

		return HandleInvalidData();
	}
}

/**
	Called when a new Group is started
	Called after each item in a Group
*/
template<typename TChar>
void JsonReader<TChar>::ResetGroupState() noexcept
{
	m_isAllowed.all = 0;

	if (m_states.top() == ParseState::StartArray)
	{
		OnValueExpected();
	}
	else
	{
		m_isAllowed.fBeginName = 1;
		m_isStringContent = false;
		m_textBuffer.clear();
	}
}

/**
	Called after '[', ':', ','
*/
template<typename TChar>
void JsonReader<TChar>::OnValueExpected() noexcept
{
	m_isAllowed.fStartGroup = 1;
	m_isAllowed.fBeginValue = 1;
	m_isStringContent = false;
	m_textBuffer.clear();
}

/**
	Handle the start of a new group: [ {
*/
template<typename TChar>
ParseState::Enum JsonReader<TChar>::HandleStartGroup(ParseState::Enum state) noexcept
{
	if (!m_isAllowed.fStartGroup)
		return HandleInvalidData();

	m_states.push(state);
	ResetGroupState();
	m_isAllowed.fEndGroup = 1;

	m_input->Inc();
	return state;
}

/**
	Handle the end of a group: } ]
*/
template<typename TChar>
ParseState::Enum JsonReader<TChar>::HandleEndGroup(ParseState::Enum oldState, ParseState::Enum newState) noexcept
{
	if (!m_isAllowed.fEndGroup)
		return HandleInvalidData();

	// Check for previously ended value first
	if (HandleEndGroupOrValue(ParseState::Value))
		return ParseState::Value;

	if (HandleEndGroupOrValue(oldState))
	{
		m_isStringContent = false;
		m_textBuffer.clear();

		m_input->Inc();
		return newState;
	}

	return HandleInvalidData();
}

/**
	Handle starting a new string: "
*/
template<typename TChar>
bool JsonReader<TChar>::HandleBeginString() noexcept
{
	if (m_isAllowed.fBeginName || m_isAllowed.fBeginValue)
	{
		m_states.push(m_isAllowed.fBeginName ? ParseState::Name : ParseState::Value);

		m_isAllowed.all = 0;
		m_isAllowed.fStringChar = 1;
		m_isStringContent = true;
		return true;
	}

	return false;
}

/**
	Handle value characters: numbers, true, false, null
*/
template<typename TChar>
bool JsonReader<TChar>::HandleNonStringChar(TChar wch) noexcept
{
	// FUTURE: could add more validation here

	if (m_isAllowed.fNonStringChar)
	{
		m_textBuffer += wch;
		return true;
	}

	if (m_isAllowed.fBeginValue)
	{
		m_isAllowed.all = 0;
		m_isAllowed.fNonStringChar = 1;
		m_isAllowed.fEndComma = 1;
		m_isAllowed.fEndGroup = 1;
		m_states.push(ParseState::Value);
		m_textBuffer.clear();

		m_textBuffer += wch;
		return true;
	}

	return false;
}

/**
	Handle an escaped string character
*/
template<typename TChar>
bool JsonReader<TChar>::HandleEscapeChar(TChar wch) noexcept
{
	switch (wch)
	{
		case '\"':
		case '\\':
		case '/':
			break;
		case 'b':
			wch = '\b';
			break;
		case 'f':
			wch = '\f';
			break;
		case 'n':
			wch = '\n';
			break;
		case 'r':
			wch = '\r';
			break;
		case 't':
			wch = '\t';
			break;
		case 'v':
			wch = '\v';
			break;

		case 'u':
			// Switch to hex-mode
			m_isAllowed.fEscapeChar = 0;
			m_isAllowed.fEscapeCharHex = 1;
			m_hexStartIndex = m_textBuffer.length();
			return true;

		default:
			return false;
	}

	m_textBuffer += wch;

	m_isAllowed.fEscapeChar = 0;
	return true;
}

/**
	Appends the current char into the buffer
	If all hex characters have been collected, try converting to character
*/
template<typename TChar>
bool JsonReader<TChar>::HandleEscapeCharHex(TChar wch) noexcept
{
	static const size_t c_cchHex = 4;

	m_textBuffer += wch;
	if (m_textBuffer.length() < m_hexStartIndex + c_cchHex)
		return true;

	const TChar* wzHexStart = m_textBuffer.c_str() + m_hexStartIndex;
	TChar* wzHexEnd = nullptr;
	wch = static_cast<TChar>(StringToUI32(wzHexStart, &wzHexEnd, 16/*base*/));

	if (wzHexStart + c_cchHex != wzHexEnd)
		return false;

	m_textBuffer.resize(m_hexStartIndex);
	m_textBuffer += wch;

	m_isAllowed.fEscapeCharHex = 0;
	return true;
}

/**
	Helper for handling the end of a group or value: , ] }
*/
template<typename TChar>
bool JsonReader<TChar>::HandleEndGroupOrValue(ParseState::Enum state) noexcept
{
	if (m_states.top() == state)
	{
		m_states.pop();

		m_isAllowed.all = 0;
		if (m_states.size() > 0)
		{
			m_isAllowed.fEndComma = 1;
			m_isAllowed.fEndGroup = 1;
		}

		return true;
	}

	return false;
}

/**
	Handle the end of a name: "
*/
template<typename TChar>
bool JsonReader<TChar>::HandleEndName() noexcept
{
	if (m_states.top() == ParseState::Name)
	{
		m_states.pop();
		m_isAllowed.all = 0;
		m_isAllowed.fNameDelim = 1;
		return true;
	}

	return false;
}

/*------------------------------------------------------------------------------
	Return the current string literal
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<Mso::basic_string_view<TChar>> JsonReader<TChar>::TryGetString() const noexcept
{
	if (m_isStringContent)
	{
		return Mso::basic_string_view<TChar>(m_textBuffer);
	}

	return {};
}

/*------------------------------------------------------------------------------
	Return the current value as a bool
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<bool> JsonReader<TChar>::TryGetBool() const noexcept
{
	if (!m_isStringContent)
	{
		if (std::ranges::equal(m_textBuffer, std::string_view("true")))
		{
			return true;
		}

		if (std::ranges::equal(m_textBuffer, std::string_view("false")))
		{
			return false;
		}
	}

	return {};
}
    template<typename To, typename From>
    constexpr std::optional<To> TrySafeCast(From from) noexcept
    {
        To to;
        if (SafeCast(from, to))
            return to;
        return {};
    }

/*------------------------------------------------------------------------------
	Return the current value as an Int
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<int32_t> JsonReader<TChar>::TryGetInt32() const noexcept
{
	if (!m_isStringContent && m_textBuffer.length())
	{
		if (Mso::Json::Flighting::s_isEnableIntOverflowChecks)
		{
			auto opt64 = this->TryGetInt64();
			if (opt64)
			{
				return static_cast<int32_t>(*opt64); // TrySafeCast<int32_t>(*opt64);
			}
		}
		else
		{
			TChar* pwchEnd = nullptr;
			auto value = StringToI32(m_textBuffer.c_str(), &pwchEnd, 10 /*base*/);
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size())
			{
				return static_cast<int>(value);
			}
		}

		// Didn't read enough characters, fall through and fail
	}

	return {};
}

/*------------------------------------------------------------------------------
	Return the current value as an Int64
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<int64_t> JsonReader<TChar>::TryGetInt64() const noexcept
{
	if (!m_isStringContent && m_textBuffer.length())
	{
		TChar* pwchEnd = nullptr;
		auto value = StringToI64(m_textBuffer.c_str(), &pwchEnd, 10 /*base*/);
		if (Mso::Json::Flighting::s_isEnableIntOverflowChecks)
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size() && errno != ERANGE)
			{
				return value;
			}
		}
		else
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size())
			{
				return value;
			}
		}

		// Didn't read enough characters, fall through and fail
	}

	return {};
}

/*------------------------------------------------------------------------------
	Return the current value as an std::uint64_t
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<uint64_t> JsonReader<TChar>::TryGetUInt64() const noexcept
{
	if (!m_isStringContent && m_textBuffer.length())
	{
		TChar* pwchEnd = nullptr;
		auto value = StringToUI64(m_textBuffer.c_str(), &pwchEnd, 10 /*base*/);
		if (Mso::Json::Flighting::s_isEnableIntOverflowChecks)
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size() && errno != ERANGE)
			{
				return value;
			}
		}
		else
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size())
			{
				return value;
			}
		}

		// Didn't read enough characters, fall through and fail
	}

	return {};
}

/*------------------------------------------------------------------------------
	Return the current value as a Double
------------------------------------------------------------------------------*/
template<typename TChar>
std::optional<double> JsonReader<TChar>::TryGetDouble() const noexcept
{
	if (!m_isStringContent && m_textBuffer.length())
	{
		TChar* pwchEnd = nullptr;
		auto value = StringToDouble(m_textBuffer.c_str(), &pwchEnd);
		if (Mso::Json::Flighting::s_isEnableIntOverflowChecks)
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size() && errno != ERANGE)
			{
				return value;
			}
		}
		else
		{
			if (pwchEnd == m_textBuffer.c_str() + m_textBuffer.size())
			{
				return value;
			}
		}

		// Didn't read enough characters, fall through and fail
	}

	return {};
}

/*------------------------------------------------------------------------------
	Return true if the current value is the null value
------------------------------------------------------------------------------*/
template<typename TChar>
bool JsonReader<TChar>::IsNull() const noexcept
{
	if (!m_isStringContent)
	{
		return std::ranges::equal(m_textBuffer, std::string_view("null"));
	}

	return false;
}

template<typename TChar>
std::optional<std::pair<Mso::basic_string_view<TChar>, bool>> JsonReader<TChar>::TryGetRawValue() const noexcept
{
	return std::make_pair(Mso::basic_string_view<TChar>(m_textBuffer), m_isStringContent);
}

namespace Details {

template<typename TChar>
static Mso::CntPtr<TIReader<TChar>> Make(std::unique_ptr<TIInput<TChar>>&& json) noexcept
{
    // GAMMA
    return Mso::Make<JsonReader<TChar>, TIReader<TChar>>(std::move(json));
	// return Mso::CntPtr<Mso::Json::TIReader<TChar>> { Mso::Make<JsonReader<TChar>, TIReader<TChar>>(std::move(json)).Detach(), false };
}

template<typename TChar>
static Mso::CntPtr<TIReader<TChar>> Make(const Mso::basic_string_view<TChar>& json) noexcept
{
	return Make<TChar>(std::make_unique<TIInputString<TChar>>(json));
}

} // Details

Mso::CntPtr<Utf8::IReader> Utf8::MakeReader(const Mso::basic_string_view<char>& json) noexcept
{
	return Details::Make(json);
}

Mso::CntPtr<Utf8::IReader> Utf8::MakeReader(std::unique_ptr<IInput>&& json) noexcept
{
	return Details::Make(std::move(json));
}

Mso::CntPtr<Unicode::IReader> Unicode::MakeReader(const Mso::basic_string_view<wchar_t>& json) noexcept
{
	return Details::Make(json);
}

Mso::CntPtr<Unicode::IReader> Unicode::MakeReader(std::unique_ptr<IInput>&& json) noexcept
{
	return Details::Make(std::move(json));
}
//
//std::unique_ptr<TIInput<char>> Utf8::MakeInput(ISequentialStream& stream) noexcept
//{
//	return std::make_unique<TIInputStream<char>>(stream);
//}
//
//std::unique_ptr<TIInput<wchar_t>> Unicode::MakeInput(ISequentialStream& stream) noexcept
//{
//	return std::make_unique<TIInputStream<wchar_t>>(stream);
//}

} // namespace
