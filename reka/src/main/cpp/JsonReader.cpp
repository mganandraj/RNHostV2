#include "precomp.h"
#include <reka/JsonReader.h>
#include "StringConversion.h"

namespace Mso::JSHost {

template <typename TChar>
class JsonRekaReader : public Mso::UnknownObject<IRekaReader>
{
public:
	using MakeJsonReader = Mso::TCntPtr<Mso::Json::TIReader<TChar>>(const Mso::basic_string<TChar>&) noexcept;

	JsonRekaReader(const TChar* jsonReader, MakeJsonReader* makeReader) noexcept;

public: // IRekaReader interface
	bool ReadObjectStart() noexcept override;
	bool ReadBool(bool& value) noexcept override;
	bool ReadInt8(int8_t& value) noexcept override;
	bool ReadInt16(int16_t& value) noexcept override;
	bool ReadInt32(int32_t& value) noexcept override;
	bool ReadInt64(int64_t& value) noexcept override;
	bool ReadUInt8(uint8_t& value) noexcept override;
	bool ReadUInt16(uint16_t& value) noexcept override;
	bool ReadUInt32(uint32_t& value) noexcept override;
	bool ReadUInt64(uint64_t& value) noexcept override;
	bool ReadDouble(double& value) noexcept override;
	bool ReadFloat(float& value) noexcept override;
	bool IsNull() noexcept override;
	bool ReadNextProperty() noexcept override;
	bool ReadNextArrayItem() noexcept override;
	bool UnknownProperty() noexcept override;
	bool IsSucceeded() const noexcept override;

protected:
	Mso::Json::ParseState::Enum GetNextState() noexcept;
	bool GetStringValue() noexcept;

	//private:
	Mso::basic_string<TChar> m_jsonText;
	Mso::TCntPtr<Mso::Json::TIReader<TChar>> m_reader;
	Mso::Json::ParseState::Enum m_state{Mso::Json::ParseState::ErrorInvalidData};
	Mso::basic_string_view<TChar> m_propertyName;
	Mso::basic_string_view<TChar> m_stringValue;
	bool m_isSucceeded { true };
};

class Utf16JsonRekaReader : public JsonRekaReader<wchar_t>
{
public:
	Utf16JsonRekaReader(const wchar_t* jsonText) noexcept;

public: // IRekaReader interface
	bool ReadString(std::string& value) noexcept override;
	bool ReadWString(std::wstring& value) noexcept override;
	void GetPropertyName(std::wstring& name) noexcept override;
	bool MatchPropertyName(const wchar_t* name) noexcept override;

private:
	static Mso::TCntPtr<Mso::Json::TIReader<wchar_t>> MakeReader(const Mso::basic_string<wchar_t>& jsonText) noexcept;
};

class Utf8JsonRekaReader : public JsonRekaReader<char>
{
public:
	Utf8JsonRekaReader(const char* jsonText) noexcept;

public: // IRekaReader interface
	bool ReadString(std::string& value) noexcept override;
	bool ReadWString(std::wstring& value) noexcept override;
	void GetPropertyName(std::wstring& name) noexcept override;
	bool MatchPropertyName(const wchar_t* name) noexcept override;

private:
	static Mso::TCntPtr<Mso::Json::TIReader<char>> MakeReader(const Mso::basic_string<char>& jsonText) noexcept;
};

//=============================================================================
// JsonRekaReader implementation.
//=============================================================================

template <typename TChar>
JsonRekaReader<TChar>::JsonRekaReader(const TChar* jsonText, MakeJsonReader* makeReader) noexcept
	: m_jsonText(jsonText), m_reader(makeReader(m_jsonText))
{
	GetNextState();
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadObjectStart() noexcept
{
	m_isSucceeded = (m_state == Mso::Json::ParseState::StartObject);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadBool(bool& value) noexcept
{
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetBool(&value);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadInt8(int8_t& value) noexcept
{
	int32_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt(&parsedValue)
		&& (parsedValue >= std::numeric_limits<int8_t>::min())
		&& (parsedValue <= std::numeric_limits<int8_t>::max());
	value = static_cast<int8_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadInt16(int16_t& value) noexcept
{
	int32_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt(&parsedValue)
		&& (parsedValue >= std::numeric_limits<int16_t>::min())
		&& (parsedValue <= std::numeric_limits<int16_t>::max());
	value = static_cast<int16_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadInt32(int32_t& value) noexcept
{
	int64_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt64(&parsedValue)
		&& (parsedValue >= std::numeric_limits<int32_t>::min())
		&& (parsedValue <= std::numeric_limits<int32_t>::max());
	value = static_cast<int32_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadInt64(int64_t& value) noexcept
{
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt64(&value);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadUInt8(uint8_t& value) noexcept
{
	int32_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt(&parsedValue)
		&& (parsedValue >= std::numeric_limits<uint8_t>::min())
		&& (parsedValue <= std::numeric_limits<uint8_t>::max());
	value = static_cast<uint8_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadUInt16(uint16_t& value) noexcept
{
	int32_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt(&parsedValue)
		&& (parsedValue >= std::numeric_limits<uint16_t>::min())
		&& (parsedValue <= std::numeric_limits<uint16_t>::max());
	value = static_cast<uint16_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadUInt32(uint32_t& value) noexcept
{
	int64_t parsedValue = 0;
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetInt64(&parsedValue)
		&& (parsedValue >= std::numeric_limits<uint32_t>::min())
		&& (parsedValue <= std::numeric_limits<uint32_t>::max());
	value = static_cast<uint32_t>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadUInt64(uint64_t& value) noexcept
{
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetUInt64(&value);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadDouble(double& value) noexcept
{
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetDouble(&value);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadFloat(float& value) noexcept
{
	double parsedValue { 0.0 };
	m_isSucceeded = (m_state == Mso::Json::ParseState::Value)
		&& m_reader->GetDouble(&parsedValue);
	value = static_cast<float>(parsedValue);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::IsNull() noexcept
{
	return m_reader->IsNull();
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadNextProperty() noexcept
{
	// Ensure that tdl\reactx generated code doesn't call unnecessary functions after finding an error in JSON
	VerifyElseCrashTag(m_isSucceeded, 0x0281d347 /* tag_c63nh */);
	if (m_isSucceeded && (GetNextState() == Mso::Json::ParseState::Name))
	{
		m_isSucceeded = m_reader->GetString(m_propertyName);
		return true;
	}

	m_isSucceeded = (m_state == Mso::Json::ParseState::EndObject);
	return false;
}

template <typename TChar>
bool JsonRekaReader<TChar>::ReadNextArrayItem() noexcept
{
	GetNextState();
	m_isSucceeded = m_isSucceeded && !Mso::Json::IsParseError(m_state);
	return (m_isSucceeded && (m_state != Mso::Json::ParseState::EndArray));
}

template <typename TChar>
bool JsonRekaReader<TChar>::UnknownProperty() noexcept
{
	// The property is unknown. We just need to read its value to start reading the next one
	m_isSucceeded = m_isSucceeded && (GetNextState() == Mso::Json::ParseState::Value);
	return m_isSucceeded;
}

template <typename TChar>
bool JsonRekaReader<TChar>::IsSucceeded() const noexcept
{
	return m_isSucceeded;
}

template <typename TChar>
Mso::Json::ParseState::Enum JsonRekaReader<TChar>::GetNextState() noexcept
{
	m_state = m_reader->ReadNext();
	return m_state;
}

template <typename TChar>
bool JsonRekaReader<TChar>::GetStringValue() noexcept
{
	m_isSucceeded = m_isSucceeded && (m_state == Mso::Json::ParseState::Value);
	m_isSucceeded = m_isSucceeded && m_reader->GetString(m_stringValue);
	return m_isSucceeded;
}

//=============================================================================
// Utf16JsonRekaReader implementation.
//=============================================================================

Utf16JsonRekaReader::Utf16JsonRekaReader(const wchar_t* jsonText) noexcept
	: JsonRekaReader(jsonText, &MakeReader)
{
}

Mso::TCntPtr<Mso::Json::TIReader<wchar_t>> Utf16JsonRekaReader::MakeReader(const Mso::basic_string<wchar_t>& jsonText) noexcept
{
	return Mso::Json::Unicode::MakeReader(Mso::basic_string_view<wchar_t>(jsonText.data(), jsonText.size()));
}

bool Utf16JsonRekaReader::ReadString(std::string& value) noexcept
{
	if (GetStringValue())
	{
		value = ConvertUtf16ToUtf8(m_stringValue.data(), m_stringValue.size());
		return m_isSucceeded;
	}
	return false;
}

bool Utf16JsonRekaReader::ReadWString(std::wstring& value) noexcept
{
	if (GetStringValue())
	{
		value = std::wstring(m_stringValue);
		return m_isSucceeded;
	}
	return false;
}

void Utf16JsonRekaReader::GetPropertyName(std::wstring& name) noexcept
{
	name = m_propertyName;
	GetNextState();
}

bool Utf16JsonRekaReader::MatchPropertyName(const wchar_t* name) noexcept
{
	if (m_isSucceeded && (m_propertyName == name))
	{
		GetNextState();
		return true;
	}
	return false;
}

//=============================================================================
// Utf8JsonRekaReader implementation.
//=============================================================================

Utf8JsonRekaReader::Utf8JsonRekaReader(const char* jsonText) noexcept
	: JsonRekaReader(jsonText, &MakeReader)
{
}

Mso::TCntPtr<Mso::Json::TIReader<char>> Utf8JsonRekaReader::MakeReader(const Mso::basic_string<char>& jsonText) noexcept
{
	return Mso::Json::Utf8::MakeReader(Mso::basic_string_view<char>(jsonText.data(), jsonText.size()));
}

bool Utf8JsonRekaReader::ReadString(std::string& value) noexcept
{
	if (GetStringValue())
	{
		value = std::string(m_stringValue);
		return m_isSucceeded;
	}
	return false;
}

bool Utf8JsonRekaReader::ReadWString(std::wstring& value) noexcept
{
	if (GetStringValue())
	{
		value = ConvertUtf8ToUtf16(m_stringValue.data(), m_stringValue.size());
		return m_isSucceeded;
	}
	return false;
}

void Utf8JsonRekaReader::GetPropertyName(std::wstring& name) noexcept
{
	name = ConvertUtf8ToUtf16(m_propertyName.data(), m_propertyName.size());
	GetNextState();
}

bool Utf8JsonRekaReader::MatchPropertyName(const wchar_t* name) noexcept
{
	if (m_isSucceeded && (ConvertUtf8ToUtf16(m_propertyName.data(), m_propertyName.size()) == name))
	{
		GetNextState();
		return true;
	}
	return false;
}

//=============================================================================
// JsonReader implementation.
//=============================================================================

LIBLET_PUBLICAPI JsonReader::JsonReader(const char* jsonText) noexcept
	: m_reader(Mso::Make<Utf8JsonRekaReader>(jsonText))
{
}

LIBLET_PUBLICAPI JsonReader::JsonReader(const wchar_t* jsonText) noexcept
	: m_reader(Mso::Make<Utf16JsonRekaReader>(jsonText))
{
}

LIBLET_PUBLICAPI JsonReader::JsonReader(Mso::JSHost::IRekaReader& reader) noexcept
	: m_reader(Mso::TCntPtr<Mso::JSHost::IRekaReader>(&reader))
{
}

LIBLET_PUBLICAPI bool JsonReader::ReadObjectStart() noexcept
{
	return m_reader->ReadObjectStart();
}

LIBLET_PUBLICAPI bool JsonReader::ReadObjectStart(const wchar_t* /*typeName*/) noexcept
{
	// This function is deprecated, the parameter is not used anymore in generated code from reka-compiler
	return ReadObjectStart();
}

LIBLET_PUBLICAPI bool JsonReader::ReadBool(bool& value) noexcept
{
	return m_reader->ReadBool(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadInt8(int8_t& value) noexcept
{
	return m_reader->ReadInt8(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadInt16(int16_t& value) noexcept
{
	return m_reader->ReadInt16(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadInt32(int32_t& value) noexcept
{
	return m_reader->ReadInt32(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadInt64(int64_t& value) noexcept
{
	return m_reader->ReadInt64(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadUInt8(uint8_t& value) noexcept
{
	return m_reader->ReadUInt8(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadUInt16(uint16_t& value) noexcept
{
	return m_reader->ReadUInt16(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadUInt32(uint32_t& value) noexcept
{
	return m_reader->ReadUInt32(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadUInt64(uint64_t& value) noexcept
{
	return m_reader->ReadUInt64(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadDouble(double& value) noexcept
{
	return m_reader->ReadDouble(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadFloat(float& value) noexcept
{
	return m_reader->ReadFloat(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadString(std::string& value) noexcept
{
	return m_reader->ReadString(value);
}

LIBLET_PUBLICAPI bool JsonReader::ReadWString(std::wstring& value) noexcept
{
	return m_reader->ReadWString(value);
}

LIBLET_PUBLICAPI bool JsonReader::IsNull() noexcept
{
	return m_reader->IsNull();
}

LIBLET_PUBLICAPI bool JsonReader::ReadNextProperty() noexcept
{
	return m_reader->ReadNextProperty();
}

LIBLET_PUBLICAPI void JsonReader::GetPropertyName(std::wstring& name) noexcept
{
	return m_reader->GetPropertyName(name);
}

LIBLET_PUBLICAPI bool JsonReader::MatchPropertyName(const wchar_t* name) noexcept
{
	return m_reader->MatchPropertyName(name);
}

LIBLET_PUBLICAPI bool JsonReader::ReadNextArrayItem() noexcept
{
	return m_reader->ReadNextArrayItem();
}

LIBLET_PUBLICAPI bool JsonReader::UnknownProperty() noexcept
{
	return m_reader->UnknownProperty();
}

} // namespace Mso::JSHost
