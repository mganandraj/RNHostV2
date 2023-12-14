#pragma once

//
// Object deserialization from JSON string.
//

#include <json/IJsonReader.h>
#include <cstdint>
#include <string>
#include <vector>
#include "DataTypes.h"
#include "RekaReader.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

template <class T>
void ReadRekaObject(Mso::JSHost::IRekaReader& reader, T& value) noexcept;

struct JsonReader
{
	template <class T>
	friend void ReadRekaObject(Mso::JSHost::IRekaReader& reader, T& value) noexcept;

	LIBLET_PUBLICAPI JsonReader(const char* jsonText) noexcept;
	LIBLET_PUBLICAPI JsonReader(const wchar_t* jsonText) noexcept;

	// Read functions consume tokens, returning false if the token is not expected, and in this case close the reader
	LIBLET_PUBLICAPI bool ReadObjectStart() noexcept;
	LIBLET_PUBLICAPI _SA_deprecated_(Mso::JSHost::JsonReader::ReadObjectStart()) bool ReadObjectStart(const wchar_t* typeName) noexcept;
	LIBLET_PUBLICAPI bool ReadBool(bool& value) noexcept;
	LIBLET_PUBLICAPI bool ReadInt8(int8_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadInt16(int16_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadInt32(int32_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadInt64(int64_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadUInt8(uint8_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadUInt16(uint16_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadUInt32(uint32_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadUInt64(uint64_t& value) noexcept;
	LIBLET_PUBLICAPI bool ReadDouble(double& value) noexcept;
	LIBLET_PUBLICAPI bool ReadFloat(float& value) noexcept;
	LIBLET_PUBLICAPI bool ReadString(std::string& value) noexcept;
	LIBLET_PUBLICAPI bool ReadWString(std::wstring& value) noexcept;
	LIBLET_PUBLICAPI bool IsNull() noexcept;

	// ReadNextProperty returns true if a property name is found, or returns false if "}" is reached.
	// If there is any unexpected token, it closes the reader.
	LIBLET_PUBLICAPI bool ReadNextProperty() noexcept;
	LIBLET_PUBLICAPI void GetPropertyName(std::wstring& name) noexcept;
	LIBLET_PUBLICAPI bool MatchPropertyName(const wchar_t* name) noexcept;

	LIBLET_PUBLICAPI bool ReadNextArrayItem() noexcept;

	// Skip the property value, returns true if nothing wrong has happened.
	LIBLET_PUBLICAPI bool UnknownProperty() noexcept;

	bool IsSucceeded() const noexcept { return m_reader->IsSucceeded(); }

	Mso::JSHost::IRekaReader& GetRekaReader() const noexcept
	{
		return *m_reader.Get();
	}

	template <class T>
	bool TryGetProperty(const wchar_t* name, T& value) noexcept
	{
		return m_reader->TryGetProperty<T>(name, value);
	}

private:
	Mso::TCntPtr<Mso::JSHost::IRekaReader> m_reader;
	
private:
	// Overloading for backward compatibility. This should be removed before we add more implementations for IRekaReader.
	LIBLET_PUBLICAPI JsonReader(Mso::JSHost::IRekaReader& reader) noexcept;
};

// Overloading for enum, this should be rewrite as ReadRekaObject after all users use the new reka-compiler
template <class T>
void ReadJsonObject(Mso::JSHost::JsonReader& reader, T& value, std::enable_if_t<std::is_enum<T>::value>* = nullptr) noexcept
{
	int32_t intValue;
	if (reader.ReadInt32(intValue))
	{
		value = static_cast<T>(intValue);
	}
}

// Overloading for backward compatibility
template <class T>
void ReadRekaObject(Mso::JSHost::IRekaReader& reader, T& value) noexcept
{
	JsonReader jsonReader(reader);
	ReadJsonObject(jsonReader, value);
}

template <class T>
void ReadJsonObject(Mso::JSHost::JsonReader& reader, T& value, std::enable_if_t<!std::is_enum<T>::value>* = nullptr) noexcept
{
	ReadRekaObject(reader.GetRekaReader(), value);
}

// Deserialize arguments from JSON array
template <class... TArgs>
void ArgsFromJson(const char* jsonText, TArgs&... args) noexcept
{
	JsonReader reader{jsonText};
	ArgsFromReader(reader.GetRekaReader(), std::forward<TArgs&>(args)...);
}

// Deserialize arguments from JSON array
template <class... TArgs>
void ArgsFromJson(const wchar_t* jsonText, TArgs&... args) noexcept
{
	JsonReader reader { jsonText };
	ArgsFromReader(reader.GetRekaReader(), std::forward<TArgs&>(args)...);
}

}} // namespace Mso::JSHost

#pragma pack(pop)
