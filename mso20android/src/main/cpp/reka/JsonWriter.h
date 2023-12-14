#pragma once

//
// Object serialization to JSON string.
//

#include <cstdint>
#include <string>
#include <json/IJsonWriter.h>
#include <vector>
#include "DataTypes.h"
#include "RekaWriter.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

template <class T>
void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const T& value) noexcept;

// Serializes an object to JSON string.
struct JsonWriter
{
	template <class T>
	friend void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const T& value) noexcept;

	LIBLET_PUBLICAPI JsonWriter() noexcept;

	LIBLET_PUBLICAPI void WriteObjectStart() noexcept;
	LIBLET_PUBLICAPI void WriteObjectEnd() noexcept;
	LIBLET_PUBLICAPI _SA_deprecated_(Mso::JSHost::JsonReader::WriteObjectStart()) void WriteObjectStart(const wchar_t* typeName) noexcept;

	LIBLET_PUBLICAPI void WritePropertyName(const wchar_t* name) noexcept;
	LIBLET_PUBLICAPI void WriteString(const char* value) noexcept;
	LIBLET_PUBLICAPI void WriteWString(const wchar_t* value) noexcept;
	LIBLET_PUBLICAPI void WriteString(const std::string& value) noexcept;
	LIBLET_PUBLICAPI void WriteWString(const std::wstring& value) noexcept;
	LIBLET_PUBLICAPI void WriteBool(bool value) noexcept;
	LIBLET_PUBLICAPI void WriteInt8(int8_t value) noexcept;
	LIBLET_PUBLICAPI void WriteInt16(int16_t value) noexcept;
	LIBLET_PUBLICAPI void WriteInt32(int32_t value) noexcept;
	LIBLET_PUBLICAPI void WriteInt64(int64_t value) noexcept;
	LIBLET_PUBLICAPI void WriteUInt8(uint8_t value) noexcept;
	LIBLET_PUBLICAPI void WriteUInt16(uint16_t value) noexcept;
	LIBLET_PUBLICAPI void WriteUInt32(uint32_t value) noexcept;
	LIBLET_PUBLICAPI void WriteUInt64(uint64_t value) noexcept;
	LIBLET_PUBLICAPI void WriteDouble(double value) noexcept;
	LIBLET_PUBLICAPI void WriteFloat(float value) noexcept;
	LIBLET_PUBLICAPI void WriteArrayStart() noexcept;
	LIBLET_PUBLICAPI void WriteArrayEnd() noexcept;
	LIBLET_PUBLICAPI void WriteNull() noexcept;

	LIBLET_PUBLICAPI std::string ToString() noexcept;
	LIBLET_PUBLICAPI std::wstring ToWString() noexcept;

	bool IsSucceeded() const noexcept { return m_writer->IsSucceeded(); }

	Mso::JSHost::IRekaWriter& GetRekaWriter() const noexcept
	{
		return *m_writer.Get();
	}

	template <class T>
	void WriteProperty(const wchar_t* name, const T& value) noexcept
	{
		m_writer->WriteProperty<T>(name, value);
	}

private:
	Mso::TCntPtr<Mso::JSHost::IRekaWriter> m_writer;

private:
	// Overloading for backward compatibility. This should be removed before we add more implementations for IRekaWriter.
	LIBLET_PUBLICAPI JsonWriter(Mso::JSHost::IRekaWriter& writer) noexcept;
};

// Overloading for enum, this should be rewrite as WriteRekaObject after all users use the new reka-compiler
template <class T>
void WriteJsonObject(Mso::JSHost::JsonWriter& writer, const T& value, std::enable_if_t<std::is_enum<T>::value>* = nullptr) noexcept
{
	writer.WriteInt32(static_cast<int32_t>(value));
}

// Overloading for backward compatibility
template <class T>
void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const T& value) noexcept
{
	Mso::JSHost::JsonWriter jsonWriter(writer);
	WriteJsonObject(jsonWriter, value);
}

template <class T>
void WriteJsonObject(Mso::JSHost::JsonWriter& writer, const T& value, std::enable_if_t<!std::is_enum<T>::value>* = nullptr) noexcept
{
	WriteRekaObject(writer.GetRekaWriter(), value);
}

// Serialize arguments to JSON array
template <class... TArgs>
std::string ArgsToJson(const TArgs&... args) noexcept
{
	JsonWriter writer;
	ArgsToWriter(writer.GetRekaWriter(), std::forward<const TArgs&>(args)...);
	return writer.ToString();
}

}} // namespace Mso::JSHost

#pragma pack(pop)
