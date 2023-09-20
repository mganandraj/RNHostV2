#include "precomp.h"
#include <crash/verifyElseCrash.h>
#include <stringcore/parsing.h>
#include "writer.h"
#include "stringrefoutput.h"
#include <charconv>
#include <cmath>

namespace Mso::Json {

template<typename TChar, typename TValue>
static void WriteNumericValue(Mso::basic_string<TChar>& output, const TValue& value) noexcept
{
	// 100 chars should be plenty to hold any numeric value
	char buffer[100];
	auto [end, ec] = std::to_chars(std::begin(buffer), std::end(buffer), value);
	VerifyElseCrashSzTag(ec == std::errc(), "Conversion can't fail", 0x1e54568a /* tag_4vf0k */);

	for (auto ptr = buffer; ptr < end; ++ptr)
		output += *ptr;
}

/*
	Not all platforms provide to_chars for floating point yet
	Use the Mso function for now
*/
template<typename TChar>
static void WriteNumericValue(Mso::basic_string<TChar>& output, const double& value) noexcept
{
	// JSON doesn't support inf or nan so write null instead
	wchar_t buffer[100] = L"null";
	if (std::isfinite(value))
	{
		auto cch = MsoWzDecodeDouble(buffer, _countof(buffer), value);
		VerifyElseCrashSzTag(cch, "Conversion can't fail", 0x1e561059 /* tag_4v7bz */);
	}

	for (auto ptr = buffer; *ptr; ++ptr)
	{
		output += static_cast<TChar>(*ptr);
	}
}

template<typename TChar>
static void WriteNumericValue(Mso::basic_string<TChar>& output, const float& value) noexcept
// UBSAN_DISABLE("float-cast-overflow") // VSO#5923871 Temporarily ignore float-cast-overflow
{
	double dValue = NAN;
	if (std::isfinite(value))
	{
		// Use WriteDouble after rounding off to 7 significant digits.
		dValue = static_cast<double>(value);
		if (dValue != 0)
		{
			constexpr int cFloatSignificantDigits = 7;
			int power = cFloatSignificantDigits - static_cast<int>(floor(log10(abs(dValue)))) - 1;
			dValue = round(dValue * pow(10, power)) * pow(10, -power);
		}
	}

	return WriteNumericValue(output, dValue);
}

/**
* Write the \u00NN syntax for control characters
*/
template<typename TChar>
static void WriteEscapedControlChar(_Out_cap_c_(6) TChar*& escapeChars, TChar wch) noexcept
{
	*escapeChars++ = '\\';
	*escapeChars++ = 'u';
	*escapeChars++ = '0';
	*escapeChars++ = '0';

	if (wch <= L'\x09')
	{
		*escapeChars++ = '0';
		*escapeChars++ = (wch + '0');
	}
	else if (wch <= L'\x0F')
	{
		*escapeChars++ = '0';
		*escapeChars++ = (wch - 0x0A + 'A');
	}
	else if (wch <= L'\x19')
	{
		*escapeChars++ = '1';
		*escapeChars++ = (wch - 0x10 + '0');
	}
	else
	{
		*escapeChars++ = '1';
		*escapeChars++ = (wch - 0x1A + 'A');
	}
}

/**
* Escape special characters, optimized for no special characters.
*/
template<typename TChar>
void TWriter<TChar>::WriteEscapedString(const Mso::basic_string_view<TChar>& value) noexcept
{
	auto start = std::begin(value);
	for (auto current = start; current != std::end(value); ++current)
	{
		TChar escapeCharsStart[6];
		TChar* escapeChars = escapeCharsStart;
		switch (*current)
		{
			default:
			{
				if (*current > '\x1F')
					continue;

				WriteEscapedControlChar(escapeChars, *current);
				break;
			}

			case '"':
				*escapeChars++ = '\\';
				*escapeChars++ = '"';
				break;
			case '\\':
				*escapeChars++ = '\\';
				*escapeChars++ = '\\';
				break;
			case '\b':
				*escapeChars++ = '\\';
				*escapeChars++ = 'b';
				break;
			case '\f':
				*escapeChars++ = '\\';
				*escapeChars++ = 'f';
				break;
			case '\n':
				*escapeChars++ = '\\';
				*escapeChars++ = 'n';
				break;
			case '\r':
				*escapeChars++ = '\\';
				*escapeChars++ = 'r';
				break;
			case '\t':
				*escapeChars++ = '\\';
				*escapeChars++ = 't';
				break;
		}

		// Write any existing non-escaped characters
		// TODO: C++20 use new string_view constructors
		WriteRawChars({ &*start, static_cast<size_t>(current - start)});
		start = current + 1;

		m_outputBuffer.append(escapeCharsStart, escapeChars);
	}

	// Write any remaining non-escaped characters
	if (start != std::end(value))
	{
		WriteRawChars({ &*start, static_cast<size_t>(std::end(value) - start) });
	}
}

template<typename TChar>
bool TWriter<TChar>::IsValueAllowed() const noexcept
{
	if (!m_optState)
		return true;

	switch (*m_optState)
	{
		case ParseState::Name:
		case ParseState::StartArray:
			return true;

		case ParseState::EndArray:
		case ParseState::EndObject:
		case ParseState::Value:
			return (m_stack.top() == ParseState::StartArray);

		default:
			return false;
	}
}

template<typename TChar>
bool TWriter<TChar>::IsNameOrEndObjectAllowed() const noexcept
{
	if (m_stack.empty())
		return false;
	if (m_stack.top() != ParseState::StartObject)
		return false;
	if (*m_optState == ParseState::Name)
		return false;
	return true;
}

template<typename TChar>
bool TWriter<TChar>::IsEndArrayAllowed() const noexcept
{
	if (m_stack.empty())
		return false;
	if (m_stack.top() != ParseState::StartArray)
		return false;
	return true;
}

template<typename TChar>
void TWriter<TChar>::StartObject() noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561058 /* tag_4v7by */);

	AddCommaIfAppending();
	m_outputBuffer += '{';
	m_stack.push(ParseState::StartObject);

	OnStateChange(ParseState::StartObject);
}

template<typename TChar>
void TWriter<TChar>::EndObject() noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsNameOrEndObjectAllowed(), 0x1e561057 /* tag_4v7bx */);

	m_outputBuffer += '}';
	m_stack.pop();

	OnStateChange(ParseState::EndObject);
}

template<typename TChar>
void TWriter<TChar>::StartArray() noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561056 /* tag_4v7bw */);

	AddCommaIfAppending();
	m_outputBuffer += '[';
	m_stack.push(ParseState::StartArray);

	OnStateChange(ParseState::StartArray);
}

template<typename TChar>
void TWriter<TChar>::EndArray() noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsEndArrayAllowed(), 0x1e561055 /* tag_4v7bv */);

	m_outputBuffer += L']';
	m_stack.pop();

	OnStateChange(ParseState::EndArray);
}

template<typename TChar>
void TWriter<TChar>::WriteName(const Mso::basic_string_view<TChar>& name) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsNameOrEndObjectAllowed(), 0x1e561054 /* tag_4v7bu */);

	AddCommaIfAppending();
	m_outputBuffer += '"';
	WriteEscapedString(name);
	m_outputBuffer += '"';

	m_outputBuffer += ':';
	if (m_prettyPrint == PrettyPrint::Spaces)
	{
		m_outputBuffer += ' ';
	}

	OnStateChange(ParseState::Name);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(const Mso::basic_string_view<TChar>& value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561053 /* tag_4v7bt */);

	AddCommaIfAppending();
	m_outputBuffer += '"';
	WriteEscapedString(value);
	m_outputBuffer += '"';

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(const Mso::basic_string_view<TChar>& value, const rawValue_t&) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561052 /* tag_4v7bs */);

	AddCommaIfAppending();
	WriteRawChars(value);

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(bool value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561051 /* tag_4v7br */);

	AddCommaIfAppending();
	if (value)
	{
		m_outputBuffer += 't';
		m_outputBuffer += 'r';
		m_outputBuffer += 'u';
		m_outputBuffer += 'e';
	}
	else
	{
		m_outputBuffer += 'f';
		m_outputBuffer += 'a';
		m_outputBuffer += 'l';
		m_outputBuffer += 's';
		m_outputBuffer += 'e';
	}

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(int64_t value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e561050 /* tag_4v7bq */);

	AddCommaIfAppending();
	WriteNumericValue(m_outputBuffer, value);

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(uint64_t value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e56104f /* tag_4v7bp */);

	AddCommaIfAppending();
	WriteNumericValue(m_outputBuffer, value);

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(double value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e56104e /* tag_4v7bo */);

	AddCommaIfAppending();
	WriteNumericValue(m_outputBuffer, value);

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(float value) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e56104d /* tag_4v7bn */);

	AddCommaIfAppending();
	WriteNumericValue(m_outputBuffer, value);

	OnStateChange(ParseState::Value);
}

template<typename TChar>
void TWriter<TChar>::WriteValue(std::nullptr_t) noexcept
{
	if (IsInvalid())
		return;
	VerifyElseCrashTag(IsValueAllowed(), 0x1e56104c /* tag_4v7bm */);

	AddCommaIfAppending();
	m_outputBuffer += 'n';
	m_outputBuffer += 'u';
	m_outputBuffer += 'l';
	m_outputBuffer += 'l';

	OnStateChange(ParseState::Value);
}

/**
	Add a comma if appending to a list
*/
template<typename TChar>
void TWriter<TChar>::AddCommaIfAppending() noexcept
{
	if (m_optState)
	{
		switch (*m_optState)
		{
			case ParseState::EndArray:
			case ParseState::EndObject:
			case ParseState::Value:
			{
				m_outputBuffer += ',';
				if (m_prettyPrint == PrettyPrint::Spaces)
				{
					m_outputBuffer += ' ';
				}
				break;
			}

			default:
				break;
		}
	}
}

/**
* This method will write value directly if it is large. This avoids needing to 
* increase m_buffer for large strings.
*/
template<typename TChar>
void TWriter<TChar>::WriteRawChars(const Mso::basic_string_view<TChar>& value) noexcept
{
	if (IsInvalid())
		return;

	if (value.size() < m_outputBuffer.capacity() - m_outputBuffer.size())
	{
		m_outputBuffer += value;
	}
	else
	{
		WriteBuffer();
		if (IsInvalid())
			return;

		if (!m_output->TryWrite(value))
		{
			m_optState = ParseState::ErrorInvalidData;
		}
	}
}

/**
	Called after a Write() operation is completed to update the state
	Flushes the buffer on completion or when it's 'mostly' full.
*/
template<typename TChar>
void TWriter<TChar>::OnStateChange(ParseState::Enum newState) noexcept
{
	if (m_stack.empty())
	{
		// Done parsing
		m_optState = ParseState::ErrorEndOfData;
	}
	else
	{
		m_optState = newState;

		// Only flush when the buffer is close to full
		if (m_outputBuffer.size() <= c_bufferSize * 3 / 4)
			return;
	}

	WriteBuffer();
}

template<typename TChar>
void TWriter<TChar>::WriteBuffer() noexcept
{
	if (m_output->TryWrite(m_outputBuffer))
	{
		m_outputBuffer.resize(0);
	}
	else
	{
		m_optState = ParseState::ErrorInvalidData;
	}
}

// Force the class to exist so all the functions exist in the android lib
template class TWriter<char>;
template class TWriter<wchar_t>;

} // namespace Mso::Json

namespace Mso::Json::Utf8 {

std::unique_ptr<IWriter> MakeWriter(IOutput& output, PrettyPrint prettyPrint) noexcept
{
	return std::make_unique<TWriter<char>>(output, prettyPrint);
}

std::unique_ptr<IWriter> MakeWriter(std::string& output, PrettyPrint prettyPrint) noexcept
{
	return MakeWriter(*MakeStringRefOutput(output), prettyPrint);
}

} // namespace Mso::Json::Utf8

namespace Mso::Json::Unicode {

std::unique_ptr<IWriter> MakeWriter(IOutput& output, PrettyPrint prettyPrint) noexcept
{
	return std::make_unique<TWriter<wchar_t>>(output, prettyPrint);
}

std::unique_ptr<IWriter> MakeWriter(std::wstring& output, PrettyPrint prettyPrint) noexcept
{
	return MakeWriter(*MakeStringRefOutput(output), prettyPrint);
}

} // namespace Mso::Json::Unicode
