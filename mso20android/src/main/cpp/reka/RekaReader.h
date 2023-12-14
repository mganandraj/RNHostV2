#pragma once

#include <cstdint>
#include <string>
#include <object/UnknownObject.h>
#include "DataTypes.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

MSO_STRUCT_GUID(IRekaReader, "2476b868-14f5-4a91-8034-8bb744c0fc31")
struct DECLSPEC_NOVTABLE IRekaReader : IUnknown
{
	virtual bool ReadObjectStart() noexcept = 0;
	virtual bool ReadBool(bool& value) noexcept = 0;
	virtual bool ReadInt8(int8_t& value) noexcept = 0;
	virtual bool ReadInt16(int16_t& value) noexcept = 0;
	virtual bool ReadInt32(int32_t& value) noexcept = 0;
	virtual bool ReadInt64(int64_t& value) noexcept = 0;
	virtual bool ReadUInt8(uint8_t& value) noexcept = 0;
	virtual bool ReadUInt16(uint16_t& value) noexcept = 0;
	virtual bool ReadUInt32(uint32_t& value) noexcept = 0;
	virtual bool ReadUInt64(uint64_t& value) noexcept = 0;
	virtual bool ReadDouble(double& value) noexcept = 0;
	virtual bool ReadFloat(float& value) noexcept = 0;
	virtual bool ReadString(std::string& value) noexcept = 0;
	virtual bool ReadWString(std::wstring& value) noexcept = 0;
	virtual bool IsNull() noexcept = 0;
	virtual bool ReadNextProperty() noexcept = 0;
	virtual bool MatchPropertyName(const wchar_t* name) noexcept = 0;
	virtual bool ReadNextArrayItem() noexcept = 0;
	virtual bool UnknownProperty() noexcept = 0;
	virtual void GetPropertyName(std::wstring& name) noexcept = 0;
	virtual bool IsSucceeded() const noexcept = 0;

	// TryGetProperty returns true if a property matched the name and read its value, or returns false otherwise
	// If there is any unexpected token, it closes the reader.
	template <class T>
	bool TryGetProperty(const wchar_t* name, T& value) noexcept;
};

// Overloading for primitive types
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, bool& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int8_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int16_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int32_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int64_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint8_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint16_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint32_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint64_t& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, double& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, float& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, std::string& value) noexcept;
LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, std::wstring& value) noexcept;

// Overloading for vector
template <class T>
void ReadRekaObject(Mso::JSHost::IRekaReader& reader, std::vector<T>& value) noexcept
{
	while (reader.ReadNextArrayItem())
	{
		T item;
		ReadRekaObject(reader, item);
		value.push_back(std::move(item));
	};
}

// Overloading for indexer
template <class T>
void ReadRekaObject(Mso::JSHost::IRekaReader& reader, Mso::JSHost::Indexer<T>& value) noexcept
{
	VerifyElseCrashTag(reader.ReadObjectStart(), 0x0281d35e /* tag_c63n4 */);

	while (reader.ReadNextProperty())
	{
		std::wstring key;
		reader.GetPropertyName(/*ref*/ key);
		T item;
		ReadRekaObject(reader, item);
		value.emplace(std::move(key), std::move(item));
	}
}

// Overloading for Optional
template <class T>
void ReadRekaObject(Mso::JSHost::IRekaReader& reader, Mso::JSHost::Optional<T>& opt) noexcept
{
	if (reader.IsNull())
	{
		opt = std::nullopt;
	}
	else
	{
		T item;
		ReadRekaObject(reader, item);
		opt = std::move(item);
	}
}

template <class T>
bool IRekaReader::TryGetProperty(const wchar_t* name, T& value) noexcept
{
	// Ensure that tdl\reactx generated code doesn't call unnecessary functions after finding an error in JSON
	VerifyElseCrashTag(IsSucceeded(), 0x0281d35f /* tag_c63n5 */);
	if (!MatchPropertyName(name))
		return false;

	ReadRekaObject(*this, value);
	return IsSucceeded();
}

template <class... TArgs>
void ArgsFromReader(IRekaReader& reader, TArgs&... args) noexcept
{
	// To read variadic template arguments in natural order we have to use them in an initializer list.
	int dummy[] = {(reader.ReadNextArrayItem(), ReadRekaObject(reader, /*ref*/ args), 0)...};
	(void)dummy; // the value is unused
}

}} // namespace Mso::JSHost

#pragma pack(pop)
