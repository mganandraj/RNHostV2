#include "precomp.h"
#include <reka/RekaReader.h>

namespace Mso { namespace JSHost {

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, bool& value) noexcept
{
	reader.ReadBool(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int8_t& value) noexcept
{
	reader.ReadInt8(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int16_t& value) noexcept
{
	reader.ReadInt16(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int32_t& value) noexcept
{
	reader.ReadInt32(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, int64_t& value) noexcept
{
	reader.ReadInt64(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint8_t& value) noexcept
{
	reader.ReadUInt8(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint16_t& value) noexcept
{
	reader.ReadUInt16(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint32_t& value) noexcept
{
	reader.ReadUInt32(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, uint64_t& value) noexcept
{
	reader.ReadUInt64(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, double& value) noexcept
{
	reader.ReadDouble(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, float& value) noexcept
{
	reader.ReadFloat(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, std::string& value) noexcept
{
	reader.ReadString(value);
}

LIBLET_PUBLICAPI void ReadRekaObject(Mso::JSHost::IRekaReader& reader, std::wstring& value) noexcept
{
	reader.ReadWString(value);
}

}} // namespace Mso::JSHost
