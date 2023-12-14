#pragma once

//
// Utilities to convert between UTF-8 and UTF-16 strings.
// They optimize for smal strings by keeping small strings on call stack.
//

#include <compileradapters/functiondecorations.h>
#include <string>

namespace Mso { namespace JSHost {

std::string ConvertUtf16ToUtf8(const wchar_t* str, size_t strLength) noexcept;
std::wstring ConvertUtf8ToUtf16(const char* str, size_t strLength) noexcept;
std::wstring ConvertUtf8ToUtf16(const char* str) noexcept;

// Collection of bytes.
// Size returns the overall buffer size. For null terminating stings Size should include the trailing zero.
struct ByteBuffer
{
	ByteBuffer() = default;
	ByteBuffer(char* inlineBytes, size_t inlineSize) noexcept
		: m_inlineBytes(inlineBytes)
		, m_inlineSize(inlineSize)
	{
	}

	ByteBuffer(const ByteBuffer& other) = delete;
	ByteBuffer& operator=(const ByteBuffer& other) = delete;

	~ByteBuffer() noexcept
	{
		Reset(0);
	}

	char* Bytes() noexcept
	{
		return m_bytes;
	}

	const char* Bytes() const noexcept
	{
		return m_bytes;
	}

	size_t Size() const noexcept
	{
		return m_size;
	}

	LIBLET_PUBLICAPI void Reset(size_t size) noexcept;

private:
	char* m_bytes{nullptr};
	size_t m_size{0};
	char* m_inlineBytes{nullptr};
	size_t m_inlineSize{0};
};

char* ConvertUtf16ToUtf8(const wchar_t* value, ByteBuffer& resultBuffer) noexcept;
wchar_t* ConvertUtf8ToUtf16(const char* value, ByteBuffer& resultBuffer) noexcept;

template <size_t InlineSize = 256>
struct Utf16Converter
{
	char* ToUtf8(const wchar_t* value) noexcept
	{
		return ConvertUtf16ToUtf8(value, /*ref*/ m_resultBuffer);
	}

private:
	char m_inlineChars[InlineSize];
	ByteBuffer m_resultBuffer{m_inlineChars, InlineSize};
};

template <size_t InlineSize = 256>
struct Utf8Converter
{
	wchar_t* ToUtf16(const char* value) noexcept
	{
		return ConvertUtf8ToUtf16(value, /*ref*/ m_resultBuffer);
	}

private:
	char m_inlineChars[InlineSize];
	ByteBuffer m_resultBuffer{m_inlineChars, InlineSize};
};

}} // namespace Mso::JSHost
