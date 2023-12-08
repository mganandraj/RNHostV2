/**
	StringCore liblet APIs
*/

#pragma once

#include <compileradapters/functiondecorations.h>
// #include <platformadapters/windowsfirst.h>
#include <oacr/oacr.h>
#include <stdlib.h>

struct IMsoMemHeap;

namespace Mso::StringCore {

	/**
	This function performs upper case conversion for a given Unicode character given its language.
	It must be kept this way because Word needs an invariant upper casing.
	Returns the input Unicode character if no conversion is found.
	Works only for the 5 Win and 5 Mac code pages needed for the Pan European version of the apps.
	Set the bit msoStrip in wflags if you want to strip all the accents

	\param [in]		wch		Unicode character to be converted
	\param [in]		wzTag	culture tag
	\param [in]		wflags	conversion flags
	\return converted character
	*/
	LIBLET_PUBLICAPI WCHAR WchToUpperCultureTag(WCHAR wch, _In_z_ const WCHAR *wzTag, WORD wflags) noexcept;

	/**
	This function performs lower case conversion for a given Unicode character given its language.
	It must be kept this way because Word needs an invariant lower casing.
	Returns the input Unicode character if no conversion is found.
	Works only for the 5 Win and 5 Mac code pages needed for the Pan European version of the apps.

	\param [in]		wch		Unicode character to be converted
	\param [in]		wzTag	culture tag
	\return converted character
	*/
	LIBLET_PUBLICAPI WCHAR WchToLowerCultureTag(WCHAR wch, _In_z_ const WCHAR *wzTag) noexcept;

	/**
	This function performs upper case conversion for a given sz, using the capitalization rules for the locale passed.

	\param [in/out]		sz		null-terminated standard width string to be converted
	\param [in]			wzTag	culture tag
	\param [in]			pmmh	memheap
	*/
	LIBLET_PUBLICAPI void SzUpperCultureTag(_Inout_z_ CHAR *sz, _In_z_ const WCHAR *wzTag, _In_opt_ IMsoMemHeap *pmmh) noexcept;

	/**
	This function performs lower case conversion for a given sz, using the capitalization rules for the locale passed.

	\param [in/out]		sz		null-terminated standard width string to be converted
	\param [in]			wzTag	culture tag
	\param [in]			pmmh	memheap
	*/
	LIBLET_PUBLICAPI void SzLowerCultureTag(_Inout_z_ CHAR *sz, _In_z_ const WCHAR *wzTag, _In_opt_ IMsoMemHeap *pmmh) noexcept;

	/**
	This function performs upper case conversion for a given pwch and cchLen, using the capitalization rules for the culture passed.

	\param [in/out]		pwch	null-terminated wide char string to be converted
	\param [in]			cch		size of string
	\param [in]			wzTag	culture tag
	*/
	LIBLET_PUBLICAPI void PwchUpperCultureTag(_Inout_count_(cch) WCHAR *pwch, int cch, _In_z_ const WCHAR *wzTag) noexcept;

	/**
	This function performs lower case conversion for a given pwch and cchLen, using the capitalization rules for the culture passed.

	\param [in/out]		pwch	null-terminated wide char string to be converted
	\param [in]			cch		size of string
	\param [in]			wzTag	culture tag
	*/
	LIBLET_PUBLICAPI void PwchLowerCultureTag(_Inout_count_(cch) WCHAR *pwch, int cch, _In_z_ const WCHAR *wzTag) noexcept;

	/**
	This function performs upper case conversion for a given wz, using the capitalization rules for the culture tag passed.

	\param [in/out]		wz		null-terminated wide char string to be converted
	\param [in]			wzTag	culture tag
	*/
	__inline void WzUpperCultureTag(_Inout_z_ WCHAR *wz, _In_z_ const WCHAR *wzTag) noexcept
	{
		PwchUpperCultureTag(wz, (int) wcslen(wz), wzTag);
	}

	/**
	This function performs lower case conversion for a given wz, using the capitalization rules for the culture tag passed.

	\param [in/out]		wz		null-terminated wide char string to be converted
	\param [in]			wzTag	culture tag
	*/
	__inline void WzLowerCultureTag(_Inout_z_ WCHAR *wz, _In_z_ const WCHAR *wzTag) noexcept
	{
		PwchLowerCultureTag(wz, (int) wcslen(wz), wzTag);
	}

} //namespace Mso::StringCore


/**
This helper function gets the language parent of a culture tag

\param [in]		wzTag				culture tag
\param [out]	parentCultureTag	parent culture tag
\param [in]		cchParentCultureTag	parent culture tag size
*/
__inline void GetParentCultureTag(_In_z_ const WCHAR *wzTag, _Out_z_cap_(cchParentCultureTag) WCHAR *parentCultureTag, int cchParentCultureTag) noexcept
{
	std::abort();
//	WCHAR* index = nullptr;
//
//	// We only care about user locale as it affects sorting
//	if (wzTag == LOCALE_NAME_USER_DEFAULT)
//	{
//		GetUserDefaultLocaleName(parentCultureTag, cchParentCultureTag);
//	}
//	else
//	{
//		wcscpy_s(parentCultureTag, (size_t) cchParentCultureTag, wzTag);
//	}
//
//	// Get the parent culture
//	OACR_WARNING_SUPPRESS(NOTHROW_FUNC_THROWS, "wcschr doesn't throw")
//	index = wcschr(parentCultureTag, L'-');
//
//	// if no - found, this is already a parent language, otherwise null terminate the string at where - is, so en-US would become en, etc.
//	if (index != nullptr)
//	{
//		index[0] = 0;
//	}
}

/**
This helper function returns true if the language parent is Turkish or Azeri

\param [in]		wzTag	culture tag
\return true if the language parent is Turkish or Azeri
*/
__inline bool IsCultureTagParentTurkishOrAzeri(_In_z_ const WCHAR *wzTag) noexcept
{
	bool fResult = false;
	static DWORD s_dwTurkishOrAzeriUserLocale = 0xffffffff;
	WCHAR parentCultureTag[LOCALE_NAME_MAX_LENGTH];
	const WCHAR *pTag = nullptr;

	if (wzTag == LOCALE_NAME_USER_DEFAULT)
	{
		if (0xffffffff != s_dwTurkishOrAzeriUserLocale)
		{
			return (1 == s_dwTurkishOrAzeriUserLocale);
		}
		GetParentCultureTag(wzTag, parentCultureTag, _countof(parentCultureTag));
		pTag = parentCultureTag;
	}
	else
	{
		pTag = wzTag;
	}

	if (pTag)
	{
		if (((pTag[0] | 0x0020) == L't') && ((pTag[1] | 0x0020) == L'r') && (((pTag[2] | 0x0020)  < L'a') || ((pTag[2] | 0x0020) > L'z')))
		{
			fResult = true;
		}
		else if (((pTag[0] | 0x0020) == L'a') && ((pTag[1] | 0x0020) == L'z') && (((pTag[2] | 0x0020)  < L'a') || ((pTag[2] | 0x0020) > L'z')))
		{
			fResult = true;
		}
	}

	if ((0xffffffff == s_dwTurkishOrAzeriUserLocale) && (wzTag == LOCALE_NAME_USER_DEFAULT))
	{
		s_dwTurkishOrAzeriUserLocale = (DWORD)(fResult ? 1 : 0);
	}

	return fResult;
}
