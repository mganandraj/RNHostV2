#pragma once
#include <compileradapters/functiondecorations.h>
#include <optional>
// #include <msoplatformfix.h>
#include <string>
#include <string_view>

namespace Mso::StringConversion::WideCharToUTF8 {

//! converts a given std::wstring_view to a multibyte string.  Returns an empty optional if conversion fails!
LIBLET_PUBLICAPI std::optional<std::string> ConvertWStringView(const std::wstring_view& input) noexcept;

//! converts a given WCHAR buffer to a multibyte string.  pInput does not have to be null-terminated.  Returns an empty optional if conversion fails!
inline std::optional<std::string> ConvertRawBuffer(const WCHAR* pInput, std::size_t cchInput) noexcept
{
	return ConvertWStringView({ pInput, cchInput });
}

//! converts a given null-terminated WCHAR buffer to a multibyte string.  Returns an empty optional if conversion fails!
inline std::optional<std::string> ConvertNullTerminatedBuffer(PCWSTR wzInput) noexcept
{
	return ConvertWStringView({ wzInput });
}

//! converts a given wstring to a multibyte string.  Returns an empty optional if conversion fails!
inline std::optional<std::string> ConvertWString(const std::wstring& input) noexcept
{
	return ConvertWStringView({ input.c_str(), input.length() });
}

//! converts a given std::wstring_view to a multibyte string.  Crashes if conversion fails!
LIBLET_PUBLICAPI std::string ConvertWStringViewOrCrash(const std::wstring_view& input) noexcept;

//! converts a given WCHAR buffer to a multibyte string.  pInput does not have to be null-terminated.  Crashes if conversion fails!
LIBLET_PUBLICAPI std::string ConvertRawBufferOrCrash(const WCHAR* pInput, std::size_t cchInput) noexcept;

//! converts a given null-terminated WCHAR buffer to a multibyte string.  Crashes if conversion fails!
LIBLET_PUBLICAPI std::string ConvertNullTerminatedBufferOrCrash(PCWSTR wzInput) noexcept;

//! converts a given wstring to a multibyte string.  Crashes if conversion fails!
LIBLET_PUBLICAPI std::string ConvertWStringOrCrash(const std::wstring& input) noexcept;

} // Mso::StringConversion::WideCharToUTF8

namespace Mso::StringCore {

/**
	StringFromWz

	This function creates a std::string from a wz

	Parameters:
	wzSource - const wz* to use
	maxLength - maximum length of the string wzSource to read.
*/
LIBLET_PUBLICAPI std::string StringFromWz(_In_z_ const wchar_t* wstringFrom, size_t maxLength = 65536) noexcept;

/**
	StringFromWString

	This function creates a std::string from a std::wstring.

	Parameters:
	sourceWString - Source std::wstring to use
*/
LIBLET_PUBLICAPI std::string StringFromWString(const std::wstring& sourceWString) noexcept;

} // Mso::StringCore