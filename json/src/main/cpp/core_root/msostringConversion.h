/**
	MsoStringConversion.h

	Copyright (c) 1994 Microsoft Corporation

	Functions related to converting strings
*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGCONVERSION_H
#define LIBLET_ROOT_MSOSTRINGCONVERSION_H
#include <compileradapters/functiondecorations.h>
// #include <compileradapters/functiondecorations_deprecated.h>
// #include <msoAssert.h>
#include <core_root/msoStringUtils.h>
#include <memoryapi/memoryapi.h>

typedef struct IMsoMemHeap IMsoMemHeap;

/*	This function provides our own layer to MultiByteToWideChar() call.  We
	provide our own lookup table if it's not available from NLS subsystem.
	Currently works for 20 SBCS Win and 5 Mac code pages. */
LIBLET_PUBLICAPI _Success_(return != 0) _When_((cchMultiByte == -1) && (cchWideChar != 0),
	_Post_equal_to_(_String_length_(lpWideCharStr) + 1))
	TASKLIBTECHDEBT_(int) MsoMultiByteToWideChar(
	UINT CodePage,
	DWORD dwFlags,
	_In_NLS_string_(cchMultiByte) LPCCH lpMultiByteStr,
	int cchMultiByte,
	_Out_writes_to_opt_(cchWideChar, return) LPWSTR lpWideCharStr,
	int cchWideChar) MSONOEXCEPT;

/*	This function provides our own layer to MultiByteToWideChar() call.  We
	provide our own lookup table if it's not available from NLS subsystem.
	Currently works for 20 SBCS Win and 5 Mac code pages. */
LIBLET_PUBLICAPI _Success_(return != 0) _When_((cchWideChar == -1) && (cchMultiByteStr != 0), _Post_equal_to_(_String_length_(lpMultiByteStr) + 1)) int MsoWideCharToMultiByte(
	UINT CodePage,
	DWORD dwFlags,
	_In_NLS_string_(cchWideChar) LPCWCH lpWideChar,
	int cchWideChar,
	_Out_writes_bytes_to_opt_(cchMultiByteStr, return) LPSTR lpMultiByteStr,
	int cchMultiByteStr,
	_In_opt_ const char* lpDefaultChar,
	_Out_opt_ LPBOOL lpUsedDefaultChar) MSONOEXCEPT;

/* These functions convert between Unicode and the UTF8 codepage, including
	surrogate support. See implementation for more detailed comments. */
LIBLET_PUBLICAPI int UTF8ToUnicode(_In_count_(*pcchSrc) LPCSTR lpSrcStr, _Inout_ int* pcchSrc, _Out_writes_to_opt_(cchDest, return) LPWSTR lpDestStr, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI int UnicodeToUTF8Core(const wchar_t* lpSrcStr, int cchSrc, int* pcchSrcLeft, _Out_opt_cap_(cchDest) char* lpDestStr, int cchDest) MSONOEXCEPT;

/**
	UnicodeToUTF8

	Simple version of UnicodeToUTF8Core that always converts the whole input.
	Use this for a single independent buffer, and Core for when converting
	a large buffer through a sequence of calls. This behaves differently than
	Core when the input ends with the first character of a Unicode surrogate
	pair. This will just treat it as a single Unicode character. However Core
	will avoid splitting the surrogate, and return 1 in the cchSrcLeft out
	parameter, meaning the last character of the input was not converted, and
	should be recycled as the first character of the next call to Core.
*/
__inline int UnicodeToUTF8(LPCWSTR lpSrcStr, int cchSrc, _Out_opt_cap_(cchDest) LPSTR lpDestStr, int cchDest) MSONOEXCEPT
{
	return (UnicodeToUTF8Core(lpSrcStr, cchSrc, NULL, lpDestStr, cchDest));
}

LIBLET_PUBLICAPI BOOL MsoFSurrogatePairToUCS4(WCHAR w1, WCHAR w2, DWORD* plUCS4) MSONOEXCEPT;
LIBLET_PUBLICAPI BOOL MsoFUCS4ToSurrogatePair(DWORD lUCS4, WCHAR* pw1, WCHAR* pw2) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoFConvertCharCode(_In_t_ const wchar_t* pwtSrc, WCHAR* pwchPrefix, DWORD* pdwval) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoFToggleCharCode(_In_t_ const wchar_t* pwtSrc, _Out_t_cap_(cchDest) wchar_t* pwtDest, int cchDest, int fExact) MSONOEXCEPT;

/**

	Functions for lower/upper case conversion
	Returns the input Unicode character if no conversion is found.

	Currently works only for the 5 Win and 5 Mac code pages needed for 
	the Pan European version of the apps.

*/

// Flags MsoWchToUpperLid
#define msoStrip 0x0001 // strip all accents

/**
This function performs upper case conversion for a given Unicode character given its language.
It must be kept this way because Word needs an invariant upper casing.
Returns the input Unicode character if no conversion is found.
Works only for the 5 Win and 5 Mac code pages needed for the Pan European version of the apps.
Set the bit msoStrip in wflags if you want to strip all the accents

\deprecated Use Mso::StringCore::WchToUpperCultureTag instead.

\param [in]		wch		Unicode character to be converted
\param [in]		lid		language id
\param [in]		wflags	conversion flags
\return converted character
*/
LIBLET_PUBLICAPI MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToUpperLid(WCHAR wch, LID lid, WORD wflags) MSONOEXCEPT;
/**
\deprecated Use Mso::StringCore::WchToUpperCultureTag instead, pass in 0 for wflags
*/
__inline WCHAR MsoWchToUpperCore(WCHAR wch, LCID lcid) MSONOEXCEPT
{
	return MsoWchToUpperLid(wch, LANGIDFROMLCID(lcid), 0);
}
/**
This function performs lower case conversion for a given Unicode character given its language.
It must be kept this way because Word needs an invariant lower casing.
Returns the input Unicode character if no conversion is found.
Works only for the 5 Win and 5 Mac code pages needed for the Pan European version of the apps.
The wflags param is unused currently.

\deprecated Use Mso::StringCore::WchToLowerCultureTag instead.  No need to pass in wflags as it's unused

\param [in]		wch		Unicode character to be converted
\param [in]		lid		language id
\param [in]		wflags	conversion flags
\return converted character
*/
LIBLET_PUBLICAPI MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToLowerLid(WCHAR wch, LID lid, WORD wflags) MSONOEXCEPT;
/**
\deprecated Use Mso::StringCore::WchToLowerCultureTag instead.
*/
__inline WCHAR MsoWchToLowerCore(WCHAR wch, LCID lcid) MSONOEXCEPT
{
	return MsoWchToLowerLid(wch, LANGIDFROMLCID(lcid), 0);
}
/**
This function performs upper case conversion for a given sz.

\deprecated Use Mso::StringCore::SzUpperCultureTag instead.

\param [in,out]		sz		sz to be converted
\param [in]			pmmh	memheap
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI void MsoSzUpperCore(_Inout_z_ CHAR* sz, IMsoMemHeap* pmmh, LCID lcid) MSONOEXCEPT;
/**
This function performs upper case conversion for a given wz, using the capitalization rules for the locale passed.

\deprecated Use Mso::StringCore::WzUpperCultureTag instead.

\param [in,out]		wz		wz to be converted
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI TASKLIBTECHDEBT_(void) MsoWzUpperCore(_Inout_z_ WCHAR* wz, LCID lcid) MSONOEXCEPT;
/**
This function performs upper case conversion for a given pwch and cchLen, using the capitalization rules for the culture passed.

\deprecated Use Mso::StringCore::PwchUpperCultureTag instead.

\param [in,out]		pwch	string to be converted
\param [in]			cch		size of string
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI TASKLIBTECHDEBT_(void) MsoPwchUpperCore(_Inout_count_(cch) WCHAR* pwch, int cch, LCID lcid) MSONOEXCEPT;
/**
This function performs lower case conversion for a given sz.

\deprecated Use Mso::StringCore::SzLowerCultureTag instead.

\param [in,out]		sz		sz to be converted
\param [in]			pmmh	memheap
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI void MsoSzLowerCore(_Inout_z_ CHAR* sz, IMsoMemHeap* pmmh, LCID lcid) MSONOEXCEPT;
/**
This function performs lower case conversion for a given wz, using the capitalization rules for the locale passed.

\deprecated Use Mso::StringCore::WzLowerCultureTag instead.

\param [in,out]		wz		wz to be converted
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI TASKLIBTECHDEBT_(void) MsoWzLowerCore(_Inout_z_ WCHAR* wz, LCID lcid) MSONOEXCEPT;
/**
This function performs lower case conversion for a given pwch and cchLen, using the capitalization rules for the culture passed.

\deprecated Use Mso::StringCore::PwchLowerCultureTag instead.

\param [in,out]		pwch	string to be converted
\param [in]			cch		size of string
\param [in]			lcid	language id
*/
LIBLET_PUBLICAPI TASKLIBTECHDEBT_(void) MsoPwchLowerCore(_Inout_count_(cch) WCHAR* pwch, int cch, LCID lcid) MSONOEXCEPT;

/**
This function performs upper case conversion for a given Unicode character with the invariant culture

\deprecated Use Mso::StringCore::WchToUpperCultureTag(wch, L"", 0) instead.

\param [in]		wch		Unicode character to be converted
\return converted character
*/
LIBLET_PUBLICAPI WCHAR MsoWchToUpperInvariant(WCHAR wch) MSONOEXCEPT;

/**

	Functions for char<->wchar_t string conversion
	Simple routines assume input string contains ASCII
	Routines without CodePage argument will use CP_ACP

*/

LIBLET_PUBLICAPI void MsoSzToWzSimple(_In_z_ const CHAR* szFrom, _Out_z_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT;

#ifdef __cplusplus
namespace Mso::String {
inline Mso::MemoryPtr<WCHAR> SzAsciiToWz(_In_z_count_(cch) const char* sz, size_t cch) noexcept
{
	Mso::MemoryPtr<WCHAR> wz;
	VerifyElseCrashTag(wz.AllocElem(cch, 1), 0x01402543 /* tag_bqcvd */);
	MsoSzToWzSimple(sz, wz.Get(), static_cast<int>(cch) + 1);
	return wz;
}

inline Mso::MemoryPtr<WCHAR> SzAsciiToWz(_In_z_ const char* sz) noexcept
{
	return SzAsciiToWz(sz, strlen(sz));
}
} // namespace Mso::String
#endif // __cplusplus

LIBLET_PUBLICAPI void MsoWzToSzSimple(_In_z_ const WCHAR* wzFrom, _Out_opt_z_cap_(cchTo) CHAR* szTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWzToSzSimpleInPlace(_Inout_z_ WCHAR* wz) MSONOEXCEPT;
LIBLET_PUBLICAPI void MsoRgwchToRgchSimple(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_cap_(cchTo) char* rgchTo, int cchTo) MSONOEXCEPT;

// These two APIs will work in-place
LIBLET_PUBLICAPI int MsoSzToSt(_In_z_ const CHAR* sz, _Out_t_cap_(cchMax) BYTE* st, int cchMax) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoSzToWz(_In_z_ const CHAR* szFrom, _Out_opt_z_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoSzToWtzCore(_In_z_ const CHAR* szFrom, _Out_tz_cap_(cchTo) WCHAR* wtzTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoStToSz(_In_t_ const BYTE* st, _Out_z_cap_(cch) CHAR* sz, int cch) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWzToWtz(_In_z_ const WCHAR* wz, _Out_tz_cap_(cchMax) WCHAR* wtz, int cchMax) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWtzToWz(_In_tz_ const WCHAR* wtz, _Out_z_cap_(cchMax) WCHAR* wz, int cchMax) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoWzToSzCore(_In_z_ const WCHAR* wzFrom, _Out_opt_z_cap_(cchTo) CHAR* szTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWzToSzNotInPlace(_In_z_ const WCHAR* wz, _Out_opt_z_cap_(cchTo) CHAR* sz, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI BOOL MsoFSzToWzCore(_In_z_ const CHAR* rgchFrom, _Out_z_cap_(cchTo) WCHAR* rgwchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI BOOL MsoFWzToSzCore(_In_z_ const WCHAR* rgwchFrom, _Out_z_cap_(cchTo) CHAR* rgchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;

LIBLET_PUBLICAPI int MsoCpSzToWzCore(UINT CodePage, _In_z_ const CHAR* szFrom, _Out_opt_z_cap_(cchTo) WCHAR* wzTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoCpRgchToRgwchCore(
	UINT CodePage, _In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoRgwchToCpRgchNotInPlace(UINT CodePage, _In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoRgwchToCpRgchExCore(
	UINT CodePage, _In_count_(cchFrom) const WCHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo, BOOL* fDefault, IMsoMemHeap* pmmh) MSONOEXCEPT;

__inline int MsoRgchToRgwchCore(_In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT
{
	return (MsoCpRgchToRgwchCore(CP_ACP, rgchFrom, cchFrom, rgwchTo, cchTo, pmmh));
}

__inline int MsoRgchToRgwchNotInPlace(_In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo) MSONOEXCEPT
{
	AssertTag((const void*)rgchFrom != (const void*)rgwchTo, 0x01402544 /* tag_bqcve */);
	if ((const void*)(rgchFrom) == (const void*)(rgwchTo))
		return 0;
	return (MsoCpRgchToRgwchCore(CP_ACP, rgchFrom, cchFrom, rgwchTo, cchTo, NULL /*pmmh*/));
}

__inline int MsoCpRgchToRgwchNotInPlace(UINT codePage, _In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo) MSONOEXCEPT
{
	AssertTag((const void*)rgchFrom != (const void*)rgwchTo, 0x01402545 /* tag_bqcvf */);
	if ((const void*)(rgchFrom) == (const void*)(rgwchTo))
		return 0;
	return (MsoCpRgchToRgwchCore(codePage, rgchFrom, cchFrom, rgwchTo, cchTo, NULL /*pmmh*/));
}

__inline int MsoRgwchToCpRgchCore(UINT CodePage, _In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT
{
	return (MsoRgwchToCpRgchExCore(CodePage, rgwchFrom, cchFrom, rgchTo, cchTo, NULL, pmmh));
}

__inline int MsoWtzToSzCore(_In_tz_ const WCHAR* wtzFrom, _Out_opt_z_cap_(cchTo) CHAR* szTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT
{
	int cchFrom = MsoCchWtzLen(wtzFrom) + 1;
	return (MsoRgwchToCpRgchExCore(CP_ACP, MsoWzFromWtz(wtzFrom), cchFrom, szTo, cchTo, NULL, pmmh) - 1);
}

__inline int MsoRgwchToRgchCore(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo, IMsoMemHeap* pmmh) MSONOEXCEPT
{
	return (MsoRgwchToCpRgchExCore(CP_ACP, rgwchFrom, cchFrom, rgchTo, cchTo, NULL, pmmh));
}

__inline int MsoRgwchToRgchNotInPlace(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo) MSONOEXCEPT
{
	AssertTag((const void*)rgwchFrom != (const void*)rgchTo, 0x01402546 /* tag_bqcvg */);
	if ((const void*)(rgwchFrom) == (const void*)(rgchTo))
		return 0;
	return (MsoRgwchToCpRgchExCore(CP_ACP, rgwchFrom, cchFrom, rgchTo, cchTo, NULL, NULL /*pmmh*/));
}

/**

	Global heap wrappers

*/

LIBLET_PUBLICAPI _Ret_range_(0, cchTo - 1) int MsoRgchToRgwch(_In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoFTranslateCp(UINT ucp1, UINT ucp2, _Inout_cap_(cch) char* pchInOut, int cch) MSONOEXCEPT;

__inline BOOL MsoFSzToWz(const CHAR* szFrom, _Out_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT
{
	return (MsoFSzToWzCore(szFrom, wzTo, cchTo, NULL));
}

__inline BOOL MsoFWzToSz(const WCHAR* wzFrom, _Out_cap_(cchTo) CHAR* szTo, int cchTo) MSONOEXCEPT
{
	return (MsoFWzToSzCore(wzFrom, szTo, cchTo, NULL));
}

__inline int MsoWzToSz(_In_z_ const WCHAR* wzFrom, _Out_opt_z_cap_(cchTo) CHAR* szTo, int cchTo) MSONOEXCEPT
{
	return (MsoWzToSzCore(wzFrom, szTo, cchTo, NULL));
}

__inline int MsoWtzToSz(_In_tz_ const WCHAR* wtz, _Out_opt_z_cap_(cchTo) CHAR* sz, int cchTo) MSONOEXCEPT
{
	return (MsoWtzToSzCore(wtz, sz, cchTo, NULL));
}

__inline int MsoSzToWtz(_In_z_ const CHAR* sz, _Out_tz_cap_(cchTo) WCHAR* wtzTo, int cchTo) MSONOEXCEPT
{
	return (MsoSzToWtzCore(sz, wtzTo, cchTo, NULL));
}

__inline int MsoCpSzToWz(UINT cp, _In_z_ const CHAR* sz, _Out_opt_z_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT
{
	return (MsoCpSzToWzCore(cp, sz, wzTo, cchTo, NULL));
}

_Ret_range_(0, cchTo - 1) __inline int MsoRgwchToRgch(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo) MSONOEXCEPT
{
	return (MsoRgwchToRgchCore(rgwchFrom, cchFrom, rgchTo, cchTo, NULL));
}

__inline int MsoCpRgchToRgwch(UINT CodePage, _In_count_(cchFrom) const CHAR* rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR* rgwchTo, int cchTo) MSONOEXCEPT
{
	return (MsoCpRgchToRgwchCore(CodePage, rgchFrom, cchFrom, rgwchTo, cchTo, NULL));
}

__inline int MsoRgwchToCpRgchEx(UINT CodePage, _In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo, BOOL* pfDefault) MSONOEXCEPT
{
	return (MsoRgwchToCpRgchExCore(CodePage, rgwchFrom, cchFrom, rgchTo, cchTo, pfDefault, NULL));
}

LIBLET_PUBLICAPI TASKLIBTECHDEBT_(int) MsoRgwchToCpRgch(UINT CodePage, _In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR* rgchTo, int cchTo) MSONOEXCEPT;

// MSO only
//  (uses vlcidCur)
/* PluggableUI: LIBLET_PUBLICAPI*/ MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToUpper(WCHAR wch) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToLower(WCHAR wch) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoWzUpper(_Inout_z_ WCHAR* wz) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoPwchUpper(_Inout_count_(cch) WCHAR* pwch, int cch) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoWzLower(_Inout_z_ WCHAR* wz) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoPwchLower(_Inout_count_(cch) WCHAR* pwch, int cch) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoSzLower(_Inout_z_ CHAR* sz) MSONOEXCEPT;

// (uses lidNoProof) TODO: move these to string core
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoWzUpperInvariant(_Inout_z_ WCHAR* wz) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoPwchUpperInvariant(_Inout_count_(cch) WCHAR* pwch, int cch) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoWzLowerInvariant(_Inout_z_ WCHAR* wz) MSONOEXCEPT;
/* PluggableUI: LIBLET_PUBLICAPI*/ void MsoPwchLowerInvariant(_Inout_count_(cch) WCHAR* pwch, int cch) MSONOEXCEPT;


// Bad function names being renamed
// TODO: go fix up the source
#define MsoFRgchToRgwch MsoFSzToWz
#define MsoFRgwchToRgch MsoFWzToSz
#define MsoSzToWzCch MsoSzToWz
#define MsoGetStringTypeExW GetStringTypeExW

#endif // LIBLET_ROOT_MSOSTRINGCONVERSION_H
