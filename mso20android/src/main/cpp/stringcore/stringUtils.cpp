/**
	StringUtils.cpp

	Copyright (c) 1994 Microsoft Corporation

	Functions related to working with strings:
		string copy
		string append
		string search
		string length
		string replace
*/
#include "precomp.h"
#include "comparison.h"
#include <core_root/msoStringConversion.h>
#include <core_root/msoStringTraits.h>
#include <stringcore/core/stringutils.h>
#include <string>

template <typename TChar>
static _Ret_z_ TChar* TruncCopy(_Out_z_cap_(cchDst) TChar *wzDst, _In_opt_z_ const TChar *wzSrc, size_t cchDst) noexcept
{
	VerifyElseCrashTag(wzSrc != nullptr, 0x027d300d /* tag_c5tan */);
	VerifyElseCrashTag(wzDst != nullptr && cchDst > 0, 0x027d300e /* tag_c5tao */);
	size_t cchSrc = std::min( std::char_traits<TChar>::length(wzSrc), cchDst -1 );

	memmove(wzDst, wzSrc, cchSrc*sizeof(TChar));
	wzDst[cchSrc] = 0;
	return(wzDst+cchSrc);
}

/**
	WzTruncCopy

	Copies the null-terminated string wzSrc to wzDst, when cbDst is
	the size of the buffer at wzDst (in bytes).  Truncates the string if it
	is too big to fit.  Resulting string is always null-terminated.

	Returns a point to the end of the string (i.e. the NIL)
*/
_Ret_z_ WCHAR *WzTruncCopy(_Out_z_cap_(cchDst) WCHAR *wzDst, _In_z_ const WCHAR *wzSrc, unsigned cchDst) noexcept
{
	AssertTag(cchDst > 0, 0x0010a2d8 /* tag_aekly */);
	return TruncCopy(wzDst, wzSrc, cchDst);
}


/**
	SzTruncCopy

	Copies the null-terminated string szSrc to szDst, when cbDst is
	the size of the buffer at szDst.   Truncates the string if it is
	too big to fit.  Resulting string is always null-terminated.

	Returns a point to the end of the string (i.e. the NIL)
*/
char *SzTruncCopy(_Out_z_cap_(cchDst) CHAR *szDst, const CHAR *szSrc, unsigned cchDst) noexcept
{
	AssertTag(cchDst > 0, 0x0010a2d9 /* tag_aeklz */);
	return TruncCopy(szDst, szSrc, cchDst);
}


/* M S O  W Z  T O K E N */
/**
	Implements functionality similar to the c-runtime's wcstok. However, instead of keeping
	a static string between calls, it uses the callers PWSTR to track where to look for the
	next token.
*/
MSOEXTERN_C PWSTR MsoWzToken(PWSTR * pwzToken, PCWSTR wzDelimiterList) noexcept
{
	AssertTag((pwzToken != NULL), 0x0010a2da /* tag_aekl0 */);
	AssertTag((wzDelimiterList != NULL), 0x0010a2db /* tag_aekl1 */);

	PWSTR wzToken = *pwzToken;

	// skip leading delimiters
	for (; *wzToken; wzToken++)
		{
		PCWSTR wzDelimiter = wzDelimiterList;
		for (;
			 *wzDelimiter && (*wzDelimiter != *wzToken);
			 wzDelimiter++)
			{
			}
		if (!*wzDelimiter)
			{
			break;
			}
		}

	PWSTR wzNextToken = wzToken;

	// find next delimiter, replace delimiter with NULL if not end of string
	for (; *wzNextToken; wzNextToken++)
		{
		PCWSTR wzDelimiter = wzDelimiterList;
		for (;
			 *wzDelimiter && (*wzDelimiter != *wzNextToken);
			 wzDelimiter++)
			{
			}
		if (*wzDelimiter)
			{
			*wzNextToken = 0;
			wzNextToken++;
			break;
			}
		}

	*pwzToken = wzNextToken;

	return((wzToken != wzNextToken) ? wzToken : nullptr);
}

/**
   MsoFStripLeadingAndEndingWSWzCore

   Removes all leading and trailing whitespace from the given string.
   Returns true if any such whitespace was found, and allocates wzOut with the result.
*/
OACR_WARNING_SUPPRESS(INVALID_PARAM_VALUE_3, "historical issue")
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
BOOL MsoFStripLeadingAndEndingWSWzCore(const WCHAR *wzIn, WCHAR **wzOut, IMsoMemHeap *pmmh) noexcept
{
	AssertTag(wzIn && wzOut, 0x0010a2dc /* tag_aekl2 */);

	const WCHAR *pwchStart = wzIn;
	const WCHAR *pwchEndOrig;
	const WCHAR *pwchEnd = pwchEndOrig = wzIn + MsoCchWzLen(wzIn) - 1;

	// figure out where the whitespace ends at the beginning and end
	while (MsoFSpaceWch(*pwchEnd) && (pwchEnd >= pwchStart))
		pwchEnd--;
	while (MsoFSpaceWch(*pwchStart) && (pwchStart <= pwchEnd))
		pwchStart++;

	// did we find any whitepsace
	if ((pwchStart != wzIn) || (pwchEnd != pwchEndOrig))
		{
		WCHAR *wzDest = nullptr;
		// SafeInt<int> cch = SafeInt<ptrdiff_t>(pwchEnd - pwchStart) + 1;
        auto cch = static_cast<ptrdiff_t>(pwchEnd - pwchStart) + 1;

		// allocate the resulting string
		Debug(Mso::Memory::AutoShutdownLeakScope scope);
		HRESULT hr = HrMsoAllocHost(sizeof(WCHAR) * (cch + 1), (void **)(&wzDest), pmmh);
		*wzOut = wzDest;

		// copy over the non-whitespace portion
		if (SUCCEEDED(hr) && wzDest)
			{
			MsoRgwchCopy(pwchStart, cch, wzDest, cch + 1);
			*(wzDest + (int)cch) = 0;
			}

		return(true);
		}

	*wzOut = (WCHAR*)wzIn;

	return(false);
}

/**
*/
BOOL MsoFStripLeadingAndEndingWSWz(const WCHAR *wzIn, _Deref_out_z_ WCHAR **wzOut) noexcept
{
	return(MsoFStripLeadingAndEndingWSWzCore(wzIn, wzOut, nullptr));
}

// Tables used by language dependent upper case conversion
#define ch(_i)  ((WCHAR) (_i))
const WCHAR mpwchLatin[] =
	{
	/* 0/8      1/9      2/A      3/B      4/C      5/D      6/E      7/F */
	ch('A'), ch('A'), ch('A'), ch(195), ch(196), ch(197), ch(198), ch(199), /* 0x00C_ */
	ch('E'), ch('E'), ch('E'), ch(203), ch('I'), ch('I'), ch('I'), ch(207),
	ch(208), ch(209), ch('O'), ch('O'), ch('O'), ch(213), ch(214), ch(215), /* 0x00D_ */
	ch(216), ch('U'), ch('U'), ch('U'), ch(220), ch(221), ch(222), ch(223),
	ch('a'), ch('a'), ch('a'), ch(227), ch(228), ch(229), ch(230), ch(231), /* 0x00E_ */
	ch('e'), ch('e'), ch('e'), ch(235), ch('i'), ch('i'), ch('i'), ch(239),
	ch(240), ch(241), ch('o'), ch('o'), ch('o'), ch(245), ch(246), ch(247), /* 0x00F_ */
	ch(248), ch('u'), ch('u'), ch('u'), ch(252), ch(253), ch(254), ch(255),
	ch('A'), ch('a'), ch('A'), ch('a'), ch('A'), ch('a'), ch('C'), ch('c'), /* 0x010_ */
	ch('C'), ch('c'), ch('C'), ch('c'), ch('C'), ch('c'), ch('D'), ch('d'),
	ch('D'), ch('d'), ch('E'), ch('e'), ch('E'), ch('e'), ch('E'), ch('e'), /* 0x011_ */
	ch('E'), ch('e'), ch('E'), ch('e'), ch('G'), ch('g'), ch('G'), ch('g'),
	ch('G'), ch('g'), ch('G'), ch('g'), ch('H'), ch('h'), ch('H'), ch('h'), /* 0x012_ */
	ch('I'), ch('i'), ch('I'), ch('i'), ch('I'), ch('i'), ch('I'), ch('i'),
	ch('I'), ch('i'), ch(306), ch(307), ch('J'), ch('j'), ch('K'), ch('k'), /* 0x013_ */
	ch(312), ch('L'), ch('l'), ch('L'), ch('l'), ch('L'), ch('l'), ch('L'),
	ch('l'), ch('L'), ch('l'), ch('N'), ch('n'), ch('N'), ch('n'), ch('N'), /* 0x014_ */
	ch('n'), ch('n'), ch('N'), ch('n'), ch('O'), ch('o'), ch('O'), ch('o'),
	ch('O'), ch('o'), ch(338), ch(339), ch('R'), ch('r'), ch('R'), ch('r'), /* 0x015_ */
	ch('R'), ch('r'), ch('S'), ch('s'), ch('S'), ch('s'), ch('S'), ch('s'),
	ch('S'), ch('s'), ch('T'), ch('t'), ch('T'), ch('t'), ch('T'), ch('t'), /* 0x016_ */
	ch('U'), ch('u'), ch('U'), ch('u'), ch('U'), ch('u'), ch('U'), ch('u'),
	ch('U'), ch('u'), ch('U'), ch('u'), ch('W'), ch('w'), ch('Y'), ch('y'), /* 0x017_ */
	ch('Y'), ch('Z'), ch('z'), ch('Z'), ch('z'), ch('Z'), ch('z'), ch(383)
	};
#undef ch

const WCHAR mpwchGreek[] =
	{
	/* 8       9       A       B       C       D       E       F*/
	0x0395, 0x0397, 0x0399, 0x038B, 0x039F, 0x039D, 0x03A5, 0x03A9, /* 038 */
	0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03B1, 0x03B5, 0x03B7, 0x03B9, /* 03A */
	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03BF, 0x03C5, 0x03C9, 0x03CF, /* 03C */
	0x03E8, 0x03E9, 0x03EA, 0x03EB, 0x03EC, 0x03ED, 0x03EE, 0x03EF  /* 03E */
  };


/**
	MsoWchStripAccent

	This function strips the accent of WinLatin1 and Greek characters.
	Please note that European Latin, Extended Latin, and Cyrillic
	characters are unaffected.
*/
WCHAR MsoWchStripAccent(WCHAR wch) noexcept
{
	// Latin
	if (wch >= 0x00C0 && wch <= 0x017F)
		return(mpwchLatin[wch-0x00C0]);

	// Greek
	else if ((wch & 0xFF98) == 0x0388)
#ifdef DEBUG
		{
		int impwch = ((wch & 0x0060)>>2) | (wch & 0x0007);
		AssertTag(impwch >= 0 && impwch < 32, 0x0010a2dd /* tag_aekl3 */);
		return(mpwchGreek[impwch]);
		}
#else // DEBUG
		return(mpwchGreek[((wch & 0x0060)>>2) | (wch & 0x0007)]);
#endif // DEBUG

	else if (wch == 0x0386) // Intersting Greek satellite character
		return(0x0391);

	else
		return(wch);
}

/**
   FWchInRgwchAscending

	Performs a binary search for wch in rgwch which has cwch elements. This
	function assumes that rgwch is already sorted in ascending order.
*/
BOOL FWchInRgwchAscending(WCHAR wch, const WCHAR *rgwch, int cwch) noexcept
{
	int iwchLow, iwchMid, iwchHigh;

	iwchLow = 0;
	iwchHigh = cwch - 1;
	while (iwchLow <= iwchHigh)
		{
		iwchMid = (iwchLow + iwchHigh) >> 1;
		if (wch < rgwch[iwchMid])
			iwchHigh = iwchMid - 1;
		else if (wch > rgwch[iwchMid])
			iwchLow = iwchMid + 1;
		else
			{
			AssertSzTag(wch == rgwch[iwchMid], "Something's wrong with the logic above.", 0x0010a2de /* tag_aekl4 */);
			return(true);
			}
		}
	return(false);
}

/**
	Replaces all occurrences of wchOut with wchIn
*/
void MsoReplaceAllOfWchWithWch(WCHAR *wz, WCHAR wchOut, WCHAR wchIn) noexcept
	{
	while (*wz != 0)
		{
		if (*wz == wchOut)
			*wz = wchIn;

		++ wz;
		}
	}

/**
	Finds the last (rightmost) occurrence of the character wch in the
	null-terminated UNICODE  string cwz, and returns a pointer to it.
	Returns NULL if the character was not in the string.
*/
const WCHAR* MsoCchWzIndexRight(const WCHAR *cwz, INT_PTR cch, int wch) noexcept
{
	AssertTag(0 < cch, 0x0010a2df /* tag_aekl5 */);
	if (cwz && 0 < cch)
		{
		AssertTag(cch <= MsoCchWzLen(cwz), 0x0010a2e0 /* tag_aekl6 */);
		cch = SafeMin(cch, MsoCchWzLen(cwz));
		for (const WCHAR *pwch = cwz + cch - 1; pwch >= cwz; pwch--)
			if (*pwch == wch)
				return(pwch);
   	}
	return(NULL);
}

/**
	PchStrStrFast

	Finds the occurrence of the substring pchFind inside the string pch, and
	returns a pointer to the beginning of it. Returns NULL if the substring
	is not found. Case sensitive and looks at bytes only.
*/
char* PchStrStrFast(const char *pch, int cch, const char *pchFind, int cchFind) noexcept
{
	int ich;

	for (ich = 0; ich + cchFind <= cch; ich++)
		{
		if (memcmp(&pch[ich], pchFind, static_cast<size_t>(cchFind)) == 0)
			return((char *)&pch[ich]);
		}
	return(NULL);
}

/**
	MsoPwchStrStrFast

	Finds the occurrence of the substring pwchFind inside the string pwch, and
	returns a pointer to the beginning of it. Returns NULL if the substring
	is not found. Case sensitive and looks at WCHAR's only.
*/
MSOEXTERN_C const WCHAR* MsoPwchStrStrFast(const WCHAR *pwch, int cch, const WCHAR *pwchFind, int cchFind) noexcept
{
	int ich;

	for (ich = 0; ich + cchFind <= cch; ich++)
		{
		if (memcmp(&pwch[ich], pwchFind, cchFind*sizeof(WCHAR)) == 0)
			return(&pwch[ich]);
		}
	return(NULL);
}


/**
	MsoPwchStrStrRightFast

	Same as MsoPwchStrStrFast but searches from the right.
*/
const WCHAR* MsoPwchStrStrRightFast(const WCHAR *pwch, int cch, const WCHAR *pwchFind, int cchFind) noexcept
{
	int ich;

	for (ich = cch - cchFind; ich >= 0; ich--)
		{
		if (memcmp(&pwch[ich], pwchFind, cchFind*sizeof(WCHAR)) == 0)
			return(&pwch[ich]);
		}
	return(NULL);
}

/**
*/
WCHAR * MsoWzQueryValue(const void *pb, unsigned int cb, const WCHAR *wz, unsigned int cch) noexcept
{
	WCHAR *pwch = (WCHAR *)pb;
	const WCHAR *pwchMac = (const WCHAR *)((char *)pb + cb);

	while (pwch < pwchMac)
	{
		if (*pwch == *wz)
		{
			if (Mso::StringExact::Equal(pwch, static_cast<int>(cch), wz, static_cast<int>(cch)))
			{
				WCHAR *pwchRet = pwch + cch + 1;
				if (*pwchRet == 0)
					pwchRet++;
				return pwchRet;
			}
		}
		pwch++;
	}
	return nullptr;
}

/**
	MsoWzAppend

	Append null-terminated string wzFrom onto the end of wzTo, such that the
	resulting string is <cch and
	returns a pointer to the end of the destination string.
*/
WCHAR* MsoWzAppend(const WCHAR *wzFrom, WCHAR *wzTo, int cchTo) noexcept
{
	AssertTag(wzTo != NULL, 0x0010a2e1 /* tag_aekl7 */);
	while (cchTo > 0 && *wzTo != L'\0')
		{
		wzTo++;
		cchTo--;
		}
	if (cchTo <= 0)
		{
		ShipAssertSz1Tag(false, "Output buffer full - can't append '%S'", wzFrom, 0x0010a2e2 /* tag_aekl8 */);
		return(wzTo);
		}

	return(TruncCopy(wzTo, wzFrom, static_cast<unsigned int>(cchTo)));
}

/**
	MsoSzAppend

	Append null-terminated string szFrom onto the end of szTo, such that the
	resulting string is < cch and
	returns a pointer to the end of the destination string.
*/
CHAR* MsoSzAppend(const CHAR *szFrom, CHAR *szTo, int cchTo) noexcept
{
	AssertTag(szTo != NULL, 0x0010a2e3 /* tag_aekl9 */);
	AssertSzTag(MsoCchSzLen(szFrom) + MsoCchSzLen(szTo) < cchTo, "the buffer is too short; string will be truncated", 0x0010a300 /* tag_aekma */);

	while (cchTo > 0 && *szTo != '\0')
		{
		szTo++;
		cchTo--;
		}
	if (cchTo <= 0)
		{
		ShipAssertSzTag(false, "Cch passed in is less than buffer size of szTo", 0x0010a301 /* tag_aekmb */);
		return(szTo);
		}

	return(TruncCopy(szTo, szFrom, static_cast<unsigned int>(cchTo)));
}

/**
	MsoWtzAppend

	Append the WTZ string wtzFrom onto the end of wtzTo.
*/
void MsoWtzAppend(const WCHAR *wtzFrom, WCHAR *wtzTo, int cch) noexcept
{
	AssertTag(MsoFValidWtz(wtzFrom), 0x0010a302 /* tag_aekmc */);
	AssertTag(MsoFValidWtz(wtzTo), 0x0010a303 /* tag_aekmd */);
	// TODO: (CWhite) Think broadly about how callers should differentiate
	// expected truncation from unexpected truncation, and how and if this
	// and other API's should return info about whether truncation has
	// occured
	//AssertSzTag(*wtzFrom + *wtzTo + 1 < cch, "This would be a buffer overflow in XP", 0x71757a6a /* tag_quzj */);

	WCHAR *wz = &wtzTo[1+wtzTo[0]];
	AssertTag(*wz == 0, 0x0010a304 /* tag_aekme */);
	int cchCopy = __min(wtzFrom[0], cch-2-wtzTo[0]);
	if (cchCopy < 0)
		return;

	memmove(wz, &wtzFrom[1], (cchCopy)*sizeof(WCHAR));
	wtzTo[0] = (WCHAR)(wtzTo[0] + cchCopy);
	wtzTo[wtzTo[0]+1] = 0;
	DebugFill(wtzTo+*wtzTo+2, (cch-*wtzTo-2)*sizeof(WCHAR));
}

/**
	MsoRgchCopy

	Copies cchSrc characters from the source string (which need not be
	null terminated) into a buffer (of size cchDest), truncating if
    necessary, and null terminating the destination.
*/
MSOEXTERN_C CHAR * MsoRgchCopy(const CHAR *rgchSrc, int cchSrc, CHAR *szDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a305 /* tag_aekmf */);
	AssertTag(cchDest > 0, 0x0010a306 /* tag_aekmg */);
	AssertTag(rgchSrc != NULL, 0x0010a307 /* tag_aekmh */);
	AssertTag(szDest != NULL, 0x0010a308 /* tag_aekmi */);

	if (cchDest <= 0)
		return(szDest);
	if (cchSrc < 0)
		cchSrc = 0;

	int cch = __min(cchSrc, cchDest-1);
	memmove(szDest, rgchSrc, static_cast<size_t>(cch));
	szDest[cch] = 0;
	DebugFill(szDest+cch+1, static_cast<size_t>(cchDest-cch-1));
	return(szDest);
};

/**
	MsoRgwchCopy

	Copies cchSrc characters from the source string (which need not be
	null terminated) into a buffer (of size cchDest), truncating if
	necessary, and null terminating the destination.
*/
MSOEXTERN_C WCHAR * MsoRgwchCopy(const WCHAR *rgwchSrc, int cchSrc, _Out_z_cap_(cchDest) WCHAR *wzDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a309 /* tag_aekmj */);
	AssertTag(cchDest > 0, 0x0010a30a /* tag_aekmk */);
	AssertTag(rgwchSrc != NULL, 0x0010a30b /* tag_aekml */);
	AssertTag(wzDest != NULL, 0x0010a30c /* tag_aekmm */);

	if (cchDest <= 0)
		return(wzDest);
	if (cchSrc < 0)
		cchSrc = 0;

	int cch = __min(cchSrc, cchDest-1);
	memmove(wzDest, rgwchSrc, cch*sizeof(WCHAR));
	wzDest[cch] = 0;
	DebugFill(wzDest+cch+1, (cchDest-cch-1)*sizeof(WCHAR));
	return(wzDest);
};

/**
	MsoRgchAppend

	Appends cchSrc characters from the source string (which need not be
	null terminated) to the destination string (of size cchDest),
	truncating if necessary, and null terminating the destination.
*/
MSOEXTERN_C CHAR * MsoRgchAppend(const CHAR *rgchSrc, int cchSrc, CHAR *szDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a30d /* tag_aekmn */);
	AssertTag(cchDest >= 0, 0x0010a30e /* tag_aekmo */);
	AssertTag(rgchSrc != NULL, 0x0010a30f /* tag_aekmp */);
	AssertTag(szDest != NULL, 0x0010a310 /* tag_aekmq */);

	int cch = MsoCchSzLen(szDest);
	AssertTag(cch < cchDest, 0x0010a311 /* tag_aekmr */);
	if (cch < cchDest)
		MsoRgchCopy(rgchSrc, cchSrc, szDest+cch, cchDest-cch);
	return(szDest);
};

/**
	MsoRgwchAppend

	Appends cchSrc characters from the source string (which need not be
	null terminated) to the destination string (of size cchDest),
	truncating if necessary, and null terminating the destination.
*/
WCHAR * MsoRgwchAppend(const WCHAR *rgwchSrc, int cchSrc, _Out_z_cap_(cchDest) WCHAR *wzDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a312 /* tag_aekms */);
	AssertTag(cchDest >= 0, 0x0010a313 /* tag_aekmt */);
	AssertTag(rgwchSrc != NULL, 0x0010a314 /* tag_aekmu */);
	AssertTag(wzDest != NULL, 0x0010a315 /* tag_aekmv */);

	int cch = MsoCchWzLen(wzDest);
	AssertTag(cch < cchDest, 0x0010a316 /* tag_aekmw */);
	if (cch < cchDest)
		MsoRgwchCopy(rgwchSrc, cchSrc,wzDest+cch, cchDest-cch);
	return(wzDest);
};

/**
	MsoRgwchWtzAppend

	Appends cchSrc characters from the source string (which need not be
	null terminated) to the destination string (of wtz type and size cchDest),
	truncating if necessary, and null terminating the destination.
*/
WCHAR * MsoRgwchWtzAppend(const WCHAR *rgwchSrc, int cchSrc, WCHAR *wtzDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a317 /* tag_aekmx */);
	AssertTag(cchDest >= 0, 0x0010a318 /* tag_aekmy */);
	AssertTag(rgwchSrc != NULL, 0x0010a319 /* tag_aekmz */);
	AssertTag(wtzDest != NULL, 0x0010a31a /* tag_aekm0 */);
	AssertTag(MsoFValidWtz(wtzDest), 0x0010a31b /* tag_aekm1 */);

	if (wtzDest[0] + 2 > cchDest)
		{
		ShipAssertSzTag(false, "Not reached", 0x0010a31c /* tag_aekm2 */);
		return(wtzDest);
		}
	MsoRgwchAppend(rgwchSrc, cchSrc, wtzDest + 1, cchDest - 1);
	wtzDest[0] = (WCHAR)MsoCchWzLen(wtzDest + 1);
	return(wtzDest);
};

/**
	MsoRgwchWtAppend

	Appends cchSrc characters from the source string (which need not be
	null terminated) to the destination string (of wt type and size cchDest),
	truncating if necessary.
*/
WCHAR * MsoRgwchWtAppend(const WCHAR *rgwchSrc, int cchSrc, WCHAR *wtDest, int cchDest) noexcept
{
	AssertTag(cchSrc >= 0, 0x0010a31d /* tag_aekm3 */);
	AssertTag(cchDest >= 0, 0x0010a31e /* tag_aekm4 */);
	AssertTag(rgwchSrc != NULL, 0x0010a31f /* tag_aekm5 */);
	AssertTag(wtDest != NULL, 0x0010a320 /* tag_aekm6 */);

	if (wtDest[0] + 1 > cchDest)
		{
		ShipAssertSzTag(false, "Not reached", 0x0010a321 /* tag_aekm7 */);
		return(wtDest);
		}
	int cchCopy = __min(cchSrc, cchDest - wtDest[0] - 1);
	memmove(wtDest + 1 + wtDest[0], rgwchSrc, cchCopy*sizeof(WCHAR));
	wtDest[0] = (WCHAR)(wtDest[0] + cchCopy);
	DebugFill(wtDest + 1 + wtDest[0], static_cast<size_t>(cchDest - wtDest[0] - 1));
	return(wtDest);
};

/**
	A simple string copy, copying rgwch,cch to wtzTo (limit of cch chars,
	incl length and null char)  Returns false on truncation.
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoWtzRgwchCopy(const WCHAR *rgwchFrom, int cchFrom, WCHAR *wtzTo, int cchMax) noexcept
{
	VerifyElseCrashTag(!(cchMax < 2), 0x00322850 /* tag_am87q */);

	*wtzTo = (WCHAR) __min(cchFrom, cchMax-2);
	memmove(wtzTo + 1, rgwchFrom, (*wtzTo) * sizeof(WCHAR));
	OACR_ASSUME(*wtzTo + 1 < cchMax);
	wtzTo[*wtzTo + 1] = 0;
	DebugFill(wtzTo+*wtzTo+2, (cchMax-*wtzTo-2)*sizeof(WCHAR));
	return(*wtzTo == cchFrom);
}

/**
	A simple string copy, copying rgwch,cch to wtTo (limit of cchMax chars,
	inclu length char).  Returns false on truncation.
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoWtRgwchCopy(const WCHAR *rgwchFrom, int cchFrom, WCHAR *wtTo, int cchMax) noexcept
{
	VerifyElseCrashTag(!(cchMax < 1), 0x00322851 /* tag_am87r */);

	memmove(wtTo+1, rgwchFrom, (*wtTo = (WCHAR)__min(cchFrom, cchMax-1))*sizeof(WCHAR));
	DebugFill(wtTo+*wtTo+1, (cchMax-*wtTo-1)*sizeof(WCHAR));
	return(*wtTo == cchFrom);
}


/**
	MsoRgchIndexSimple

	Returns pointer to first occurrence of ch in rgch, NULL if not found.
	No DBCS characters assumed
*/
const CHAR* MsoRgchIndexSimple(const CHAR *rgch, int cch, char c) noexcept
{
	const CHAR *pch;
	if (cch <= 0 || rgch == nullptr)
		return(NULL);

	for (pch = rgch; pch < rgch+cch; )
		{
		if (*pch == c)
			return(pch);
		pch++;
		}
	return(NULL);
}


/**
	MsoRgchIndexRight

	Returns pointer to last occurrence of ch in rgch, NULL if not found.
*/
const CHAR* MsoRgchIndexRight(const CHAR *rgch, int cch, CHAR ch) noexcept
{
	if (cch < 0 || rgch == nullptr)
		return(NULL);

	while (cch-- && *(rgch + cch) != ch);
	return((cch<0) ? nullptr : rgch + cch);
}


/**
	MsoRgwchIndex

	Returns pointer to first occurrence of wch in rgwch, NULL if not found.
*/
const WCHAR* MsoRgwchIndex(const WCHAR *rgwch, int cch, int wch) noexcept
{
	if (cch < 0 || rgwch == nullptr)
		return(NULL);

	while (cch-- && *rgwch++ != wch);
	return((cch<0) ? nullptr : rgwch-1);
}


/**
	MsoRgwchIndexRight

	Returns pointer to last occurrence of wch in rgwch, NULL if not found.
*/
const WCHAR* MsoRgwchIndexRight(const WCHAR *rgwch, int cch, WCHAR wch) noexcept
{
	if (cch < 0 || rgwch == nullptr)
		return(NULL);

	while (cch-- && *(rgwch + cch) != wch);
	return((cch<0) ? nullptr : rgwch + cch);
}


/**
	MsoCpCchSzLenFromWz

	Given a wz returns the cch in the sz derived from the wz.
	Returns -1 on error.  Count does not include zero-terminator
*/
int MsoCpCchSzLenFromWz(UINT CodePage, const WCHAR *wz) noexcept
{
	int cch;

	if (*wz == 0)
		return(0);

	if ((cch = MsoRgwchToCpRgchCore(CodePage, wz, MsoCchWzLen(wz), nullptr, 0, nullptr)) == 0)
		cch = -1;

	return(cch);
}

/**
	Remove leading and trailing spaces.

	No buffer size passed in since the input string will not expand.
*/
WCHAR* MsoWzStripSpaces(WCHAR *pch) noexcept
{
	WCHAR *pchSave = pch;

	while (MsoFSpaceWch(*pch))
		++ pch;

	if (*pch==0)
		{
		// All spaces in this string.
		pchSave[0] = 0;
		return pchSave;
		}


	WCHAR *pchLast = pch + MsoCchWzLen(pch) - 1;

	while (MsoFSpaceWch(*pchLast))
		pchLast--;

	pchLast[1] = 0;

	memmove(pchSave, pch, (MsoCchWzLen(pch) + 1) * sizeof(WCHAR));

	return pch;
}



