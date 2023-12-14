#pragma once

#include <cstdint>
#include <string>
#include <object/UnknownObject.h>
#include "DataTypes.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

MSO_STRUCT_GUID(IRekaWriter, "11ddf3ba-2832-4a7f-91dd-7503c0151aef")
struct DECLSPEC_NOVTABLE IRekaWriter : IUnknown
{
	virtual void WriteObjectStart() noexcept = 0;
	virtual void WriteObjectEnd() noexcept = 0;
	virtual void WritePropertyName(const wchar_t* name) noexcept = 0;
	virtual void WriteString(const char* value) noexcept = 0;
	virtual void WriteWString(const wchar_t* value) noexcept = 0;
	virtual void WriteString(const std::string& value) noexcept = 0;
	virtual void WriteWString(const std::wstring& value) noexcept = 0;
	virtual void WriteBool(bool value) noexcept = 0;
	virtual void WriteInt8(int8_t value) noexcept = 0;
	virtual void WriteInt16(int16_t value) noexcept = 0;
	virtual void WriteInt32(int32_t value) noexcept = 0;
	virtual void WriteInt64(int64_t value) noexcept = 0;
	virtual void WriteUInt8(uint8_t value) noexcept = 0;
	virtual void WriteUInt16(uint16_t value) noexcept = 0;
	virtual void WriteUInt32(uint32_t value) noexcept = 0;
	virtual void WriteUInt64(uint64_t value) noexcept = 0;
	virtual void WriteDouble(double value) noexcept = 0;
	virtual void WriteFloat(float value) noexcept = 0;
	virtual void WriteArrayStart() noexcept = 0;
	virtual void WriteArrayEnd() noexcept = 0;
	virtual void WriteNull() noexcept = 0;
	virtual bool IsSucceeded() const noexcept = 0;

	template <class T>
	void WriteProperty(const wchar_t* name, const T& value) noexcept;
};

// Overloading for primitive types
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, bool value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int8_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int16_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int32_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int64_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint8_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint16_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint32_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint64_t value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, double value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, float value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const char* value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const wchar_t* value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const std::string& value) noexcept;
LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const std::wstring& value) noexcept;

// Overloading for vector
template <class T>
void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const std::vector<T>& value) noexcept
{
	writer.WriteArrayStart();
	for (const T& item : value)
	{
		WriteRekaObject(writer, item);
	}
	writer.WriteArrayEnd();
}

// Overloading for indexer
template <class T>
void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const Mso::JSHost::Indexer<T>& value) noexcept
{
	writer.WriteObjectStart();
	for (const auto& pair : value)
	{
		writer.WriteProperty(pair.first.c_str(), pair.second);
	}
	writer.WriteObjectEnd();
}

// Overloading for Optional
template <class T>
void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const Mso::JSHost::Optional<T>& opt) noexcept
{
	if (opt)
	{
		WriteRekaObject(writer, *opt);
	}
	else
	{
		writer.WriteNull();
	}
}

template <class T>
void IRekaWriter::WriteProperty(const wchar_t* name, const T& value) noexcept
{
	WritePropertyName(name);
	WriteRekaObject(*this, value);
}

template <class... TArgs>
void ArgsToWriter(IRekaWriter& writer, const TArgs&... args) noexcept
{
	writer.WriteArrayStart();
	// To write variadic template arguments in natural order we have to use them in an initializer list.
	int dummy[] = {(WriteRekaObject(writer, args), 0)...};
	(void)dummy; // the value is unused
	writer.WriteArrayEnd();
}

}} // namespace Mso::JSHost

#pragma pack(pop)
