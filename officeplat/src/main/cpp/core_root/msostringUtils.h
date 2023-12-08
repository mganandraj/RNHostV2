/**
	msoStringUtils.h

	Copyright (c) 1994 Microsoft Corporation

	Locale-invariant functions related to working with strings:
                string reset
		string copy
		string append
		string search
		string length
		string replace
*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGUTILS_H
#define LIBLET_ROOT_MSOSTRINGUTILS_H
#include <limits.h>
// #include <mbstring.h>
// #include <msoAssert.h>
#include <core_root/msoStringTraits.h>
typedef struct IMsoMemHeap IMsoMemHeap;

// #include <WinAPIFamily.h>

/**
	
	Set wtz string to empty

*/
__inline void MsoResetWtz(_Out_cap_c_(2) _Post_tz_ wchar_t* wtz) MSONOEXCEPT
{
	wtz[0] = wtz[1] = 0;
}

/**
	
	String length functions
	Ok to use CRT directly, as Mso versions simply add a null check.

*/

_Ret_range_(==, _String_length_(sz)) __inline int MsoCchSzLen(_In_opt_z_ _Post_readable_size_(return +1) const CHAR* sz) MSONOEXCEPT {
	size_t cch = sz ? strlen(sz) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchSzLen - String too long", 0x0140254b /* tag_bqcvl */);
	return (int)cch;
}

_Ret_range_(==, _String_length_(wz)) __inline int MsoCchWzLen(_In_opt_z_ _Post_readable_size_(return +1) const WCHAR* wz) MSONOEXCEPT
{
	size_t cch = wz ? wcslen(wz) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchWzLen - String too long", 0x0140254c /* tag_bqcvm */);
	return (int)cch;
}

_Ret_range_(<=, _String_length_(sz)) __inline unsigned int MsoCchSzULen(_In_opt_z_ _Post_readable_size_(return +1) const CHAR* sz) MSONOEXCEPT
{
	size_t cch = sz ? strlen(sz) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchSzLen - String too long", 0x0140254d /* tag_bqcvn */);
	return (unsigned int)cch;
}

_Ret_range_(<=, _String_length_(wz))
	_Ret_range_(<=, ULONG_MAX) __inline unsigned int MsoCchWzULen(_In_opt_z_ _Post_readable_size_(return +1) const WCHAR* wz) MSONOEXCEPT
{
	size_t cch = wz ? wcsnlen(wz, ULONG_MAX) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchWzLen - String too long", 0x0140254e /* tag_bqcvo */);
	return (unsigned int)cch;
}

// Or the really recommended function to get string length is:
// returns max if a null-terminator wasn't found
_Ret_range_(<=, _String_length_(sz)) _Ret_range_(<=, max) __inline size_t
	MsoStrnLen(_In_opt_z_ _Post_readable_size_(return +1) const CHAR* sz, size_t max) MSONOEXCEPT
{
	size_t cch = sz ? strnlen(sz, max) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchSzLen - String too long", 0x0140254f /* tag_bqcvp */);
	return cch;
}

_Ret_range_(<=, _String_length_(wz)) _Ret_range_(<=, max) __inline size_t
	MsoWcsnLen(_In_opt_z_ _Post_readable_size_(return +1) const WCHAR* wz, size_t max) MSONOEXCEPT
{
	size_t cch = wz ? wcsnlen(wz, max) : 0;
	AssertSzTag(cch < INT_MAX, "MsoCchWzLen - String too long", 0x01402550 /* tag_bqcvq */);
	return cch;
}

__inline BOOL MsoFEmptyWz(const wchar_t* wz) MSONOEXCEPT
{
	return wz == NULL || wz[0] == L'\0';
}

_Ret_range_(==, _String_length_(wtz)) __inline WORD MsoCchWtzLen(_In_tz_ _Post_readable_size_(return +2) const WCHAR* wtz) MSONOEXCEPT
{
	return (WORD)wtz[0];
}

_Ret_range_(==, _String_length_(wt)) __inline WORD MsoCchWtLen(_In_t_ _Post_readable_size_(return +1) const WCHAR* wt) MSONOEXCEPT
{
	return (WORD)wt[0];
}

/*	Returns the length, in bytes, of the sz derived from wz.  -1 on error */
LIBLET_PUBLICAPI int MsoCpCchSzLenFromWz(UINT CodePage, _In_z_ const WCHAR* wz) MSONOEXCEPT;

/**
	
	String length prefixed helpers (ST, STZ, WT, WTZ)

*/

_Ret_z_ __inline const WCHAR* MsoWzFromWtz(_In_tz_ const WCHAR* wtz) MSONOEXCEPT
{
	return wtz + 1;
}
NON_CONST_VER_DECL(_Ret_z_ __inline WCHAR* MsoWzFromWtz)(_In_tz_ WCHAR* wtz) MSONOEXCEPT
{
	return wtz + 1;
}

/* Returns a pointer to the text part of the string wt */
__inline const WCHAR* MsoRgwchFromWt(_In_t_ const WCHAR* wt) MSONOEXCEPT
{
	return wt + 1;
}
NON_CONST_VER_DECL(__inline WCHAR* MsoRgwchFromWt)(_In_t_ WCHAR* wt) MSONOEXCEPT
{
	return wt + 1;
}

#define WtzToWzCch(wtz) wtz + 1, wtz[0]
#define DefineWtz(wz) \
	{ \
		(WCHAR) _countof(wz) - 1, wz \
	}
#define WzCchFromWtz(wtz, wz, cch) \
	__pragma(warning(push)) __pragma(warning(disable : 4127 4018 4389)) do \
	{ \
		(wz) = MsoWzFromWtz(wtz); \
		(cch) = MsoCchWtzLen(wtz); \
	} \
	while (0) \
	__pragma(warning(pop))

/**
	
	String Buffer size calculations

*/

#ifdef __cplusplus

// #include <safeint/safealloct.h> // MsoCbBufSizeT, INVALID_BUFFER_SIZE, MAXIMUM_BUFFER_SIZE

inline constexpr size_t INVALID_BUFFER_SIZE = ((size_t)~0);
inline constexpr size_t MAXIMUM_BUFFER_SIZE = ((size_t)INT_MAX);

template <typename T>
_Ret_range_(==, (cElements + cExtra) * sizeof(T))
size_t MsoCbBufSizeT(size_t cElements, size_t cExtra = 0, size_t cbMaxBufSize = MAXIMUM_BUFFER_SIZE) noexcept
{
	//first calculate maximum allowed
	//typical case is a compile time constant
	const size_t cMax = cbMaxBufSize/sizeof(T);
	const size_t cMaxAllowed = cMax - cExtra;

	if(cExtra > cMax            || //cExtra is too large
	   cElements > cMaxAllowed)    //real check here
	{
#if DEBUG
		// TODO: (CWhite) This should be changed to a tagged function
		//__asm int 3
#endif
		//we have a problem
		return INVALID_BUFFER_SIZE;
	}

	return (cElements + cExtra) * sizeof(T);
}

/*
* MsoCbBufSizeCwch
* This function returns the bytes needed to allocate a buffer for (_countof + cExtra)
* instances of WCHAR.
*
* Arguments:
* cchElements - the number of elements needed
* cchExtra - any additional elements needed
* cbMaxBufSize - optional argument for the maximum bytes allowed. Default is the maximum
*            possible value for the architecture.
*
* Return:
* Either a mathematically correct number of bytes required, or in the case of an error,
* INVALID_BUFFER_SIZE
*
* Revision history:
* 10/22/04 - dleblanc - created
*/


_Ret_range_(==, (cchLength + cchExtra) * 2) __inline size_t
	MsoCbBufSizeCwch(size_t cchLength, size_t cchExtra = 0, size_t cbMaxBufSize = MAXIMUM_BUFFER_SIZE) MSONOEXCEPT
{
	return MsoCbBufSizeT<WCHAR>(cchLength, cchExtra, cbMaxBufSize);
}

/*
* MsoCbBufSizeCch
* This function returns the bytes needed to allocate a buffer for (_countof + cExtra)
* instances of CHAR.
*
* Arguments:
* cchElements - the number of elements needed
* cchExtra - any additional elements needed
* cbMaxBufSize - optional argument for the maximum bytes allowed. Default is the maximum
*            possible value for the architecture.
*
* Return:
* Either a mathematically correct number of bytes required, or in the case of an error,
* INVALID_BUFFER_SIZE
*
* Revision history:
* 10/22/04 - dleblanc - created
*/
_Ret_range_(==, (cchLength + cchExtra) * 1) __inline size_t
	MsoCbBufSizeCch(size_t cchLength, size_t cchExtra = 0, size_t cbMaxBufSize = MAXIMUM_BUFFER_SIZE) MSONOEXCEPT
{
	return MsoCbBufSizeT<CHAR>(cchLength, cchExtra, cbMaxBufSize);
}

/*
* MsoCbBufSizeWz
* This function returns the bytes needed to allocate a buffer for a null-terminated
* WCHAR (Unicode) string.
*
* Arguments:
* wzString - input string. If wzString is not null terminated, this may create read exceptions.
* cchExtra - any additional characters needed. Typically, this will be 1 for the null terminator.
* pcCharCount - optional pointer that receives the number of characters in the string, not counting the
*                trailing null or cchExtra
*
* Return:
* Either a mathematically correct number of bytes required, or in the case of an error,
* INVALID_BUFFER_SIZE
*
* Revision history:
* 10/22/04 - dleblanc - created
*/
__inline size_t MsoCbBufSizeWz(_In_z_ const WCHAR* wzString, size_t cchExtra = 0, _Out_opt_ size_t* pcch = nullptr) MSONOEXCEPT
{
	size_t cchLen = wcslen(wzString);
	if (pcch != nullptr)
		*pcch = cchLen;

	// cchLen + 1 cannot practically overflow
	return MsoCbBufSizeT<WCHAR>(cchLen + 1, cchExtra);
}

/*
* CbBufSizeSz
* This function returns the bytes needed to allocate a buffer for a null-terminated
* ANSI (char) string.
*
* Arguments:
* szString - input string. If wzString is not null terminated, this may create read exceptions.
* cchExtra - any additional characters needed. Typically, this will be 1 for the null terminator.
* pcCharCount - optional pointer that receives the number of characters in the string, not counting the
*                trailing null or cchExtra
*
* Return:
* Either a mathematically correct number of bytes required, or in the case of an error,
* INVALID_BUFFER_SIZE
*
* Revision history:
* 10/22/04 - dleblanc - created
*/

__inline size_t MsoCbBufSizeSz(_In_z_ const char* szString, size_t cchExtra = 0, _Out_opt_ size_t* pcCharCount = nullptr) MSONOEXCEPT
{
	size_t cchLen = strlen(szString);
	if (pcCharCount != nullptr)
		*pcCharCount = cchLen;

	// cchLen + 1 cannot practically overflow
	return MsoCbBufSizeT<char>(cchLen + 1, cchExtra);
}

#endif // __cplusplus

/*
* MsoCbBufSizeCb
* This function returns the bytes needed to allocate a buffer for nElements +
* nExtraElements of size cbElement, plus an extra count of bytes for the "head."
*
* Arguments:
* cbElement - size of 1 item in bytes.
* nElements - base number of elements
* nExtraElements -  extra element count, for various purposes
* cbHead - extra byte count to be used for the "head" of the structure being
* allocated
*
* Return:
* Either a mathematically correct number of bytes required, or in the case of an error,
* INVALID_BUFFER_SIZE
*
* Revision history:
* 08/13/05 - willbr - created
*/
LIBLET_PUBLICAPI size_t MsoCbBufSizeCb(size_t cbElement, size_t nElements, size_t nExtraElements, size_t cbHead) MSONOEXCEPT;

/**
	
	Character Copying
	
	Copy a source string into a target buffer with room for cchDest chars.
	Target buffer is returned null terminated, truncating source if needed.
	The return value is the target buffer, except for MsoSzCopy / MsoWzCopy
	Consider calling the CRT directly.

*/

LIBLET_PUBLICAPI MSOEXTERN_C CHAR* MsoRgchCopy(_In_count_(cchSrc) const CHAR* rgchSrc, int cchSrc, _Out_z_cap_(cchDest) CHAR* szDest, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C WCHAR* MsoRgwchCopy(_In_count_(cchSrc) const WCHAR* rgwchSrc, int cchSrc, _Out_z_cap_(cchDest) WCHAR* wzDest, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWtRgwchCopy(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_t_cap_(cchMax) WCHAR* wtTo, int cchMax) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoWtzRgwchCopy(_In_count_(cchFrom) const WCHAR* rgwchFrom, int cchFrom, _Out_tz_cap_(cchMax) WCHAR* wtzTo, int cchMax) MSONOEXCEPT;

#define strncpy_s(d,ds,s,n) strncpy(d,s,n)
#define wcsncpy_s(d,ds,s,n) wcsncpy(d,s,n)

__inline CHAR* MsoSzCopy(_In_z_ const CHAR* szFrom, _Out_z_cap_(cchTo) CHAR* szTo, int cchTo) MSONOEXCEPT
{
	if (cchTo <= 0)
		return (szTo);
	strncpy_s(szTo, (size_t)cchTo, szFrom, _TRUNCATE);
	return szTo + strlen(szTo);
}

__inline WCHAR* MsoWzCopy(_In_z_ const WCHAR* wzFrom, _Out_z_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT
{
	if (cchTo <= 0)
		return (wzTo);
	wcsncpy_s(wzTo, (size_t)cchTo, wzFrom, _TRUNCATE);
	return wzTo + wcslen(wzTo);
}

__inline int MsoWtCopy(_In_t_ const WCHAR* wtFrom, _Out_t_cap_(cchTo) WCHAR* wtTo, int cchTo) MSONOEXCEPT
{
	return MsoWtRgwchCopy(MsoRgwchFromWt(wtFrom), MsoCchWtLen(wtFrom), wtTo, cchTo);
}

__inline int MsoWtzCopy(_In_tz_ const WCHAR* wtzFrom, _Out_tz_cap_(cchTo) WCHAR* wtzTo, int cchTo) MSONOEXCEPT
{
	return MsoWtzRgwchCopy(MsoWzFromWtz(wtzFrom), MsoCchWtzLen(wtzFrom), wtzTo, cchTo);
}

__inline void MsoWtzCopyNull(_In_opt_tz_ const WCHAR* wtzFrom, _Out_tz_cap_(cchTo) WCHAR* wtzTo, int cchTo) MSONOEXCEPT
{
	if (wtzFrom == NULL)
		wtzFrom = L"\0";
	MsoWtzCopy(wtzFrom, wtzTo, cchTo);
}

// REVIEW: [danjump] This needs to go away
#if (OACR)
__inline WCHAR* MsoWzRgwchCchCopy(_In_count_(cch) const WCHAR* rgwchFrom, _Out_z_cap_(cch + 1) WCHAR* wzTo, int cch) MSONOEXCEPT
{
	return MsoRgwchCopy(rgwchFrom, cch, wzTo, ((cch) + 1));
}
#else // OACR
// WARNING! this is unsafe.  There is no target buffer
#define MsoWzRgwchCchCopy(rgwchFrom, wzTo, cch) MsoRgwchCopy(rgwchFrom, cch, wzTo, ((cch) + 1))
#endif // OACR

/**
	
	Character Appending
	
	Append a source string into a target buffer with room for cchTo chars.
	Target buffer must be initially null terminated.
	Target buffer is returned null terminated, truncating source if needed.
	Return value is a pointer to the null terminator in the target buffer.
	This allows for easy chaining, assuming that truncation can be ignored.

	These functions differ from wcscat_s/strcat_s in two ways:

	1. The CRT APIs return an error code, so they're not easy to chain

	2. The CRT APIs invoke the invalid_parameter handler (see:
	https://docs.microsoft.com/en-us/cpp/c-runtime-library/parameter-validation)
	on failure. These functions silently truncate.

	Example usage:
	WCHAR wzDest[100];
	wzDest[0] = L'\0';
	const WCHAR *pwzEnd = std::end(wzDest);
	
	// Function template that infers `cchTo` from the size of the provided array
	// Equivalent to calling: MsoWzAppend(L"Start", wzDest, _countof(wzDest));
	WCHAR* pwzDest = MsoWzAppend(L"Start", wzDest); 
	pwzDest = MsoWzAppend(L"Middle", pwzDest, static_cast<int>(pwzEnd - pwzDest));
	pwzDest = MsoWzAppend(L"End", pwzDest, static_cast<int>(pwzEnd - pwzDest));

*/

LIBLET_PUBLICAPI MSOEXTERN_C char* MsoSzAppend(_In_z_ const CHAR* szFrom, _Inout_z_cap_(cchTo) CHAR* szTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C WCHAR* MsoWzAppend(_In_z_ const WCHAR* wzFrom, _Inout_z_cap_(cchTo) WCHAR* wzTo, int cchTo) MSONOEXCEPT;
LIBLET_PUBLICAPI void MsoWtzAppend(_In_tz_ const WCHAR* wtzFrom, _Inout_tz_cap_(cch) WCHAR* wtzTo, int cch) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C CHAR* MsoRgchAppend(_In_count_(cchSrc) const CHAR* rgchSrc, int cchSrc, _Inout_z_cap_(cchDest) CHAR* szDest, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C WCHAR* MsoRgwchAppend(_In_count_(cchSrc) const WCHAR* rgwchSrc, int cchSrc, _Inout_z_cap_(cchDest) WCHAR* wzDest, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI WCHAR* MsoRgwchWtzAppend(_In_count_(cchSrc) const WCHAR* rgwchSrc, int cchSrc, _Inout_tz_cap_(cchDest) WCHAR* wtzDest, int cchDest) MSONOEXCEPT;
LIBLET_PUBLICAPI WCHAR* MsoRgwchWtAppend(_In_count_(cchSrc) const WCHAR* rgwchSrc, int cchSrc, _Inout_t_cap_(cchDest) WCHAR* wtDest, int cchDest) MSONOEXCEPT;

#ifdef __cplusplus

template<size_t cch>
WCHAR* MsoWzAppend(_In_z_ const WCHAR* wzFrom, _Inout_z_ WCHAR (&wzTo)[cch]) MSONOEXCEPT
{
	static_assert(cch <= INT_MAX, "You can't create 4GB strings");
	return MsoWzAppend(wzFrom, wzTo, static_cast<int>(cch));
}

#endif


/**
	
	Character Searching
	
	Return the pointer to the first (or last) occurance of a given character
	Return NULL if the character is not found.

*/

//#pragma region Desktop Family
//#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//__inline const char* MsoSzIndex(const char* sz, char c) MSONOEXCEPT
//{
//	return (const char*)_mbschr((const unsigned char*)sz, (unsigned char)c);
//};
//NON_CONST_VER_DECL(__inline char* MsoSzIndex)(char* sz, char c) MSONOEXCEPT
//{
//	return (char*)_mbschr((unsigned char*)sz, (unsigned char)c);
//};
//#endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//#pragma endregion

_When_(return != nullptr, _Ret_range_(wz, wz + c)) __inline const WCHAR* MsoWzIndex(const WCHAR* wz, WCHAR c) MSONOEXCEPT
{
	return wcschr(wz, c);
};
_When_(return != nullptr, _Ret_range_(wz, wz + c)) NON_CONST_VER_DECL(__inline WCHAR* MsoWzIndex)(_Const_ WCHAR* wz, WCHAR c) MSONOEXCEPT
{
	OACR_USE_PTR(wz);
	return wcschr(wz, c);
};

LIBLET_PUBLICAPI const CHAR* MsoRgchIndexSimple(const CHAR* rgch, int cch, char c) MSONOEXCEPT;
NON_CONST_VER_DECL(__inline CHAR* MsoRgchIndexSimple)(_In_count_(cch) CHAR* rgch, int cch, char c) MSONOEXCEPT
{
	OACR_USE_PTR(rgch);
	return (CHAR*)MsoRgchIndexSimple((const CHAR*)rgch, cch, c);
}

LIBLET_PUBLICAPI _When_(return != nullptr, _Ret_range_(rgwch, rgwch + cch - 1)) const WCHAR* MsoRgwchIndex(const WCHAR* rgwch, int cch, int wch) MSONOEXCEPT;
_When_(return != nullptr,
		 _Ret_range_(rgwch, rgwch + cch - 1)) NON_CONST_VER_DECL(__inline WCHAR* MsoRgwchIndex)(_Const_ _In_count_(cwch) WCHAR* rgwch, int cwch, int ch) MSONOEXCEPT
{
	OACR_USE_PTR(rgwch);
	return (WCHAR*)MsoRgwchIndex((const WCHAR*)rgwch, cwch, ch);
}

/*	Does not work on DBCS */
__inline const char* MsoSzIndexOneOf(_In_opt_z_ const char* sz, _In_z_ const char* szCharSet) MSONOEXCEPT
{
	size_t cch;
	if (sz == NULL)
		return NULL;
	cch = strcspn(sz, szCharSet);
	return sz[cch] == 0 ? NULL : sz + cch;
}
NON_CONST_VER_DECL(__inline char* MsoSzIndexOneOf)(_In_opt_z_ char* sz, _In_z_ const char* szCharSet) MSONOEXCEPT
{
	OACR_USE_PTR(sz);
	return (char*)MsoSzIndexOneOf((const char*)sz, szCharSet);
}

_When_(return != nullptr, _Ret_range_(wz, wz + _String_length_(wz))) __inline const WCHAR* MsoWzIndexOneOf(_In_opt_z_ const WCHAR* wz, _In_z_ const WCHAR* wzCharSet) MSONOEXCEPT
{
	size_t cch;
	if (wz == NULL)
		return NULL;
	cch = wcscspn(wz, wzCharSet);
	return wz[cch] == 0 ? NULL : wz + cch;
}
_When_(return != nullptr,
		 _Ret_range_(wz, wz + _String_length_(wz))) NON_CONST_VER_DECL(__inline WCHAR* MsoWzIndexOneOf)(_Const_ _In_opt_z_ WCHAR* wz, _In_z_ const WCHAR* wzCharSet) MSONOEXCEPT
{
	OACR_USE_PTR(wz);
	return (WCHAR*)MsoWzIndexOneOf((const WCHAR*)wz, wzCharSet);
}

/*	Just like MsoSzIndex, except the string is guaranteed to be full of
	single-byte ANSI characters (i.e., no DBCS characters) */
__inline const char* MsoSzIndex1(_In_opt_z_ const char* sz, char ch) MSONOEXCEPT
{
	return sz ? strchr(sz, ch) : NULL;
}
NON_CONST_VER_DECL(__inline char* MsoSzIndex1)(_In_opt_z_ char* sz, char ch) MSONOEXCEPT
{
	OACR_USE_PTR(sz);
	return sz ? strchr(sz, ch) : NULL;
}

//#pragma region Desktop Family
//#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//__inline const char* MsoSzIndexRight(const char* sz, char c) MSONOEXCEPT
//{
//	return (const char*)_mbsrchr((const unsigned char*)sz, (unsigned char)c);
//};
//NON_CONST_VER_DECL(__inline char* MsoSzIndexRight)(char* sz, char c) MSONOEXCEPT
//{
//	return (char*)_mbsrchr((unsigned char*)sz, (unsigned char)c);
//};
//#endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//#pragma endregion

_When_(return != nullptr, _Ret_range_(wz, wz + _String_length_(wz))) __inline const WCHAR* MsoWzIndexRight(const WCHAR* wz, WCHAR c) MSONOEXCEPT
{
	return wcsrchr(wz, c);
}
_When_(return != nullptr,
		 _Ret_range_(wz, wz + _String_length_(wz))) NON_CONST_VER_DECL(__inline WCHAR* MsoWzIndexRight)(_Const_ _In_z_ WCHAR* wz, WCHAR c) MSONOEXCEPT
{
	OACR_USE_PTR(wz);
	return wcsrchr(wz, c);
}

LIBLET_PUBLICAPI const CHAR* MsoRgchIndexRight(const CHAR* rgch, int cch, CHAR ch) MSONOEXCEPT;
NON_CONST_VER_DECL(__inline CHAR* MsoRgchIndexRight)(_In_count_(cch) CHAR* rgch, int cch, CHAR ch) MSONOEXCEPT
{
	OACR_USE_PTR(rgch);
	return (CHAR*)MsoRgchIndexRight((const CHAR*)rgch, cch, ch);
}

LIBLET_PUBLICAPI const WCHAR* MsoRgwchIndexRight(const WCHAR* rgwch, int cch, WCHAR wch) MSONOEXCEPT;
_When_(return != nullptr,
		 _Ret_range_(rgwch, rgwch + cwch - 1)) NON_CONST_VER_DECL(__inline WCHAR* MsoRgwchIndexRight)(_Const_ _In_count_(cwch) WCHAR* rgwch, int cwch, WCHAR wch) MSONOEXCEPT
{
	OACR_USE_PTR(rgwch);
	return (WCHAR*)MsoRgwchIndexRight((const WCHAR*)rgwch, cwch, wch);
}

/*	Finds the last (rightmost) occurance of the character wch in the null-terminated
	UNINCODE  string wz starting from char position ich, and returns a pointer to it.
	Returns NULL if the character was not in the string. */
LIBLET_PUBLICAPI _When_(return != nullptr, _Ret_range_(wz, wz + _String_length_(wz))) const WCHAR* MsoCchWzIndexRight(_In_opt_z_ const WCHAR* wz, INT_PTR ich, int wch) MSONOEXCEPT;
_When_(return != nullptr,
		 _Ret_range_(wz, wz + _String_length_(wz))) NON_CONST_VER_DECL(__inline WCHAR* MsoCchWzIndexRight)(_Const_ _In_opt_z_ WCHAR* wz, INT_PTR ich, int wch) MSONOEXCEPT
{
	OACR_USE_PTR(wz);
	return (WCHAR*)MsoCchWzIndexRight((const WCHAR*)wz, ich, wch);
}

/**
	
	String Searching
	
	Return the pointer to the first (or last) occurance of a given string.
	Return NULL if the character is not found.
	*Fast variations are Invariant (e.g. use memcmp)

*/
//
//#pragma region Desktop Family
//#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//__inline const char* MsoSzStrStr(const char* sz, const char* c) MSONOEXCEPT
//{
//	if (!sz)
//		return NULL;
//	if (!c)
//	{
//		ShipAssertSzTag(0, "Not reached", 0x01402551 /* tag_bqcvr */);
//		return sz;
//	}
//	return (const char*)(_mbsstr((const unsigned char*)(sz), (const unsigned char*)(c)));
//}
//NON_CONST_VER_DECL(__inline char* MsoSzStrStr)(char* sz, const char* c) MSONOEXCEPT
//{
//	if (!sz)
//		return NULL;
//	if (!c)
//	{
//		ShipAssertSzTag(0, "Not reached", 0x01402552 /* tag_bqcvs */);
//		return sz;
//	}
//	return (char*)(_mbsstr((unsigned char*)(sz), (unsigned const char*)(c)));
//}
//#endif // WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//#pragma endregion

_When_(return != nullptr, _Ret_range_(wz, wz + _String_length_(wz))) _Ret_opt_z_ __inline const WCHAR* MsoWzStrStr(_In_z_ const WCHAR* wz, _In_z_ const WCHAR* wzFind) MSONOEXCEPT
{
	if (!wz)
		return NULL;
	if (!wzFind)
	{
		ShipAssertSzTag(0, "Not reached", 0x01402553 /* tag_bqcvt */);
		return wz;
	};
	return wcsstr(wz, wzFind);
}

_When_(return != nullptr,
		 _Ret_range_(wz, wz + _String_length_(wz))) NON_CONST_VER_DECL(_Ret_opt_z_ __inline WCHAR* MsoWzStrStr)(_Const_ _In_z_ WCHAR* wz, _In_z_ const WCHAR* wzFind) MSONOEXCEPT
{
	OACR_USE_PTR(wz);
	if (!wz)
		return NULL;
	if (!wzFind)
	{
		ShipAssertSzTag(0, "Not reached", 0x01402554 /* tag_bqcvu */);
		return wz;
	};
	return wcsstr(wz, wzFind);
}

LIBLET_PUBLICAPI MSOEXTERN_C const WCHAR* MsoPwchStrStrFast(_In_count_(cwch) const WCHAR* pwch, int cwch, _In_count_(cwchFind) const WCHAR* pwchFind, int cwchFind) MSONOEXCEPT;
NON_CONST_VER_DECL(__inline WCHAR* MsoPwchStrStrFast)
(_In_count_(cwch) WCHAR* pwch, int cwch, _In_count_(cwchFind) const WCHAR* pwchFind, int cwchFind) MSONOEXCEPT
{
	OACR_USE_PTR(pwch);
	return (WCHAR*)MsoPwchStrStrFast((const WCHAR*)pwch, cwch, pwchFind, cwchFind);
}

LIBLET_PUBLICAPI const WCHAR* MsoPwchStrStrRightFast(_In_count_(cwch) const WCHAR* pwch, int cwch, _In_count_(cwchFind) const WCHAR* pwchFind, int cwchFind) MSONOEXCEPT;
NON_CONST_VER_DECL(__inline WCHAR* MsoPwchStrStrRightFast)
(_In_count_(cwch) WCHAR* pwch, int cwch, _In_count_(cwchFind) const WCHAR* pwchFind, int cwchFind) MSONOEXCEPT
{
	OACR_USE_PTR(pwch);
	return (WCHAR*)MsoPwchStrStrRightFast((const WCHAR*)pwch, cwch, pwchFind, cwchFind);
}

// Similar to strtok
LIBLET_PUBLICAPI _Ret_opt_z_ MSOEXTERN_C PWSTR MsoWzToken(_Inout_ _Deref_prepost_z_ PWSTR* pwzToken, _In_z_ PCWSTR wzDelimiterList) MSONOEXCEPT;


/**
	
	String Replacement
	
*/

__inline void MsoWzReplaceFirstFoundWch(_Inout_z_ WCHAR* wz, WCHAR wchOut, WCHAR wchIn) MSONOEXCEPT
{
	WCHAR* pchFound = NON_CONST_VER(MsoWzIndex)(wz, wchOut);
	if (pchFound)
		*pchFound = wchIn;
}
__inline void MsoRgchReplaceFirstFoundWch(_Out_cap_(cwch) WCHAR* rgwch, int cwch, WCHAR wchOut, WCHAR wchIn) MSONOEXCEPT
{
	WCHAR* pchFound = NON_CONST_VER(MsoRgwchIndex)(rgwch, cwch, wchOut);
	if (pchFound)
		*pchFound = wchIn;
}

__inline void MsoWzReplaceLastFoundWch(_Inout_z_ WCHAR* wz, WCHAR wchOut, WCHAR wchIn) MSONOEXCEPT
{
	WCHAR* pchFound = NON_CONST_VER(MsoWzIndexRight)(wz, wchOut);
	if (pchFound)
		*pchFound = wchIn;
}

LIBLET_PUBLICAPI void MsoReplaceAllOfWchWithWch(_Inout_z_ WCHAR* wz, WCHAR wchOut, WCHAR wchIn) MSONOEXCEPT;

/*	This function strips the accent of WinLatin1 and Greek characters.
	Please note that European Latin, Extended Latin, and Cyrillic
	characters are unaffected. */
LIBLET_PUBLICAPI WCHAR MsoWchStripAccent(WCHAR wch) MSONOEXCEPT;


// Global heap versions
LIBLET_PUBLICAPI BOOL MsoFStripLeadingAndEndingWSWz(const WCHAR* wzIn, _Deref_out_z_ WCHAR** wzOut) MSONOEXCEPT;
LIBLET_PUBLICAPI BOOL MsoFStripLeadingAndEndingWSWzCore(const WCHAR* wzIn, _Deref_out_z_ WCHAR** wzOut, IMsoMemHeap* pmmh) MSONOEXCEPT;

LIBLET_PUBLICAPI WCHAR* MsoWzQueryValue(const void* pb, unsigned int cb, const WCHAR* wz, unsigned int cch) MSONOEXCEPT;

// Legacy functions
#define MsoWzCchCopy MsoWzCopy
#define MsoSzCchCopy MsoSzCopy
#define MsoWzCchAppend MsoWzAppend

// Strip Flags for MsoPwchStripWtz()
#define msofStripOddAmpersands 0x00000001
#define msofStripTrailingColon 0x00000002
#define msofStripTrailingElipses 0x00000004
#define msofStripAccelarator 0x00000008 // will remove OddAmpersand and also (&N)
#define msofStripMultZeroWidthSpaces 0x00000010 // remove runs of two or three zero width
// space characters (see o14 60956)

/* PluggableUI: LIBLET_PUBLICAPI*/ WCHAR* MsoPwchStripWtzCore(_Inout_tz_ WCHAR* wtz, int rgfStrip, LCID lcid) MSONOEXCEPT;
/* StringIntl: LIBLET_PUBLICAPI*/ void StripFEAmpersands(_Inout_tz_ WCHAR* wtz) MSONOEXCEPT;
LIBLET_PUBLICAPI WCHAR* MsoWzStripSpaces(_Inout_z_ WCHAR* pwch) MSONOEXCEPT;

#endif // LIBLET_ROOT_MSOSTRINGUTILS_H
