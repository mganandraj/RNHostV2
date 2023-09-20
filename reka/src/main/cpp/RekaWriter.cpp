#include "precomp.h"
#include <reka/RekaWriter.h>

namespace Mso { namespace JSHost {

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, bool value) noexcept
{
	writer.WriteBool(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int8_t value) noexcept
{
	writer.WriteInt8(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int16_t value) noexcept
{
	writer.WriteInt16(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int32_t value) noexcept
{
	writer.WriteInt32(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, int64_t value) noexcept
{
	writer.WriteInt64(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint8_t value) noexcept
{
	writer.WriteUInt8(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint16_t value) noexcept
{
	writer.WriteUInt16(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint32_t value) noexcept
{
	writer.WriteUInt32(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, uint64_t value) noexcept
{
	writer.WriteUInt64(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, double value) noexcept
{
	writer.WriteDouble(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, float value) noexcept
{
	writer.WriteFloat(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const char* value) noexcept
{
	writer.WriteString(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const wchar_t* value) noexcept
{
	writer.WriteWString(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const std::string& value) noexcept
{
	writer.WriteString(value);
}

LIBLET_PUBLICAPI void WriteRekaObject(Mso::JSHost::IRekaWriter& writer, const std::wstring& value) noexcept
{
	writer.WriteWString(value);
}

}} // namespace Mso::JSHost
