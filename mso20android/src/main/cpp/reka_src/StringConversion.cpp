#include "precomp.h"
#include "StringConversion.h"
#include <core_root/msostringConversion.h>
#include <crash/verifyElseCrash.h>

#undef new
#undef delete

namespace Mso { namespace JSHost {

std::string ConvertUtf16ToUtf8(const wchar_t* str, size_t strLength) noexcept
{
	std::string result;
	if (str != nullptr && strLength != 0)
	{
		const int requiredSize = UnicodeToUTF8(str, static_cast<int>(strLength), nullptr, 0);
		result.assign(requiredSize, '\0');

		const int convertedSize = UnicodeToUTF8(str, static_cast<int>(strLength), &result[0], static_cast<int>(result.length()));
		(void)convertedSize; //TODO: add error reporting
	}

	return result;
}

std::wstring ConvertUtf8ToUtf16(const char* str, size_t strLength) noexcept
{
	std::wstring result;
	if (str != nullptr)
	{
		int len = static_cast<int>(strLength);
		const int requiredSize = UTF8ToUnicode(str, &len, nullptr, 0);
		result = std::wstring(requiredSize, '\0');
		const int convertedSize = UTF8ToUnicode(str, &len, &result[0], static_cast<int>(result.length()));
		(void)convertedSize; //TODO: add error reporting
	}

	return result;
}

std::wstring ConvertUtf8ToUtf16(const char* str) noexcept
{
	return ConvertUtf8ToUtf16(str, strlen(str));
}

void ByteBuffer::Reset(size_t size) noexcept
{
	if (m_bytes)
	{
		if (m_bytes != m_inlineBytes)
		{
			delete[] m_bytes;
		}

		m_bytes = nullptr;
		m_size = 0;
	}

	if (size != 0)
	{
		if (size <= m_inlineSize)
		{
			m_bytes = m_inlineBytes;
		}
		else
		{
			m_bytes = new char[size];
		}

		m_size = size;
	}
}

char* ConvertUtf16ToUtf8(const wchar_t* value, ByteBuffer& resultBuffer) noexcept
{
	int len = static_cast<int>(wcslen(value));
	const int requiredSize = UnicodeToUTF8(value, len, nullptr, 0);
	resultBuffer.Reset(static_cast<size_t>(requiredSize + 1));
	const int convertedSize = UnicodeToUTF8(value, len, resultBuffer.Bytes(), static_cast<int>(resultBuffer.Size()));
	VerifyElseCrashTag(convertedSize <= requiredSize, 0x0281d357 /* tag_c63nx */);
	char* result = reinterpret_cast<char*>(resultBuffer.Bytes());
	result[convertedSize] = '\0';
	return result;
}

wchar_t* ConvertUtf8ToUtf16(const char* value, ByteBuffer& resultBuffer) noexcept
{
	int len = static_cast<int>(strlen(value));
	const int requiredSize = UTF8ToUnicode(value, &len, nullptr, 0);
	resultBuffer.Reset(static_cast<size_t>(requiredSize + 1) * sizeof(wchar_t));
	const int convertedSize = UTF8ToUnicode(value, &len, reinterpret_cast<wchar_t*>(resultBuffer.Bytes()), static_cast<int>(resultBuffer.Size() / sizeof(wchar_t)));
	VerifyElseCrashTag(convertedSize <= requiredSize, 0x0281d358 /* tag_c63ny */);
	wchar_t* result = reinterpret_cast<wchar_t*>(resultBuffer.Bytes());
	result[convertedSize] = '\0';
	return result;
}

}} // namespace Mso::JSHost
