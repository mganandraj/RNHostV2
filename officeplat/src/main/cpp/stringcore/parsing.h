/*-------------------------------------------------------------------------
	msoStringParsing.h

	Copyright (c) 1994 Microsoft Corporation

	Functions related to parsing simple data types to/from strings
	FUTURE: can any of these be replaced with atoi, et al?

	%%Owner: AskIntl
	%%Id: 96dc3da1-b7a4-424b-bdc2-7c21d7423405
-------------------------------------------------------------------------*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGPARSING_H
#define LIBLET_ROOT_MSOSTRINGPARSING_H
//#include <msoCore.h>
typedef struct IMsoMemHeap IMsoMemHeap;

/*-------------------------------------------------------------------------
	Decoding functions convert something into the string buffer.
	Returns the number of characters written (not including trailing zero)
-------------------------------------------------------------------------*/

LIBLET_PUBLICAPI MSOEXTERN_C
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeInt(_Out_z_cap_(cch) WCHAR *rgwch, int cch, int w, int wBase) MSONOEXCEPT;

LIBLET_PUBLICAPI MSOEXTERN_C
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeInt64(_Out_z_cap_(cch) WCHAR *rgwch, int cch, __int64 w, int wBase) MSONOEXCEPT;

MSOEXTERN_C LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeUint(_Out_z_cap_(cch) WCHAR *rgwch, int cch, unsigned u, int wBase) MSONOEXCEPT;

LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeUint64(
	_When_(cch <= 1, _At_(rgwch, _Out_cap_(cch)))
	_When_(cch > 1, _At_(rgwch, _Out_z_cap_(cch))) WCHAR *rgwch, int cch, UINT64 u, int wBase) MSONOEXCEPT;

LIBLET_PUBLICAPI int MsoWzDecodeUIntFill(_Out_z_cap_(cch) WCHAR *rgwch, int cch, unsigned u, int wBase) MSONOEXCEPT;

MSOEXTERN_C LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeDouble(_Out_z_cap_(cchMax) WCHAR *rgwch, int cchMax, double d) MSONOEXCEPT;

LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(rgwch))
int MsoWzDecodeDoubleEx(
	_When_(cch <= 0, _At_(rgwch, _Out_cap_(cch)))
	_When_(cch > 0 , _At_(rgwch, _Out_z_cap_(cch))) WCHAR *rgwch, int cch, const WCHAR wchDp, double d) MSONOEXCEPT;

MSOEXTERN_C LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(sz))
int MsoSzDecodeInt(_Out_z_cap_(cch) CHAR* sz, int cch, int w, int wBase) MSONOEXCEPT;

MSOEXTERN_C LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(sz))
int MsoSzDecodeUint(_Out_z_cap_(cch) CHAR* sz, int cch, unsigned u, int wBase) MSONOEXCEPT;

LIBLET_PUBLICAPI
_Post_equal_to_(_String_length_(sz))
int MsoSzDecodeSize_t(_Out_z_cap_(cch) CHAR* sz, int cch, size_t u, int wBase) MSONOEXCEPT;
//
//LIBLET_PUBLICAPI
//_Success_(return > 0)
//_Post_equal_to_(_String_length_(wz))
//int MsoWzDecodeTime(_Out_z_cap_(cch) WCHAR* wz, int cch, const SYSTEMTIME *pst) MSONOEXCEPT;
//
//_Post_equal_to_(_String_length_(wz))
//__inline int MsoWzDecodeIntPtr(_Out_z_cap_(cchMax) WCHAR* wz, int cchMax, INT_PTR u, int wBase) MSONOEXCEPT
//{
//#if OFFICE64
//	return MsoWzDecodeInt64(wz, cchMax, u, wBase);
//#else
//	return MsoWzDecodeInt(wz, cchMax, (int)u, wBase);
//#endif
//}
//
//_Post_equal_to_(_String_length_(wz))
//__inline int MsoWzDecodeUintPtr(_Out_z_cap_(cchMax) WCHAR* wz, int cchMax, UINT_PTR u, int wBase) MSONOEXCEPT
//{
//#if OFFICE64
//	return MsoWzDecodeUint64(wz, cchMax, u, wBase);
//#else
//	return MsoWzDecodeUint(wz, cchMax, (UINT)u, wBase);
//#endif
//}

// Converts each byte in rgbBytes to two hex characters and returns the hex string in rgwchHex.
LIBLET_PUBLICAPI
_Success_(return > 0)
_Post_equal_to_(_String_length_(rgwchEncodedHex))
unsigned int MsoHexFromBytes(
	_Out_z_cap_(cchEncodedHex) wchar_t *rgwchEncodedHex,
	unsigned int cchEncodedHex,
	_In_bytecount_(cbBytesToEncode) const BYTE* rgbBytesToEncode,
	unsigned int cbBytesToEncode) MSONOEXCEPT;

/*-------------------------------------------------------------------------
	Parsing functions convert a string into something.
	Returns the number of characters used, including leading whitespace
-------------------------------------------------------------------------*/

LIBLET_PUBLICAPI MSOEXTERN_C int MsoParseIntWz(const WCHAR *rgwch, int *pw) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoParseInt64Wz(const WCHAR* rgwch, __int64 *pw) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoParseUIntWz(const WCHAR *rgwch, unsigned *pw) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoParseUInt64Wz(const WCHAR* rgwch, uint64_t *pw) MSONOEXCEPT;
LIBLET_PUBLICAPI MSOEXTERN_C int MsoParseHexIntWz(const WCHAR *wz, int *pdw) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoParseHexUIntWz(const WCHAR *wz, unsigned *pu) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoParseDoubleWz(const WCHAR *rgwch, double *pd) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoParseDoubleWzEx(const WCHAR *rgwch, const WCHAR wchDp, double *pd) MSONOEXCEPT;

/* TODO: LIBLET_PUBLICAPI*/ int MsoParseIntSz(const CHAR *rgch, int *pdw) MSONOEXCEPT;
/* TODO: LIBLET_PUBLICAPI*/ int MsoParseUIntSz(const CHAR *rgch, unsigned *pdw) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoParseHexIntSz(const CHAR *sz, int *pdw) MSONOEXCEPT;

// Expects time in ISO8061 format.  Returns time in Z-time, not local time.
LIBLET_PUBLICAPI int MsoParseTimeWz(const WCHAR* rgwch, SYSTEMTIME *pst) MSONOEXCEPT;

LIBLET_PUBLICAPI int MsoFParseBoolWz(LPCWSTR wz) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFGuidFromWz(const WCHAR *wz, GUID *pguid) MSONOEXCEPT;
LIBLET_PUBLICAPI int MsoINumFromHebrewWz(LPCWSTR wzHeb) MSONOEXCEPT;

// LIBLET_PUBLICAPI HRESULT HrRfc1123DateToSystemTime(_In_z_ const WCHAR* wzRfc1123Date, _Out_ SYSTEMTIME* pSystemTime) MSONOEXCEPT;

// Legacy name mappings
#define MsoIntEncodeSz MsoParseIntSz
#define MsoIntEncodeWz MsoParseIntWz
//
//__inline int MsoParseIntPtrWz(const WCHAR *rgwch, INT_PTR *pw) MSONOEXCEPT
//{
//#if OFFICE64
//	return MsoParseInt64Wz(rgwch, (__int64 *)pw);
//#else
//	return MsoParseIntWz(rgwch, (int*)pw);
//#endif
//}
//
//__inline int MsoParseUIntPtrWz(const WCHAR *rgwch, UINT_PTR *pw) MSONOEXCEPT
//{
//#if OFFICE64
//	return MsoParseUInt64Wz(rgwch, (unsigned __int64 *)(pw));
//#else
//	return MsoParseUIntWz(rgwch, (unsigned *)(pw));
//#endif
//}

/*----------------------------------------------------------------------------
	MsoValHexWch
	Returns the value of the hex digit in wch.
----------------------------------------------------------------------------*/
__inline int MsoValHexWch(WCHAR wch) MSONOEXCEPT
{
	if (L'0' <= wch && wch <= L'9')
		return(wch - L'0');
	wch &= 0xFFDF; // upper-case wch
	if (L'A' <= wch && wch <= L'F')
		return(wch - L'A' + 10);
	return(-1);
}

#ifdef __cplusplus

/*------------------------------------------------------------------------------
	Converts the hex string to BYTE array. One BYTE for two hex digits.
-------------------------------------------------------------------------------*/
LIBLET_PUBLICAPI bool MsoBytesFromHex(
	const wchar_t* wzFrom,
	_Out_bytecap_(cb) BYTE *pb,
	_Inout_ DWORD& cb) MSONOEXCEPT;

/*------------------------------------------------------------------------------
	SystemTimeToISO8601

	Decodes the system time to an ISO8601 GMT time.
	For use when you know where the SYSTEMTIME came from (FileTimeToSystemTime
	doesn't usually give you bad outputs) and don't need to do 6 division operations.
--------------------------------------------------------------------dleblanc---*/
//LIBLET_PUBLICAPI bool SystemTimeToISO8601(
//	const SYSTEMTIME * pstTime,
//	_Out_writes_z_(cch) wchar_t* wzISO8601Time,
//	ULONG cch) MSONOEXCEPT;
//constexpr inline size_t CchMaxISO8601Time = 24;

/*------------------------------------------------------------------------------
	ParseISO8601FileTime

	Parses out a file time from an ISO8601 GMT time.

	Note - does not tolerate things like no supplied date, and also
	does not deal with anything other than universal time.

	%%Id: 54d706ee-f67b-43f0-ae13-ecd953c93750
--------------------------------------------------------------------dleblanc---*/
//LIBLET_PUBLICAPI bool ParseISO8601FileTime(_In_z_ LPCWSTR wzISO8601Time, ::FILETIME * pftTime) MSONOEXCEPT;

/*-----------------------------------------------------------------------------
	%%id:  667864ff-11a8-407f-9a33-2b9cf99b9e5d
	MsoFUnescapePercentHexHex
	Converts a string starting with %HH form into a WCHAR.
----------------------------------------------------------------------------*/
template< typename T >
__inline BOOL MsoFUnescapePercentHexHex(const T *pz, WCHAR &wchOut) MSONOEXCEPT
{
	if (pz[0] == (T) '%')
		{
		UINT i0 = static_cast<UINT>(MsoValHexWch(pz[1]));
		if (i0 <= 0xF)
			{
			UINT i1 = static_cast<UINT>(MsoValHexWch(pz[2]));
			if (i1 <= 0xF)
				{
				wchOut = (WCHAR) ((i0 << 4) + i1);
				return TRUE;
				}
			}
		}

	return FALSE;
}

namespace Mso::Strings {

/*------------------------------------------------------------------------------
	Convert a character into %HH format
	Does not null-terminate
------------------------------------------------------------------------------*/
template<typename Tch>
__inline size_t HexEncode(char ch, _In_count_(cch) Tch* rgtch, _In_ size_t cch) MSONOEXCEPT
	{
	static const char* hexDigits = "0123456789ABCDEF";

	if (cch < 3)
		return 0;

	*rgtch++ = '%';
	*rgtch++ = hexDigits[(ch >> 4) & 0x0F];
	*rgtch++ = hexDigits[(ch >> 0) & 0x0F];
	return 3;
	}

template<unsigned int N>
struct HexEncodedSize
{
	enum : unsigned int { value = 2*N + 2 };
};

template<unsigned int N>
void HexEncodeBytes(const BYTE (&bytes)[N], WCHAR (&encodedBytes)[HexEncodedSize<N>::value]) MSONOEXCEPT
{
	::MsoHexFromBytes(encodedBytes, _countof(encodedBytes), bytes, sizeof(bytes));
}

} // Mso::Strings

namespace Mso::StringCore::Flighting
{

LIBLET_PUBLICAPI void FixMsoParseDoubleWzEx(bool fFix) noexcept;

} // Mso::StringCore::Flighting

#endif // __cplusplus

#endif // LIBLET_ROOT_MSOSTRINGPARSING_H
