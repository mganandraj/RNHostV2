#include "precomp.h"
#include <reka/JsonWriter.h>
#include "StringConversion.h"

namespace Mso { namespace JSHost {

struct JsonRekaWriter : Mso::UnknownObject<IRekaWriter>
{
	JsonRekaWriter()
		: m_writer(Mso::Json::CreateJsonWriter())
	{
	}

	void WriteObjectStart() noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->StartObject();
	}
	
	void WriteObjectEnd() noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->EndObject();
	}

	void WritePropertyName(const wchar_t* name) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteName(name);
	}

	void WriteString(const std::string& value) noexcept override
	{
		WriteString(value.c_str());
	}

	void WriteWString(const std::wstring& value) noexcept override
	{
		WriteWString(value.c_str());
	}

	void WriteString(const char* value) noexcept override
	{
		Utf8Converter<> converter;
		WriteWString(converter.ToUtf16(value));
	}

	void WriteWString(const wchar_t* value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteString(value);
	}

	void WriteBool(bool value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteBool(value);
	}

	void WriteInt8(int8_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteInt(value);
	}

	void WriteInt16(int16_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteInt(value);
	}

	void WriteInt32(int32_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteInt(value);
	}

	void WriteInt64(int64_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteInt(value);
	}

	void WriteUInt8(uint8_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteUInt64(value);
	}

	void WriteUInt16(uint16_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteUInt64(value);
	}

	void WriteUInt32(uint32_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteUInt64(value);
	}

	void WriteUInt64(uint64_t value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteUInt64(value);
	}

	void WriteDouble(double value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteDouble(value);
	}

	void WriteFloat(float value) noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteDouble(value);
	}

	void WriteArrayStart() noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->StartArray();
	}

	void WriteArrayEnd() noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->EndArray();
	}

	void WriteNull() noexcept override
	{
		m_isSucceeded = m_isSucceeded && m_writer->WriteNull();
	}

	bool IsSucceeded() const noexcept override
	{
		return m_isSucceeded;
	}

	void GetResultData(std::wstring& data) noexcept
	{
		m_writer->GetResultData(data);
	}
private:
	Mso::TCntPtr<Mso::Json::IJsonWriter> m_writer;
	bool m_isSucceeded{true};
};

LIBLET_PUBLICAPI JsonWriter::JsonWriter() noexcept
	: m_writer(Mso::Make<JsonRekaWriter>())
{
}

LIBLET_PUBLICAPI JsonWriter::JsonWriter(Mso::JSHost::IRekaWriter& writer) noexcept
	: m_writer(Mso::TCntPtr<Mso::JSHost::IRekaWriter>(&writer))
{
}

LIBLET_PUBLICAPI void JsonWriter::WriteObjectStart() noexcept
{
	m_writer->WriteObjectStart();
}

LIBLET_PUBLICAPI void JsonWriter::WriteObjectEnd() noexcept
{
	m_writer->WriteObjectEnd();
}

LIBLET_PUBLICAPI void JsonWriter::WriteObjectStart(const wchar_t* /* typeName */) noexcept
{
	// This function is deprecated, the parameter is not used anymore in generated code from reka-compiler
	WriteObjectStart();
}

LIBLET_PUBLICAPI void JsonWriter::WritePropertyName(const wchar_t* name) noexcept
{
	m_writer->WritePropertyName(name);
}

LIBLET_PUBLICAPI void JsonWriter::WriteString(const std::string& value) noexcept
{
	m_writer->WriteString(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteWString(const std::wstring& value) noexcept
{
	m_writer->WriteWString(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteString(const char* value) noexcept
{
	m_writer->WriteString(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteWString(const wchar_t* value) noexcept
{
	m_writer->WriteWString(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteBool(bool value) noexcept
{
	m_writer->WriteBool(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteInt8(int8_t value) noexcept
{
	m_writer->WriteInt8(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteInt16(int16_t value) noexcept
{
	m_writer->WriteInt16(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteInt32(int32_t value) noexcept
{
	m_writer->WriteInt32(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteInt64(int64_t value) noexcept
{
	m_writer->WriteInt64(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteUInt8(uint8_t value) noexcept
{
	m_writer->WriteUInt8(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteUInt16(uint16_t value) noexcept
{
	m_writer->WriteUInt16(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteUInt32(uint32_t value) noexcept
{
	m_writer->WriteUInt32(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteUInt64(uint64_t value) noexcept
{
	m_writer->WriteUInt64(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteDouble(double value) noexcept
{
	m_writer->WriteDouble(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteFloat(float value) noexcept
{
	m_writer->WriteDouble(value);
}

LIBLET_PUBLICAPI void JsonWriter::WriteArrayStart() noexcept
{
	m_writer->WriteArrayStart();
}

LIBLET_PUBLICAPI void JsonWriter::WriteArrayEnd() noexcept
{
	m_writer->WriteArrayEnd();
}

LIBLET_PUBLICAPI void JsonWriter::WriteNull() noexcept
{
	m_writer->WriteNull();
}

LIBLET_PUBLICAPI std::string JsonWriter::ToString() noexcept
{
	std::wstring result = ToWString();
	return ConvertUtf16ToUtf8(result.data(), result.length());
}

LIBLET_PUBLICAPI std::wstring JsonWriter::ToWString() noexcept
{
	std::wstring result;
	static_cast<JsonRekaWriter*>(m_writer.Get())->GetResultData(/*ref*/ result);
	return result;
}

}} // namespace Mso::JSHost
