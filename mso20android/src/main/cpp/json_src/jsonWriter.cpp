#include "precomp.h"
#include <crash/verifyElseCrash.h>
#include <stringcore/parsing.h>
#include "jsonWriter.h"
#include "writer.h"
#include "stringrefoutput.h"

namespace Mso::Json::Flighting {

static bool s_enableNewWriter = false;

void EnableNewWriter(bool value) noexcept
{
	s_enableNewWriter = value;
}

} // namespace Mso::Json::Flighting

/**
	Legacy JsonWriter codepaths
*/
namespace Mso::Json {

/*!
	JsonWriter::StartObject
*/
bool JsonWriter::StartObject() noexcept
{
	if (!StartObjectAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten == ParseState::EndObject ||
			lastWritten == ParseState::Value ||
			lastWritten == ParseState::EndArray)
			resultString += this->m_shouldPrettyPrint ? L", {" : L",{";
		else
			resultString += L"{";

		stack.push(ParseState::StartObject);
		lastWritten = ParseState::StartObject;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::EndObject
*/
bool JsonWriter::EndObject() noexcept
{
	if (!EndObjectAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		resultString += L"}";
		stack.pop();
		lastWritten = ParseState::EndObject;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::StartArray
*/
bool JsonWriter::StartArray() noexcept
{
	if (!StartArrayAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten == ParseState::EndArray ||
			lastWritten == ParseState::Value ||
			lastWritten == ParseState::EndObject)
			resultString += this->m_shouldPrettyPrint ? L", [" : L",[";
		else
			resultString += L"[";

		stack.push(ParseState::StartArray);
		lastWritten = ParseState::StartArray;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::EndArray
*/
bool JsonWriter::EndArray() noexcept
{
	if (!EndArrayAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		resultString += L"]";
		stack.pop();
		lastWritten = ParseState::EndArray;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteName
*/
bool JsonWriter::WriteName(_In_z_ const wchar_t* wzName) noexcept
{
	VerifyElseCrashTag(wzName != nullptr, 0x004552d9 /* tag_arvlz */);

	if (!NameAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten == ParseState::StartObject)
			resultString += L"\"";
		else
			resultString += this->m_shouldPrettyPrint ? L", \"" : L",\"";

		WriteEscapedString(wzName);
		resultString += this->m_shouldPrettyPrint ? L"\": " : L"\":";

		lastWritten = ParseState::Name;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteString
*/
bool JsonWriter::WriteString(_In_z_ const wchar_t* wzValue) noexcept
{
	VerifyElseCrashTag(wzValue != nullptr, 0x004552da /* tag_arvl0 */);

	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten == ParseState::Name || lastWritten == ParseState::StartArray)
			resultString += L"\"";
		else
			resultString += this->m_shouldPrettyPrint ? L", \"" : L",\"";

		WriteEscapedString(wzValue);
		resultString += L"\"";

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteBool
*/
bool JsonWriter::WriteBool(bool fValue) noexcept
{
	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten != ParseState::Name && lastWritten != ParseState::StartArray)
			resultString += this->m_shouldPrettyPrint ? L", " : L",";

		resultString += fValue ? L"true" : L"false";

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteInt
*/
bool JsonWriter::WriteInt(int64_t iValue) noexcept
{
	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten != ParseState::Name && lastWritten != ParseState::StartArray)
			resultString += this->m_shouldPrettyPrint ? L", " : L",";

		resultString += std::to_wstring(iValue);

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteUInt64
*/
bool JsonWriter::WriteUInt64(uint64_t ui64Value) noexcept
{
	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten != ParseState::Name && lastWritten != ParseState::StartArray)
			resultString += this->m_shouldPrettyPrint ? L", " : L",";

		resultString += std::to_wstring(ui64Value);

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteDouble
*/
bool JsonWriter::WriteDouble(double dValue) noexcept
{
	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten != ParseState::Name && lastWritten != ParseState::StartArray)
			resultString += this->m_shouldPrettyPrint ? L", " : L",";

		WCHAR wzValue[32] {};
		if (MsoWzDecodeDouble(RgC(wzValue), dValue) == 0)
			return false;
		resultString += wzValue;

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteFloat
*/
bool JsonWriter::WriteFloat(float fpValue) noexcept
// UBSAN_DISABLE("float-cast-overflow") // VSO#5923871 Temporarily ignore float-cast-overflow
{
	// Use WriteDouble after rounding off to 7 significant digits.
	double dValue = static_cast<double>(fpValue);
	if (dValue != 0)
	{
		constexpr int cFloatSignificantDigits = 7;
		int power = cFloatSignificantDigits - static_cast<int>(floor(log10(dValue))) - 1;
		dValue = round(dValue * pow(10, power)) * pow(10, -power);
	}

	return WriteDouble(dValue);
}

/*!
	JsonWriter::WriteNull
*/
bool JsonWriter::WriteNull() noexcept
{
	if (!NullAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten == ParseState::Name || lastWritten == ParseState::StartArray)
			resultString += L"null";
		else
			resultString += this->m_shouldPrettyPrint ? L", null" : L",null";

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::WriteRawChars
*/
bool JsonWriter::WriteRawChars(_In_z_ const wchar_t* wzRawChars) noexcept
{
	VerifyElseCrashTag(wzRawChars != nullptr, 0x0164e10c /* tag_bzoem */);

	if (!ValueAllowed())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		if (lastWritten != ParseState::Name && lastWritten != ParseState::StartArray)
			resultString += this->m_shouldPrettyPrint ? L", " : L",";

		resultString += wzRawChars;

		lastWritten = ParseState::Value;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		m_isInvalid = true;
		return false;
	}
}

/*!
	JsonWriter::GetResultData
*/
bool JsonWriter::GetResultData(_Out_ std::wstring& value) const noexcept
{
	value.clear();

	if (m_isInvalid || resultString.empty() || !stack.empty())
		return false;

	OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
	try
	{
		value = resultString;
		return true;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

std::optional<std::wstring> JsonWriter::TryExtractOutput() noexcept
{
	if (m_isInvalid || resultString.empty() || !stack.empty())
		return {};

	return std::move(resultString);
}

/*!
	JsonWriter::StartObjectAllowed
*/
bool JsonWriter::StartObjectAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (resultString.empty() || ValueAllowed())
	{
		return true;
	}

	return false;
}
/*!
	JsonWriter::EndObjectAllowed
*/
bool JsonWriter::EndObjectAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (!stack.empty() && stack.top() == ParseState::StartObject &&
		(lastWritten == ParseState::Value || lastWritten == ParseState::EndArray ||
		lastWritten == ParseState::EndObject || lastWritten == ParseState::StartObject))
	{
		return true;
	}

	return false;
}

/*!
	JsonWriter::StartArrayAllowed
*/
bool JsonWriter::StartArrayAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (resultString.empty() || ValueAllowed())
	{
		return true;
	}

	return false;

}

/*!
	JsonWriter::EndArrayAllowed
*/
bool JsonWriter::EndArrayAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (!stack.empty() && stack.top() == ParseState::StartArray &&
		(lastWritten == ParseState::Value || lastWritten == ParseState::EndArray ||
		lastWritten == ParseState::EndObject || lastWritten == ParseState::StartArray))
	{
		return true;
	}

	return false;
}

/*!
	JsonWriter::NameAllowed
*/
bool JsonWriter::NameAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (!stack.empty() && stack.top() == ParseState::StartObject && lastWritten != ParseState::Name)
	{
		return true;
	}

	return false;
}

/*!
	JsonWriter::ValueAllowed
*/
bool JsonWriter::ValueAllowed() const noexcept
{
	if (m_isInvalid)
		return false;
	
	if (lastWritten == ParseState::Name || lastWritten == ParseState::StartArray ||
	(!stack.empty() && stack.top() == ParseState::StartArray &&
		(lastWritten == ParseState::Value ||   // previous sibling was a value
		lastWritten == ParseState::EndArray || // previous sibling was a nested array
		lastWritten == ParseState::EndObject))) // previous sibling was a nested object
	{
		return true;
	}

	return false;
}

/*!
	JsonWriter::NullAllowed
*/
bool JsonWriter::NullAllowed() const noexcept
{
	if (m_isInvalid)
		return false;

	if (lastWritten == ParseState::Name || (!stack.empty() && stack.top() == ParseState::StartArray))
	{
		return true;
	}

	return false;
}

/**
	Escapes quotation marks, slashes, and control characters per the JSON
	format specification.  Should be called on all input strings before
	appending to the result.
	The control characters are code points 0x00-0x1F.
	Some control characters are represented as two character sequence.
*/
void JsonWriter::WriteEscapedString(_In_z_ LPCWSTR wz)
{
	if (wz == nullptr || wz[0] == L'\0')
		return;

	wchar_t wch{ L'\0' };
	for (int i = 0; (wch = wz[i]) != L'\0'; ++i)
	{
		switch (wch)
		{
		default:
			if (wch > L'\x1F')
				resultString += wch;
			else
				WriteEscapedControlChar( wch);
			break;
		case L'\"':
			resultString += LR"(\")";
			break;
		case L'\\':
			resultString += LR"(\\)";
			break;
		case L'\b':
			resultString += LR"(\b)";
			break;
		case L'\f':
			resultString += LR"(\f)";
			break;
		case L'\n':
			resultString += LR"(\n)";
			break;
		case L'\r':
			resultString += LR"(\r)";
			break;
		case L'\t':
			resultString += LR"(\t)";
			break;
		}
	}
}

/**
	Escapes control characters as an unicode sequence '\uXXXX'.
*/
void JsonWriter::WriteEscapedControlChar(wchar_t wch)
{
	// Each control character is escaped as six characters.
	OACR_POSSIBLE_THROW;
	resultString.reserve(resultString.size() + 6);

	if (wch <= L'\x09')
	{
		resultString += LR"(\u000)";
		resultString += (wch + L'0');
	}
	else if (wch <= L'\x0F')
	{
		resultString += LR"(\u000)";
		resultString += (wch - 0x0A + L'A');
	}
	else if (wch <= L'\x19')
	{
		resultString += LR"(\u001)";
		resultString += (wch - 0x10 + L'0');
	}
	else
	{
		resultString += LR"(\u001)";
		resultString += (wch - 0x1A + L'A');
	}
}

} // Mso::Json

namespace Mso::Json::Unicode {

/**
	Wrapper class for legacy callers
*/
class StringWriter final : public Mso::RefCountedObject<IJsonWriter>
{
public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(StringWriter);
	StringWriter(PrettyPrint prettyPrint) noexcept : m_writer(*MakeStringRefOutput(m_output), prettyPrint)
	{
	}

	bool StartObject() noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.StartObject(); 
		return true;
	}

	bool EndObject() noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsNameOrEndObjectAllowed())
			return false;
		m_writer.EndObject();
		return true;
	}

	bool StartArray() noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.StartArray();
		return true;
	}

	bool EndArray() noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsEndArrayAllowed())
			return false;
		m_writer.EndArray();
		return true;
	}

	bool WriteName(_In_z_ const wchar_t* wzName) noexcept override
	{
		VerifyElseCrashTag(wzName, 0x1e56104b /* tag_4v7bl */);
		if (m_writer.IsInvalid() || !m_writer.IsNameOrEndObjectAllowed())
			return false;
		m_writer.WriteName(wzName);
		return true;
	}

	bool WriteString(_In_z_ const wchar_t* wzValue) noexcept override
	{ 
		VerifyElseCrashTag(wzValue, 0x1e56104a /* tag_4v7bk */);
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(wzValue);
		return true;
	}

	bool WriteBool(bool value) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(value);
		return true;
	}

	bool WriteInt(int64_t value) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(value);
		return true;
	}

	bool WriteUInt64(uint64_t value) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(value);
		return true;
	}

	bool WriteDouble(double value) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(value);
		return true;
	}

	bool WriteFloat(float value) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(value);
		return true;
	}

	bool WriteNull() noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(nullptr);
		return true;
	}

	bool WriteRawChars(_In_z_ const wchar_t* wzRawChars) noexcept override
	{
		if (m_writer.IsInvalid() || !m_writer.IsValueAllowed())
			return false;
		m_writer.WriteValue(wzRawChars, rawValue);
		return true;
	}

	bool IsInvalid() const noexcept override { return m_writer.IsInvalid(); }

	bool GetResultData(_Out_ std::wstring& value) const noexcept override
	{
		if (m_writer.IsDone())
		{
			OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Purposely want to handle OOM here");
			try
			{
				value = m_output;
				return true;
			}
			catch (const std::bad_alloc&)
			{
			}
		}

		value = {};
		return false;
	}

	std::optional<std::wstring> TryExtractOutput() noexcept override
	{
		if (m_writer.IsDone())
		{
			return std::move(m_output);
		}

		return {};
	}

private:
	virtual ~StringWriter() = default;

	std::wstring m_output;
	TWriter<wchar_t> m_writer;
};

} // namespace Mso::Json::Unicode

namespace Mso::Json {

/*!
	CreateJsonWriter
*/
Mso::CntPtr<IJsonWriter> CreateJsonWriter() noexcept
{
	if (Mso::Json::Flighting::s_enableNewWriter)
	{
		return Mso::Make<Mso::Json::Unicode::StringWriter, IJsonWriter>(PrettyPrint::None);
	}
	else
	{
		return Mso::Make<JsonWriter, IJsonWriter>(false);
	}
}

Mso::CntPtr<IJsonWriter> CreatePrettyPrintingJsonWriter() noexcept
{
	if (Mso::Json::Flighting::s_enableNewWriter)
	{
		return Mso::Make<Mso::Json::Unicode::StringWriter, IJsonWriter>(PrettyPrint::Spaces);
	}
	else
	{
		return Mso::Make<JsonWriter, IJsonWriter>(true);
	}
}


} // namespace Mso::Json
