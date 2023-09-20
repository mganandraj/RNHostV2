/**
	Copyright (c) 1994 Microsoft Corporation

	Non-locale aware functions related to comparing strings. 
	Used for non-UI features, XML compares, file format, etc.
	
	*** If you aren't sure, talk to AskIntl about which set to use ***

	[Mso::StringExact namespace]
	Does a case-sensitive (byte equivalence) compare, basically a memcmp.

	[Mso::StringAscii namespace]
	Performs an ASCII-based case-insensitive compare. In other words, 
	the letters 'a' - 'z' are treated as equal to 'A' to 'Z'. Typically useful 
	when one string is known to contain ASCII.

	[Mso::StringInvariant]
	Performs a locale-Invariant comparison using a fixed table to convert characters
	to upper case. It would be trivial to add more functions if needed. It would also 
	be useful to understand if the system APIs could be used instead.

	[Mso::StringIntl]
	Locale-aware string functions (Mso::StringIntlCompare.h)

	***

	Methods Examples:

	if (Mso::StringExact::Compare(wz1, wz2) < 0) { ... }
	if (Mso::StringAscii::Equal(sz1, sz2)) { ... }

	[Return Values]
		negative if A < B
		0 if A == B
		positive if B > A
*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGCOMPARISON_H
#define LIBLET_ROOT_MSOSTRINGCOMPARISON_H

#include <crash/verifyElseCrash.h>
#include <core_root/msostringutils.h>



//
//__inline size_t strlen_( const char * str )
//{
//    size_t len = -1;
//    const char * p = str - 1;
//
//    do
//        len++;
//    while (*++p != L'\0');
//
//    return len;
//}
//
//__inline size_t wcslen_( const wchar_t * str )
//{
//    size_t len = -1;
//    const wchar_t * p = str - 1;
//
//    do
//        len++;
//    while (*++p != L'\0');
//
//    return len;
//}

//__inline int MsoCchSzLen(_In_opt_z_ _Post_count_(return+1) const CHAR *sz)
//{
//
//    size_t cch = sz ? strlen_(sz) : 0;
//    // MsoAssertMsgInlineTag(cch < INT_MAX, "MsoCchSzLen - String too long", 0x63327871 /* tag_c2xq */);
//    return (int)cch;
//}
//
//__inline int MsoCchWzLen(_In_opt_z_ _Post_count_(return+1) const WCHAR *wz)
//{
//    size_t cch  = wz ? wcslen_(wz) : 0;
//    // MsoAssertMsgInlineTag(cch < INT_MAX, "MsoCchWzLen - String too long", 0x63327872 /* tag_c2xr */);
//    return (int)cch;
//}

#ifdef __cplusplus

#pragma warning(push)
#pragma warning(disable:4530) // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma push_macro("new")
#undef new
#include <string>
#pragma pop_macro("new")
#pragma warning(pop)

namespace Mso::StringCore { 

/*----------------------------------------------------------------------------
	Mso::StringCore::StartsWith

	Shared StartsWith implementation
----------------------------------------------------------------------------*/
template<typename Tch, typename CompareMode> 
__inline bool StartsWith(_In_opt_z_ const Tch *tz, _In_opt_z_ const Tch *tzCompare) noexcept
	{
	if (tz == nullptr || tzCompare == nullptr) { return false; }
	while (*tzCompare != '\0') 
		{
		if (CompareMode::ToUpperChar(*tz++) != CompareMode::ToUpperChar(*tzCompare++))
			return false;
		}
	return true;
	}

/*----------------------------------------------------------------------------
	Mso::StringCore::Compare

	Simple template to check for null before calling compare method
----------------------------------------------------------------------------*/
template<typename Tch, typename Pfn>
static int Compare(_In_opt_z_ const Tch* tz1, _In_opt_z_ const Tch* tz2, Pfn pfnCompare) noexcept
	{
	if (tz1 == tz2) { return 0; }
	if (tz1 == nullptr) { return -1; }
	if (tz2 == nullptr) { return 1; }
	return pfnCompare(tz1, tz2); 
	}

} // namespace Mso::StringCore


namespace Mso { 

/*----------------------------------------------------------------------------
	Mso::StringExact
----------------------------------------------------------------------------*/
struct StringExact { 

/*----------------------------------------------------------------------------
	Mso::StringExact::ToUpperChar

	Case-exact ToUpper implementation
----------------------------------------------------------------------------*/
static char ToUpperChar(char ch) noexcept { return ch; }
static wchar_t ToUpperChar(wchar_t wch) noexcept { return wch; }

/*----------------------------------------------------------------------------
	Mso::StringExact::Compare
	Mso::StringExact::Equal

	Case exact comparison for two strings
----------------------------------------------------------------------------*/
static int Compare(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept { return Mso::StringCore::Compare<char>(sz1, sz2, strcmp); }
static int Compare(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept { return Mso::StringCore::Compare<wchar_t>(wz1, wz2, wcscmp); }
static bool Equal(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept { return (Mso::StringExact::Compare(sz1, sz2) == 0); }
static bool Equal(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept { return (Mso::StringExact::Compare(wz1, wz2) == 0); }

/*----------------------------------------------------------------------------
	Mso::StringExact::Compare
	Mso::StringExact::Equal

	Case exact comparison for two character buffers
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const char* rgch1, int cch1, _In_opt_count_(cch2) const char* rgch2, int cch2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const wchar_t* rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t* rgwch2, int cch2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const char* rgch, UINT cch1, _In_opt_count_(cch2) const wchar_t* rgwch, UINT cch2) noexcept;

static bool Equal(_In_count_(cch1) const char* rgch1, int cch1, _In_count_(cch2) const char* rgch2, int cch2) noexcept
{
	VerifyElseCrashSzTag(cch1 >= 0, "Negative character count?", 0x1f5c2190 /* tag_5xcgq */);
	VerifyElseCrashSzTag(cch2 >= 0, "Negative character count?", 0x1f5c218f /* tag_5xcgp */);

	if (cch1 != cch2)
	{
		return false;
	}
	return (memcmp(rgch1, rgch2, static_cast<size_t>(cch1)) == 0);
}

static bool Equal(_In_count_(cch1) const wchar_t* rgwch1, int cch1, _In_count_(cch2) const wchar_t* rgwch2, int cch2) noexcept
{
	VerifyElseCrashSzTag(cch1 >= 0, "Negative character count?", 0x1f5c218e /* tag_5xcgo */);
	VerifyElseCrashSzTag(cch2 >= 0, "Negative character count?", 0x1f5c218d /* tag_5xcgn */);

	if (cch1 != cch2)
	{
		return false;
	}
	return (memcmp(rgwch1, rgwch2, cch1 * sizeof(wchar_t)) == 0);
}

static bool Equal(_In_count_(cch1) const char* rgch, UINT cch1, _In_count_(cch2) const wchar_t* rgwch, UINT cch2) noexcept
	{
	if (cch1 != cch2) { return false; }
	return (Mso::StringExact::Compare(rgch, cch1, rgwch, cch2) == 0);
	}

/*----------------------------------------------------------------------------
	Case exact comparison for string_views
----------------------------------------------------------------------------*/
static constexpr int32_t Compare(const std::string_view& left, const std::string_view& right) noexcept
{
	return left.compare(right);
}

static constexpr int32_t Compare(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return left.compare(right);
}

static int32_t Compare(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Compare(left.data(), static_cast<uint32_t>(left.size()), right.data(), static_cast<uint32_t>(right.size()));
}

static int32_t Compare(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return -Compare(right, left);
}

static constexpr bool Equal(const std::string_view& left, const std::string_view& right) noexcept
{
	return left == right;
}

static constexpr bool Equal(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return left == right;
}

static bool Equal(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.data(), static_cast<uint32_t>(left.size()), right.data(), static_cast<uint32_t>(right.size()));
}

static bool Equal(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return Equal(right, left);
}

/*-----------------------------------------------------------------------------
	Mso::StringExact::SgnCompareWz

	Case-exact comparision function for MSOTPX
	Parameters are really: const wchar_t **.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int SgnCompareWz(const void *pv1, const void *pv2) noexcept;

/*----------------------------------------------------------------------------
	Mso::StringExact::StartsWith

	Case exact String A starts with String B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_z_ const char* sz, _In_opt_z_ const char* szCompare) noexcept { return Mso::StringCore::StartsWith<char, Mso::StringExact>(sz, szCompare); }
static bool StartsWith(_In_opt_z_ const wchar_t* wz, _In_opt_z_ const wchar_t* wzCompare) noexcept { return Mso::StringCore::StartsWith<wchar_t, Mso::StringExact>(wz, wzCompare); }

/*----------------------------------------------------------------------------
	Mso::StringExact::StartsWith

	Case exact character buffer A starts with character buffer B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_count_(cch) const wchar_t* rgwch, int cch, _In_opt_count_(cchCompare) const wchar_t* rgwchCompare, int cchCompare) noexcept
{
	VerifyElseCrashSzTag(cch >= 0, "Negative character count?", 0x1f5c218c /* tag_5xcgm */);
	VerifyElseCrashSzTag(cchCompare >= 0, "Negative character count?", 0x1f5c218b /* tag_5xcgl */);

	if (rgwch == nullptr || rgwchCompare == nullptr)
	{
		return false;
	}
	return (cchCompare <= cch) && Mso::StringExact::Equal(rgwch, cchCompare, rgwchCompare, cchCompare);
}

static bool StartsWith(const std::string_view& left, const std::string_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

/*----------------------------------------------------------------------------
	Mso::StringExact::Find

	Case exact character search for a given character buffer
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static const    char* Find(_In_count_(cch) const    char*  rgch, int cch, _In_count_(cchFind) const    char*  rgchFind, int cchFind) noexcept;
LIBLET_PUBLICAPI static const wchar_t* Find(_In_count_(cch) const wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept;

static const    char* Find(_In_z_ const    char* sz, _In_z_ const    char* szFind) noexcept { return Mso::StringExact::Find(sz, MsoCchSzLen(sz), szFind, MsoCchSzLen(szFind)); }
static const wchar_t* Find(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringExact::Find(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }

static const    char* Find(_In_count_(cch) const    char*  rgch, int cch, _In_z_ const    char* szFind) noexcept { return Mso::StringExact::Find( rgch, cch, szFind, MsoCchSzLen(szFind)); }
static const wchar_t* Find(_In_count_(cch) const wchar_t* rgwch, int cch, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringExact::Find(rgwch, cch, wzFind, MsoCchWzLen(wzFind)); }

/*----------------------------------------------------------------------------
	Mso::StringExact::EndsWith

	Case exact String A ends with String B
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static const wchar_t* EndsWith(_In_count_(cch) const wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept;
static const wchar_t* EndsWith(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringExact::EndsWith(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }
static wchar_t* EndsWith(_In_count_(cch) wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept 
	{
	OACR_USE_PTR(rgwch);
	return (wchar_t*)Mso::StringExact::EndsWith((const wchar_t*)rgwch, cch, rgwchFind, cchFind);
	}
static wchar_t* EndsWith(_In_z_ wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringExact::EndsWith(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }

static bool EndsWith(const std::string_view& left, const std::string_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

/*----------------------------------------------------------------------------
	Case exact character search for a given character buffer
	Returns -1 if not found.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int32_t IndexOf(_In_count_(cch) const char* rgch, size_t cch, _In_count_(cchFind) const char* rgchFind, size_t cchFind) noexcept;
LIBLET_PUBLICAPI static int32_t IndexOf(_In_count_(cch) const wchar_t* rgwch, size_t cch, _In_count_(cchFind) const wchar_t* rgwchFind, size_t cchFind) noexcept;

static int32_t IndexOf(_In_z_ const char* sz, _In_z_ const char* szFind) noexcept
{ return IndexOf(sz, strlen(sz), szFind, strlen(szFind)); }
static int32_t IndexOf(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept
{ return IndexOf(wz, wcslen(wz), wzFind, wcslen(wzFind)); }

static int32_t IndexOf(_In_z_ const char* sz, char chFind) noexcept
{ return IndexOf(sz, strlen(sz), &chFind, 1); }
static int32_t IndexOf(_In_z_ const wchar_t* wz, wchar_t wchFind) noexcept
{ return IndexOf(wz, wcslen(wz), &wchFind, 1); }

/*----------------------------------------------------------------------------
	Case exact: replaces all instances of *From with *To.
	Returns false if not enough room (partial progress may have been written)
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_count_(cchFrom) const char* rgchFrom, size_t cchFrom, _In_count_(cchTo) const char* rgchTo, size_t cchTo) noexcept;
LIBLET_PUBLICAPI static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_count_(cchFrom) const wchar_t* rgwchFrom, size_t cchFrom, _In_count_(cchTo) const wchar_t* rgwchTo, size_t cchTo) noexcept;

static bool TryReplaceAll(_Inout_z_cap_(cchMax) char* sz, size_t cchMax, _In_z_ const char* szFrom, _In_z_ const char* szTo) noexcept
{ return TryReplaceAll(sz, strlen(sz), cchMax, szFrom, strlen(szFrom), szTo, strlen(szTo)); }
static bool TryReplaceAll(_Inout_z_cap_(cchMax) wchar_t* wz, size_t cchMax, _In_z_ const wchar_t* wzFrom, _In_z_ const wchar_t* wzTo) noexcept
{ return TryReplaceAll(wz, wcslen(wz), cchMax, wzFrom, wcslen(wzFrom), wzTo, wcslen(wzTo)); }

/*
static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_z_ const char* szFrom, _In_z_ const char* szTo) noexcept
{ return TryReplaceAll(sz, cch, cchMax, szFrom, strlen(szFrom), szTo, strlen(szTo)); }
static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_z_ const wchar_t* wzFrom, _In_z_ const wchar_t* wzTo) noexcept
{ return TryReplaceAll(wz, cch, cchMax, wzFrom, wcslen(wzFrom), wzTo, wcslen(wzTo)); }
*/

private:
	MSO_NO_COPYCONSTR(StringExact);
	MSO_NO_ASSIGNMENT(StringExact);
	MSO_NO_DEFAULTCONSTR(StringExact);
}; // struct StringExact







/*----------------------------------------------------------------------------
	Ascii-comparison functions
----------------------------------------------------------------------------*/
struct StringAscii 
{

/*----------------------------------------------------------------------------
	Mso::StringAscii::ToUpperChar

	ASCII ToUpper implementation
----------------------------------------------------------------------------*/
static char ToUpperChar(char ch) noexcept { return (ch >= 'a' && ch <= 'z') ? static_cast<char>(ch - ('a' - 'A')) : ch; }
static wchar_t ToUpperChar(wchar_t wch) noexcept { return (wch >= 'a' && wch <= 'z') ? static_cast<wchar_t>(wch - ('a' - 'A')) : wch; }

LIBLET_PUBLICAPI static int Compare(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept;

LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const char* rgch1, int cch1, _In_opt_count_(cch2) const char* rgch2, int cch2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const wchar_t* rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t* rgwch2, int cch2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const char* rgch, UINT cch1, _In_opt_count_(cch2) const wchar_t* rgwch, UINT cch2) noexcept;

static int32_t Compare(const std::string_view& left, const std::string_view& right) noexcept
{
	return Compare(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

static int32_t Compare(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Compare(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

static int32_t Compare(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Compare(left.data(), static_cast<uint32_t>(left.size()), right.data(), static_cast<uint32_t>(right.size()));
}

static int32_t Compare(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return -Compare(right, left);
}

static bool Equal(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept { return (Mso::StringAscii::Compare(sz1, sz2) == 0); }
static bool Equal(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept { return (Mso::StringAscii::Compare(wz1, wz2) == 0); }

static bool Equal(_In_count_(cch1) const char* rgch1, int cch1, _In_count_(cch2) const char* rgch2, int cch2) noexcept
{
	VerifyElseCrashSzTag(cch1 >= 0, "Negative character count?", 0x1f5c218a /* tag_5xcgk */);
	VerifyElseCrashSzTag(cch2 >= 0, "Negative character count?", 0x1f5c2189 /* tag_5xcgj */);

	if (cch1 != cch2)
	{
		return false;
	}
	return (Mso::StringAscii::Compare(rgch1, cch1, rgch2, cch2) == 0);
}

static bool Equal(_In_count_(cch1) const wchar_t* rgwch1, int cch1, _In_count_(cch2) const wchar_t* rgwch2, int cch2) noexcept
{
	VerifyElseCrashSzTag(cch1 >= 0, "Negative character count?", 0x1f5c2188 /* tag_5xcgi */);
	VerifyElseCrashSzTag(cch2 >= 0, "Negative character count?", 0x1f5c2187 /* tag_5xcgh */);

	if (cch1 != cch2)
	{
		return false;
	}
	return (Mso::StringAscii::Compare(rgwch1, cch1, rgwch2, cch2) == 0);
}

static bool Equal(_In_count_(cch1) const char* rgch, UINT cch1, _In_count_(cch2) const wchar_t* rgwch, UINT cch2) noexcept
{
	if (cch1 != cch2)
	{
		return false;
	}
	return (Mso::StringAscii::Compare(rgch, cch1, rgwch, cch2) == 0);
}

static bool Equal(const std::string_view& left, const std::string_view& right) noexcept
{
	return Equal(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

static bool Equal(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

static bool Equal(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.data(), static_cast<uint32_t>(left.size()), right.data(), static_cast<uint32_t>(right.size()));
}

static bool Equal(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return Equal(right, left);
}

/*-----------------------------------------------------------------------------
	Mso::StringAscii::SgnCompareWz

	Case-insensitive ASCII comparision function for MSOTPX
	Parameters are const wchar_t **.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int SgnCompareWz(const void *pv1, const void *pv2) noexcept;

/*----------------------------------------------------------------------------
	Mso::StringAscii::StartsWith
	
	Case-insensitive ASCII compare for whether String A starts with String B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_z_ const char* sz, _In_opt_z_ const char* szCompare) noexcept { return Mso::StringCore::StartsWith<char, Mso::StringAscii>(sz, szCompare); }
static bool StartsWith(_In_opt_z_ const wchar_t* wz, _In_opt_z_ const wchar_t* wzCompare) noexcept { return Mso::StringCore::StartsWith<wchar_t, Mso::StringAscii>(wz, wzCompare); }

/*----------------------------------------------------------------------------
	Mso::StringAscii::StartsWith

	Case-insensitive ASCII compare if buffer A starts with character buffer B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_count_(cch) const wchar_t* rgwch, int cch, _In_opt_count_(cchCompare) const wchar_t* rgwchCompare, int cchCompare) noexcept
{
	VerifyElseCrashSzTag(cch >= 0, "Negative character count?", 0x1f5c2186 /* tag_5xcgg */);
	VerifyElseCrashSzTag(cchCompare >= 0, "Negative character count?", 0x1f5c2185 /* tag_5xcgf */);

	if (rgwch == nullptr || rgwchCompare == nullptr)
	{
		return false;
	}
	return (cchCompare <= cch) && Mso::StringAscii::Equal(rgwch, cchCompare, rgwchCompare, cchCompare);
}

static bool StartsWith(const std::string_view& left, const std::string_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

static bool StartsWith(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

/*----------------------------------------------------------------------------
	Mso::StringAscii::Find

	Case-insensitive ASCII search for a given character buffer
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static const    char* Find(_In_count_(cch) const    char*  rgch, int cch, _In_count_(cchFind) const    char*  rgchFind, int cchFind) noexcept;
LIBLET_PUBLICAPI static const wchar_t* Find(_In_count_(cch) const wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept;

static const    char* Find(_In_z_ const    char* sz, _In_z_ const    char* szFind) noexcept { return Mso::StringAscii::Find(sz, MsoCchSzLen(sz), szFind, MsoCchSzLen(szFind)); }
static const wchar_t* Find(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringAscii::Find(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }

static const    char* Find(_In_count_(cch) const    char*  rgch, int cch, _In_z_ const    char* szFind) noexcept { return Mso::StringAscii::Find( rgch, cch, szFind, MsoCchSzLen(szFind)); }
static const wchar_t* Find(_In_count_(cch) const wchar_t* rgwch, int cch, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringAscii::Find(rgwch, cch, wzFind, MsoCchWzLen(wzFind)); }

/*----------------------------------------------------------------------------
	Mso::StringAscii::EndsWith

	Case-insensitive ASCII compare if buffer A starts with character buffer B
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static const wchar_t* EndsWith(_In_count_(cch) const wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept;
static const wchar_t* EndsWith(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringAscii::EndsWith(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }
static wchar_t* EndsWith(_In_count_(cch) wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept 
	{
	OACR_USE_PTR(rgwch);
	return (wchar_t*)Mso::StringAscii::EndsWith((const wchar_t*)rgwch, cch, rgwchFind, cchFind);
	}
static wchar_t* EndsWith(_In_z_ wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept { return Mso::StringAscii::EndsWith(wz, MsoCchWzLen(wz), wzFind, MsoCchWzLen(wzFind)); }

static bool EndsWith(const std::string_view& left, const std::string_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::string_view& left, const std::wstring_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::wstring_view& left, const std::string_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

static bool EndsWith(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return left.size() >= right.size() && Equal(left.substr(left.size() - right.size(), right.size()), right);
}

/*----------------------------------------------------------------------------
	Ascii-insensitive: character search for a given character buffer
	Returns -1 if not found.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int32_t IndexOf(_In_count_(cch) const char* rgch, size_t cch, _In_count_(cchFind) const char* rgchFind, size_t cchFind) noexcept;
LIBLET_PUBLICAPI static int32_t IndexOf(_In_count_(cch) const wchar_t* rgwch, size_t cch, _In_count_(cchFind) const wchar_t* rgwchFind, size_t cchFind) noexcept;

static int32_t IndexOf(_In_z_ const char* sz, _In_z_ const char* szFind) noexcept
{ return IndexOf(sz, strlen(sz), szFind, strlen(szFind)); }
static int32_t IndexOf(_In_z_ const wchar_t* wz, _In_z_ const wchar_t* wzFind) noexcept
{ return IndexOf(wz, wcslen(wz), wzFind, wcslen(wzFind)); }

static int32_t IndexOf(_In_z_ const char* sz, char chFind) noexcept
{ return IndexOf(sz, strlen(sz), &chFind, 1); }
static int32_t IndexOf(_In_z_ const wchar_t* wz, wchar_t wchFind) noexcept
{ return IndexOf(wz, wcslen(wz), &wchFind, 1); }

/*----------------------------------------------------------------------------
	Ascii-insensitive: replaces all instances of *From with *To.
	Returns false if not enough room (partial progress may have been written)
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_count_(cchFrom) const char* rgchFrom, size_t cchFrom, _In_count_(cchTo) const char* rgchTo, size_t cchTo) noexcept;
LIBLET_PUBLICAPI static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_count_(cchFrom) const wchar_t* rgwchFrom, size_t cchFrom, _In_count_(cchTo) const wchar_t* rgwchTo, size_t cchTo) noexcept;

static bool TryReplaceAll(_Inout_z_cap_(cchMax) char* sz, size_t cchMax, _In_z_ const char* szFrom, _In_z_ const char* szTo) noexcept
{ return TryReplaceAll(sz, strlen(sz), cchMax, szFrom, strlen(szFrom), szTo, strlen(szTo)); }
static bool TryReplaceAll(_Inout_z_cap_(cchMax) wchar_t* wz, size_t cchMax, _In_z_ const wchar_t* wzFrom, _In_z_ const wchar_t* wzTo) noexcept
{ return TryReplaceAll(wz, wcslen(wz), cchMax, wzFrom, wcslen(wzFrom), wzTo, wcslen(wzTo)); }

/*
static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_z_ const char* szFrom, _In_z_ const char* szTo) noexcept
{ return TryReplaceAll(sz, cch, cchMax, szFrom, strlen(szFrom), szTo, strlen(szTo)); }
static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_z_ const wchar_t* wzFrom, _In_z_ const wchar_t* wzTo) noexcept
{ return TryReplaceAll(wz, cch, cchMax, wzFrom, wcslen(wzFrom), wzTo, wcslen(wzTo)); }
*/

private:
	MSO_NO_COPYCONSTR(StringAscii);
	MSO_NO_ASSIGNMENT(StringAscii);
	MSO_NO_DEFAULTCONSTR(StringAscii);
}; // struct Mso::StringAscii




/*----------------------------------------------------------------------------
	Mso::StringInvariant
----------------------------------------------------------------------------*/
struct StringInvariant {

/*----------------------------------------------------------------------------
	Mso::StringInvariant::ToUpperChar

	Locale-Invariant ToUpper implementation
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static wchar_t ToUpperChar(wchar_t wch) noexcept;
static char ToUpperChar(char ch) noexcept { return static_cast<char>(Mso::StringInvariant::ToUpperChar(static_cast<wchar_t>(ch))); }

/*----------------------------------------------------------------------------
	Mso::StringInvariant::Compare
	Mso::StringInvariant::Equal

	Performs the case-insensitive comparison using MsoToUpperInvariant.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI static int Compare(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept;
LIBLET_PUBLICAPI static int Compare(_In_opt_count_(cch1) const wchar_t *rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t *rgwch2, int cch2) noexcept; 

static int32_t Compare(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Compare(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

static bool Equal(_In_z_ const char* sz1, _In_z_ const char* sz2) noexcept { return (Mso::StringInvariant::Compare(sz1, sz2) == 0); }
static bool Equal(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2) noexcept { return (Mso::StringInvariant::Compare(wz1, wz2) == 0); }
static bool Equal(_In_opt_count_(cch1) const wchar_t *rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t *rgwch2, int cch2) noexcept { return (Mso::StringInvariant::Compare(rgwch1, cch1, rgwch2, cch2) == 0); } 

static bool Equal(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.data(), static_cast<int>(left.size()), right.data(), static_cast<int>(right.size()));
}

/*----------------------------------------------------------------------------
	Mso::StringInvariant::StartsWith

	Case insensitive String A starts with String B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_z_ const char* sz, _In_opt_z_ const char* szCompare) noexcept { return Mso::StringCore::StartsWith<char, Mso::StringInvariant>(sz, szCompare); }
static bool StartsWith(_In_opt_z_ const wchar_t* wz, _In_opt_z_ const wchar_t* wzCompare) noexcept { return Mso::StringCore::StartsWith<wchar_t, Mso::StringInvariant>(wz, wzCompare); }

/*----------------------------------------------------------------------------
	Mso::StringInvariant::StartsWith

	Case insensitive character buffer A starts with character buffer B
----------------------------------------------------------------------------*/
static bool StartsWith(_In_opt_count_(cch) const wchar_t* rgwch, int cch, _In_opt_count_(cchCompare) const wchar_t* rgwchCompare, int cchCompare) noexcept
{
	VerifyElseCrashSzTag(cch >= 0, "Negative character count?", 0x1f5c2184 /* tag_5xcge */);
	VerifyElseCrashSzTag(cchCompare >= 0, "Negative character count?", 0x1f5c2183 /* tag_5xcgd */);

	if (rgwch == nullptr || rgwchCompare == nullptr)
	{
		return false;
	}
	return (cchCompare <= cch) && Mso::StringInvariant::Compare(rgwch, cchCompare, rgwchCompare, cchCompare) == 0;
}

static bool StartsWith(const std::wstring_view& left, const std::wstring_view& right) noexcept
{
	return Equal(left.substr(0, right.size()), right);
}

private:
	MSO_NO_COPYCONSTR(StringInvariant);
	MSO_NO_ASSIGNMENT(StringInvariant);
	MSO_NO_DEFAULTCONSTR(StringInvariant);
}; // struct Mso::StringInvariant



} // namespace Mso









#ifndef MSOSTR_DISABLE_LEGACY
/*----------------------------------------------------------------------------

	These should all go away

----------------------------------------------------------------------------*/

__inline int MsoSgnRgchCompareSimple(_In_count_(cch1) const CHAR* rgch1, int cch1, _In_count_(cch2) const CHAR* rgch2, int cch2, int cs) noexcept
	{
	if (cs == 1) // msocsIgnoreCase
		return Mso::StringAscii::Compare(rgch1, cch1, rgch2, cch2); 
	return Mso::StringExact::Compare(rgch1, cch1, rgch2, cch2);
	}

__inline int MsoSgnRgwchCompareSimple(_In_count_(cch1) const wchar_t *rgwch1, UINT cch1, _In_count_(cch2) const wchar_t *rgwch2, UINT cch2, BOOL fIgnoreCase) noexcept
	{
	if (fIgnoreCase)
		return Mso::StringAscii::Compare(rgwch1, static_cast<int>(cch1), rgwch2, static_cast<int>(cch2)); 
	return Mso::StringExact::Compare(rgwch1, static_cast<int>(cch1), rgwch2, static_cast<int>(cch2));
	}

__inline int MsoSgnWzCompareSimple(const wchar_t *wz1, const wchar_t *wz2, BOOL fIgnoreCase) noexcept
	{
	if (fIgnoreCase)
		return Mso::StringAscii::Compare(wz1, wz2); 
	return Mso::StringExact::Compare(wz1, wz2);
	}

__inline BOOL MsoFRgchEqualSimple(_In_count_(cch1) const CHAR* rgch1, int cch1, _In_count_(cch2) const CHAR* rgch2, int cch2, int cs) noexcept
{
	if (cs == 1) // msocsIgnoreCase
		return Mso::StringAscii::Equal(rgch1, cch1, rgch2, cch2); 
	return Mso::StringExact::Equal(rgch1, cch1, rgch2, cch2);
}

__inline BOOL MsoFRgchRgwchEqualSimple(_In_count_(cch1) const CHAR *rgch, UINT cch1, _In_count_(cch2) const wchar_t *rgwch, UINT cch2, BOOL fIgnoreCase) noexcept
{
	if (fIgnoreCase)
		return Mso::StringAscii::Equal(rgch, cch1, rgwch, cch2); 
	return Mso::StringExact::Equal(rgch, cch1, rgwch, cch2);
}

__inline BOOL MsoFRgwchEqualSimple(_In_count_(cch1) const wchar_t *rgwch1, UINT cch1, _In_count_(cch2) const wchar_t *rgwch2, UINT cch2, BOOL fIgnoreCase) noexcept
{
	if (fIgnoreCase)
		return Mso::StringAscii::Equal(rgwch1, static_cast<int>(cch1), rgwch2, static_cast<int>(cch2)); 
	return Mso::StringExact::Equal(rgwch1, static_cast<int>(cch1), rgwch2, static_cast<int>(cch2));
}

__inline BOOL MsoFWzEqualSimple(const wchar_t *wz1, const wchar_t *wz2, BOOL fIgnoreCase) noexcept
{
	if (fIgnoreCase)
		return Mso::StringAscii::Equal(wz1, wz2); 
	return Mso::StringExact::Equal(wz1, wz2);
}

__inline int MsoICompRgchCchInvariant(_In_opt_count_(cch) const char *rgch1, _In_opt_count_(cch) const char *rgch2, int cch) noexcept
	{
	return Mso::StringAscii::Compare(rgch1, cch, rgch2, cch);
	}

__inline int MsoICompRgwchCchInvariant(_In_opt_count_(cch) const wchar_t *rgwch1, _In_opt_count_(cch) const wchar_t *rgwch2, int cch) noexcept
	{
	return Mso::StringAscii::Compare(rgwch1, cch, rgwch2, cch);
	}

__inline int MsoICompWzInvariant(_In_z_ const wchar_t *wz1, _In_z_ const wchar_t *wz2) noexcept
	{
	return Mso::StringExact::Compare(wz1, wz2);
	}

__inline bool MsoFRgwchEqualExact(const WCHAR *rgwch1, int cch1, const WCHAR *rgwch2, int cch2) noexcept 
	{ 
	return Mso::StringExact::Equal(rgwch1, cch1, rgwch2, cch2); 
	}

__inline bool MsoFSzIEqualFast(const char* sz1, const char* sz2) noexcept
	{
	return Mso::StringAscii::Equal(sz1, sz2);
	}

/*----------------------------------------------------------------------------
	MsoSgnRgwchCompareInvariant

	Performs the case-insensitive comparison using MsoToUpperInvariant.
----------------------------------------------------------------------------*/
__inline int MsoSgnRgwchCompareInvariant(_In_opt_count_(cch1) const wchar_t *rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t *rgwch2, int cch2, int cs) noexcept
{
	VerifyElseCrashSzTag(cs == 1 || cs == 4, "Unsupported flags", 0x1f5c2182 /* tag_5xcgc */);

	if (cs == 4) // msocsExact
		return Mso::StringExact::Compare(rgwch1, cch1, rgwch2, cch2);
	return Mso::StringInvariant::Compare(rgwch1, cch1, rgwch2, cch2);
}

#endif // !MSOSTR_DISABLE_LEGACY


#endif // __cplusplus

// For now - include the Intl header
// #include "MsoStringIntlCompare.h"

#endif // LIBLET_ROOT_MSOSTRINGCOMPARISON_H
