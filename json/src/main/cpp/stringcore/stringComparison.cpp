/**
	Copyright (c) 1994 Microsoft Corporation

	Functions related to comparing strings
	- simple / invariant string compares
*/
#include "precomp.h"
#include <stringcore/core/msoStringTraits.h>
#include <stringcore/core/stringcomparison.h>
#include <stringcore/core/stringconversion.h>
#include "stringcore/core/stringTraits.h"
#include <core_root/msostringUtils.h>

/**
	*::Compare
	*::Compare
	*::Compare

	Shared compare function that works with two character ranges
*/
template<typename Tch1, typename Tch2, typename CompareMode>
int CompareRgtch(_In_opt_count_(cch1) const Tch1* rgtch1, int cch1, _In_opt_count_(cch2) const Tch2* rgtch2, int cch2) noexcept
	{
	static_assert(sizeof(Tch1) <= sizeof(Tch2), "Assumes same type or Tch1 < Tch2");

	// FUTURE: it would be nice to ShipAssertSz(false, "Not reached", ) on nullptrs
	if (reinterpret_cast<const void *>(rgtch1) == reinterpret_cast<const void *>(rgtch2)) { return 0; }
	if (rgtch1 == nullptr) { return -1; }
	if (rgtch2 == nullptr) { return 1; }

	int cchCommon = __min(cch1, cch2);
	while (cchCommon > 0)
		{
		auto tch1 = static_cast<typename std::make_unsigned<Tch1>::type>(CompareMode::ToUpperChar(*rgtch1));
		auto tch2 = static_cast<typename std::make_unsigned<Tch2>::type>(CompareMode::ToUpperChar(*rgtch2));
		if (tch1 < tch2)
			return -1;
		if (tch1 > tch2)
			return 1;

		++rgtch1;
		++rgtch2;
		--cchCommon;
		}

	if (cch1 < cch2)
		return -1;
	if (cch1 > cch2)
		return 1;
	return 0;
	}

/**
	*::Compare
	*::Compare

	Shared function for comparing two strings.
*/
template<typename Tch, typename CompareMode>
int CompareTz(_In_z_ const Tch* tz1, _In_z_ const Tch* tz2) noexcept
	{
	if (tz1 == tz2)
		{
		// FUTURE: it would be nice to ShipAssertSz(false, "Not reached", ) here
		return 0;
		}

	if (tz1 == nullptr || tz2 == nullptr) 
		{ 
		// FUTURE: it would be nice to ShipAssertSz(false, "Not reached", ) here
		return (tz1 == nullptr) ? -1 : 1;
		}

	while (*tz1)
		{
		Tch tch1 = CompareMode::ToUpperChar(*tz1);
		Tch tch2 = CompareMode::ToUpperChar(*tz2);
		if (tch1 != tch2)
			return (tch1 > tch2 ? 1 : -1);

		++tz1;
		++tz2;
		}

	// If tz2 was shorter, we returned above (tch1 != tch2, where tch2 == 0)
	// If tz2 is same length, return zero / equal
	// If tz2 is longer, return -1
	return (*tz2) ? -1 : 0;
	}

template<typename Tch, typename CompareMode>
static const Tch* EndsWithRgtch(_In_count_(cch) const wchar_t* rgwch, int cch, _In_count_(cchFind) const wchar_t* rgwchFind, int cchFind) noexcept
{
	if(cch >= cchFind && CompareMode::Equal(rgwch + (cch - cchFind), cchFind, rgwchFind, cchFind))
		return rgwch + (cch - cchFind);
	return nullptr;
}

namespace Mso {
namespace StringCore {
namespace Details {

template<typename Tch, typename CompareMode>
static int32_t IndexOf(_In_count_(cch) const Tch* rgtch, size_t cch, _In_count_(cchFind) const Tch* rgtchFind, size_t cchFind) noexcept
{
	VerifyElseCrashTag(rgtch, 0x015d005a /* tag_bxqb0 */);
	VerifyElseCrashTag(rgtchFind, 0x015d005b /* tag_bxqb1 */);

	for (size_t ich = 0; ich + cchFind <= cch; ich++)
	{
		if (CompareMode::Equal(rgtch + ich, static_cast<int>(cchFind), rgtchFind, static_cast<int>(cchFind)))
			return static_cast<int>(ich);
	}

	return -1;
}

template<typename Tch, typename CompareMode>
static const Tch* Find(_In_count_(cch) const Tch* rgtch, int cch, _In_count_(cchFind) const Tch* rgtchFind, int cchFind) noexcept
{
	// FUTURE: should VEC on null pointers
	VerifyElseCrashTag(cch >= 0, 0x015d005c /* tag_bxqb2 */);
	VerifyElseCrashTag(cchFind >= 0, 0x015d005d /* tag_bxqb3 */);
	if (!rgtch || !rgtchFind)
		return nullptr;
	
	auto index = IndexOf<Tch, CompareMode>(rgtch, cch, rgtchFind, cchFind);
	return (index >= 0) ? rgtch + index : nullptr;
}

		int memmove_s(
		_Out_writes_bytes_to_opt_(_DestinationSize, _SourceSize) void*       const _Destination,
		_In_                                                     rsize_t     const _DestinationSize,
		_In_reads_bytes_opt_(_SourceSize)                        void const* const _Source,
		_In_                                                     rsize_t     const _SourceSize
)
{
	if (_SourceSize == 0)
	{
		return 0;
	}


#pragma warning(suppress:4996) // Deprecation
		memmove(_Destination, _Source, _SourceSize);
		return 0;
	}

template<typename Tch, typename CompareMode>
static bool TryReplaceAll(_Inout_z_count_(cch) _Inout_z_cap_(cchMax) Tch* tz, size_t cch, size_t cchMax, _In_count_(cchFrom) const Tch* rgtchFrom, size_t cchFrom, _In_count_(cchTo) const Tch* rgtchTo, size_t cchTo) noexcept
{
	VerifyElseCrashTag(tz, 0x015d005e /* tag_bxqb4 */);
	VerifyElseCrashTag(cch + 1 <= cchMax, 0x015d005f /* tag_bxqb5 */);
	VerifyElseCrashTag(rgtchFrom, 0x015d0060 /* tag_bxqb6 */);
	VerifyElseCrashTag(rgtchTo, 0x015d0061 /* tag_bxqb7 */);

	if (cchFrom == 0)
		return true;

	const Tch* tzEnd = tz + cchMax;
	for (;;)
	{
		auto matchIndex = IndexOf<Tch, CompareMode>(tz, static_cast<size_t>(tzEnd - tz), rgtchFrom, static_cast<size_t>(cchFrom));
		if (matchIndex == -1)
			break;

		tz += matchIndex;
		cch -= matchIndex;

		Tch* tzAfterReplace = tz + cchTo;
		const size_t cchAfterReplace = (cch - cchFrom);
		if (tzAfterReplace + cchAfterReplace >= tzEnd)
			return false;

		memmove_s(tzAfterReplace, static_cast<size_t>(tzEnd - tzAfterReplace) * sizeof(Tch), tz + cchFrom, static_cast<size_t>(cchAfterReplace + 1) * sizeof(Tch));
		memmove_s(tz, cchTo * sizeof(Tch), rgtchTo, cchTo * sizeof(Tch));
		tz = tzAfterReplace;
		cch = cchAfterReplace;
	}
	
	return true;
}

} // Details
} // StringCore
} // Mso

/**
	Mso::StringExact::Compare
	Mso::StringExact::Compare
	Mso::StringExact::Compare

	Case-exact compare functions that work with two character buffers.
*/
int Mso::StringExact::Compare(_In_count_(cch1) const char* rgch1, int cch1, _In_count_(cch2) const char* rgch2, int cch2)  noexcept
	{ return CompareRgtch<char, char, Mso::StringExact>(rgch1, cch1, rgch2, cch2); }
int Mso::StringExact::Compare(_In_count_(cch1) const wchar_t* rgwch1, int cch1, _In_count_(cch2) const wchar_t* rgwch2, int cch2)  noexcept
	{ return CompareRgtch<wchar_t, wchar_t, Mso::StringExact>(rgwch1, cch1, rgwch2, cch2); }
int Mso::StringExact::Compare(_In_count_(cch1) const char* rgch, UINT cch1, _In_count_(cch2) const wchar_t* rgwch, UINT cch2)  noexcept
    { return CompareRgtch<char, wchar_t, Mso::StringExact>(rgch, static_cast<int>(cch1), rgwch, static_cast<int>(cch2)); }

/**
	Mso::StringExact::EndsWith
*/
const wchar_t* Mso::StringExact::EndsWith(const wchar_t* rgwch, int cch, const wchar_t* rgwchFind, int cchFind) noexcept
	{ return EndsWithRgtch<wchar_t, Mso::StringExact>(rgwch, cch, rgwchFind, cchFind); }

int32_t Mso::StringExact::IndexOf(const char* rgch, size_t cch, const char* rgchFind, size_t cchFind) noexcept
{ return Mso::StringCore::Details::IndexOf<char, Mso::StringExact>(rgch, cch, rgchFind, cchFind); }
int32_t Mso::StringExact::IndexOf(const wchar_t* rgwch, size_t cch, const wchar_t* rgwchFind, size_t cchFind) noexcept
{ return Mso::StringCore::Details::IndexOf<wchar_t, Mso::StringExact>(rgwch, cch, rgwchFind, cchFind); }

const char* Mso::StringExact::Find(const char* rgch, int cch, const char* rgchFind, int cchFind) noexcept
{ return Mso::StringCore::Details::Find<char, Mso::StringExact>(rgch, cch, rgchFind, cchFind); }
const wchar_t* Mso::StringExact::Find(const wchar_t* rgwch, int cch, const wchar_t* rgwchFind, int cchFind) noexcept
{ return Mso::StringCore::Details::Find<wchar_t, Mso::StringExact>(rgwch, cch, rgwchFind, cchFind); }

bool Mso::StringExact::TryReplaceAll(_In_z_count_(cch) _Out_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_count_(cchFrom) const char* rgchFrom, size_t cchFrom, _In_count_(cchTo) const char* rgchTo, size_t cchTo) noexcept
{ return Mso::StringCore::Details::TryReplaceAll<char, Mso::StringExact>(sz, cch, cchMax, rgchFrom, cchFrom, rgchTo, cchTo); }
bool Mso::StringExact::TryReplaceAll(_In_z_count_(cch) _Out_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_count_(cchFrom) const wchar_t* rgwchFrom, size_t cchFrom, _In_count_(cchTo) const wchar_t* rgwchTo, size_t cchTo) noexcept
{ return Mso::StringCore::Details::TryReplaceAll<wchar_t, Mso::StringExact>(wz, cch, cchMax, rgwchFrom, cchFrom, rgwchTo, cchTo); }

/**
	Mso::StringExact::SgnCompareWz
*/
int Mso::StringExact::SgnCompareWz(const void *pv1, const void *pv2) noexcept
{
	return Mso::StringExact::Compare(*(LPCWSTR *)pv1, *(LPCWSTR *)pv2);
}











/**
	Mso::StringAscii::Compare
	Mso::StringAscii::Compare

	Ascii-compare function that works with two strings.
*/
int Mso::StringAscii::Compare(_In_z_ const char* sz1, _In_z_ const char* sz2)  noexcept
	{ return CompareTz<char, Mso::StringAscii>(sz1, sz2); }
int Mso::StringAscii::Compare(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2)  noexcept
	{ return CompareTz<wchar_t, Mso::StringAscii>(wz1, wz2); }

/**
	Mso::StringAscii::Compare
	Mso::StringAscii::Compare
	Mso::StringAscii::Compare

	Case-insensitive ASCII compare functions that work with two character buffers.
*/
int Mso::StringAscii::Compare(_In_count_(cch1) const char* rgch1, int cch1, _In_count_(cch2) const char* rgch2, int cch2)  noexcept
	{ return CompareRgtch<char, char, Mso::StringAscii>(rgch1, cch1, rgch2, cch2); }
int Mso::StringAscii::Compare(_In_count_(cch1) const wchar_t* rgwch1, int cch1, _In_count_(cch2) const wchar_t* rgwch2, int cch2)  noexcept
	{ return CompareRgtch<wchar_t, wchar_t, Mso::StringAscii>(rgwch1, cch1, rgwch2, cch2); }
int Mso::StringAscii::Compare(_In_count_(cch1) const char* rgch, UINT cch1, _In_count_(cch2) const wchar_t* rgwch, UINT cch2)  noexcept
    { return CompareRgtch<char, wchar_t, Mso::StringAscii>(rgch, static_cast<int>(cch1), rgwch, static_cast<int>(cch2)); }

/**
	Mso::StringAscii::EndsWith
*/
const wchar_t* Mso::StringAscii::EndsWith(const wchar_t* rgwch, int cch, const wchar_t* rgwchFind, int cchFind) noexcept
	{ return EndsWithRgtch<wchar_t, Mso::StringAscii>(rgwch, cch, rgwchFind, cchFind); }

int32_t Mso::StringAscii::IndexOf(const char* rgch, size_t cch, const char* rgchFind, size_t cchFind) noexcept
{ return Mso::StringCore::Details::IndexOf<char, Mso::StringAscii>(rgch, cch, rgchFind, cchFind); }
int32_t Mso::StringAscii::IndexOf(const wchar_t* rgwch, size_t cch, const wchar_t* rgwchFind, size_t cchFind) noexcept
{ return Mso::StringCore::Details::IndexOf<wchar_t, Mso::StringAscii>(rgwch, cch, rgwchFind, cchFind); }

const char* Mso::StringAscii::Find(const char* rgch, int cch, const char* rgchFind, int cchFind) noexcept
{ return Mso::StringCore::Details::Find<char, Mso::StringAscii>(rgch, cch, rgchFind, cchFind); }
const wchar_t* Mso::StringAscii::Find(const wchar_t* rgwch, int cch, const wchar_t* rgwchFind, int cchFind) noexcept
{ return Mso::StringCore::Details::Find<wchar_t, Mso::StringAscii>(rgwch, cch, rgwchFind, cchFind); }

bool Mso::StringAscii::TryReplaceAll(_In_z_count_(cch) _Out_z_cap_(cchMax) char* sz, size_t cch, size_t cchMax, _In_count_(cchFrom) const char* rgchFrom, size_t cchFrom, _In_count_(cchTo) const char* rgchTo, size_t cchTo) noexcept
{ return Mso::StringCore::Details::TryReplaceAll<char, Mso::StringAscii>(sz, cch, cchMax, rgchFrom, cchFrom, rgchTo, cchTo); }
bool Mso::StringAscii::TryReplaceAll(_In_z_count_(cch) _Out_z_cap_(cchMax) wchar_t* wz, size_t cch, size_t cchMax, _In_count_(cchFrom) const wchar_t* rgwchFrom, size_t cchFrom, _In_count_(cchTo) const wchar_t* rgwchTo, size_t cchTo) noexcept
{ return Mso::StringCore::Details::TryReplaceAll<wchar_t, Mso::StringAscii>(wz, cch, cchMax, rgwchFrom, cchFrom, rgwchTo, cchTo); }

/**
	Mso::StringAscii::SgnCompareWz
*/
int Mso::StringAscii::SgnCompareWz(const void *pv1, const void *pv2) noexcept
{
	return Mso::StringAscii::Compare(*(LPCWSTR *)pv1, *(LPCWSTR *)pv2);
}


/**
	Mso::StringInvariant::ToUpperChar

	Locale-invariant upper-casing function.
*/

typedef WORD LID;
WCHAR MsoWchToUpperLid(WCHAR a, LID, WORD) noexcept
{
return a;
}

wchar_t Mso::StringInvariant::ToUpperChar(wchar_t wch) noexcept
	{
	return MsoWchToUpperLid(wch, 0, 0);
	}

/**
	Mso::StringInvariant::Compare
	Mso::StringInvariant::Compare

	Locale-invariant equality function.
*/
int Mso::StringInvariant::Compare(_In_z_ const char* sz1, _In_z_ const char* sz2)  noexcept
	{ return CompareTz<char, Mso::StringInvariant>(sz1, sz2); }
int Mso::StringInvariant::Compare(_In_z_ const wchar_t* wz1, _In_z_ const wchar_t* wz2)  noexcept
	{ return CompareTz<wchar_t, Mso::StringInvariant>(wz1, wz2); }

/**
	Mso::StringInvariant::Compare

	Compares the sort order of the two unicode strings rgwch1
	(# of chars cch1) and rgwch2 (# of chars cch2).  Returns negative if
	rgwch1 < rgwch2, 0 if they sort equally, and positive if rgwch1 > rgwch2.
	The string compare is based on Unicode order. Mso::StringInvariant::ToUpperChar is
	used as an invariant ToUpper. It must behave the same no matter which
	language we run on.
*/
int Mso::StringInvariant::Compare(_In_opt_count_(cch1) const wchar_t *rgwch1, int cch1, _In_opt_count_(cch2) const wchar_t *rgwch2, int cch2)  noexcept
	{ 
	return CompareRgtch<wchar_t, wchar_t, Mso::StringInvariant>(rgwch1, cch1, rgwch2, cch2); 
	}
