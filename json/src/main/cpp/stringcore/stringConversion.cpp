/**
	Copyright (c) 1994 Microsoft Corporation

	Functions related to converting strings
*/
#include "precomp.h"
#include <stringcore/cloning.h>
#include <core_root/msoStringConversion.h>
#include <stringcore/localesupport.h>
#include <stringcore/core/stringconversion.h>
#include <stringcore/core/stringparsing.h>
#include <stringcore/core/stringtraits.h>
#include <stringcore/core/unicodetraits.h>
#include <stringcore/codepages.h>
#include <stringcore/languageids.h>
#include <stringcore/stringapiflags.h>
#include <stringcore/surrogatepairsupport.h>
#include <stringcore/stringConvertData.h>
#include <stringcore/widechartoutf8conversion.h>
#include <stringcore/stringcore.h>
// #include <memoryleakscope/memoryleakscope.h>

UINT _wcsicmp(
        WCHAR * lpwstr1,
        WCHAR * lpwstr2)
{
    USHORT ch1, ch2;

    for(;;) {
        ch1 = *lpwstr1++;
        ch2 = *lpwstr2++;

        if (ch1 == 0 || ch2 == 0)
            break;

        if (tolower(LOBYTE(ch1)) != tolower(LOBYTE(ch2)))
            return ~0u;
    }

    return ch1 || ch2;
}

/*------------------------------------------------------------------------
	MsoWtCopy

	A simple string copy, copying wtFrom to wtTo.
---------------------------------------------------------------- RICKP -*/
void MsoWtCopy(const WCHAR* wtFrom, WCHAR* wtTo)
{
    memcpy(wtTo, wtFrom, (MsoCchWtLen(wtFrom)+1)*sizeof(WCHAR));
}


inline bool StringCoreFInPlace(const void *pvFrom, const void *pvTo) noexcept
{
	AssertSzTag(pvFrom != pvTo, "function does not work in-place", 0x1f7a0045 /* tag_546bf */);
	return (pvFrom == pvTo);
}

/**
	MsoMultiByteToWideChar

	This function provides our own layer to WideCharToMultiByte() call.  We
	provide our own lookup table if it's not available from NLS subsystem.
	Currently works for 20 SBCS Win and 5 Mac code pages only.
*/
TASKLIBTECHDEBT_(int) MsoMultiByteToWideChar(UINT CodePage, DWORD  dwFlags,
	LPCCH lpMultiByteStr, int cchMultiByte32, 
	LPWSTR lpWideCharStr, int cchWideChar) noexcept
{

	// GAMMA : TODO : Implement using std::mbstowcs
	std::abort();

//	int cch, cchT;
//	const WCHAR *pwchConv;
//
//	if (lpWideCharStr)
//		DebugFill(lpWideCharStr, cchWideChar * sizeof(WCHAR));
//
//	// added to check that length of MultiByte String can be 0 with
//	// no harmful side effects thereafter (i.e., no conversion takes place)
//	AssertTag((lpMultiByteStr != NULL) || (cchMultiByte32==0), 0x0010a222 /* tag_aeki8 */);
//	// Win32 docs state that these must be different buffers
//	// note: can be same if both point to null values
//	AssertTag(((char *)lpWideCharStr != (char *)lpMultiByteStr) || (cchMultiByte32==0), 0x0010a223 /* tag_aeki9 */);
//
//	if (CodePage == CP_ACP || CodePage == CP_OEMCP || IsValidCodePage(CodePage))
//		{
//		return(MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cchMultiByte32, lpWideCharStr, cchWideChar));
//		}
//
//	if (cchMultiByte32 < 0)
//		cch = MsoCchSzLen(lpMultiByteStr) + 1;  // include null terminator
//	else
//		cch = cchMultiByte32;
//
//	// Special handling for UTF8
//	if (CodePage == CP_UTF8)
//		return(UTF8ToUnicode(lpMultiByteStr, &cch, lpWideCharStr, cchWideChar));
//
//	if (cchWideChar == 0)
//		return(cch);
//	if (cch > cchWideChar)
//	{
//		SetLastError(ERROR_INSUFFICIENT_BUFFER);
//		return(0);
//	}
//	cchT = cch;
//
//	switch (CodePage)
//		{
//		case CP_EASTEUROPE:
//			pwchConv = mpwchConv1250;
//			break;
//		case CP_RUSSIAN:
//			pwchConv = mpwchConv1251;
//			break;
//		case CP_WESTEUROPE:
//			pwchConv = mpwchConv1252;
//			break;
//		case CP_GREEK:
//			pwchConv = mpwchConv1253;
//			break;
//		case CP_TURKISH:
//			pwchConv = mpwchConv1254;
//			break;
//		case CP_HEBREW:
//			pwchConv = mpwchConv1255;
//			break;
//		case CP_ARABIC:
//			pwchConv = mpwchConv1256;
//			break;
//		case CP_BALTIC:
//			pwchConv = mpwchConv1257;
//			break;
//		case CP_VIETNAMESE:
//			pwchConv = mpwchConv1258;
//			break;
//		case CP_RUSSIANKOI8R:
//			pwchConv = mpwchConv20866;
//			break;
//		case CP_ASCII:
//		case CP_ISOLATIN1:
//			pwchConv = mpwchConv28591;
//			break;
//		case CP_ISOEASTEUROPE:
//			pwchConv = mpwchConv28592;
//			break;
//		case CP_ISOTURKISH:
//			pwchConv = mpwchConv28593;
//			break;
//		case CP_ISOBALTIC:
//			pwchConv = mpwchConv28594;
//			break;
//		case CP_ISORUSSIAN:
//			pwchConv = mpwchConv28595;
//			break;
//		case CP_ISOARABIC:
//			pwchConv = mpwchConv28596;
//			break;
//		case CP_ISOGREEK:
//			pwchConv = mpwchConv28597;
//			break;
//		case CP_ISOHEBREW:
//			pwchConv = mpwchConv28598;
//			break;
//		case CP_ISOTURKISH2:
//			pwchConv = mpwchConv28599;
//			break;
//		case CP_ISOLATIN9:
//			pwchConv = mpwchConv28605;
//			break;
//#ifdef MAC  // REVIEW nobuyah: bad assumption?
//		case CP_ACP:
//		case CP_OEMCP:
//#endif // MAC
//		case CP_MACCP:
//		case CP_MAC_ROMAN:
//			pwchConv = mpwchConv10000;
//			break;
//		case CP_MAC_GREEK:
//			pwchConv = mpwchConv10006;
//			break;
//		case CP_MAC_CYRILLIC:
//			pwchConv = mpwchConv10007;
//			break;
//		case CP_MAC_LATIN2:
//			pwchConv = mpwchConv10029;
//			break;
//		case CP_MAC_TURKISH:
//			pwchConv = mpwchConv10081;
//			break;
//		case CP_SYMBOL:
//			if (lpWideCharStr != nullptr)
//				{
//				while (cchT--)
//					{
//					*(lpWideCharStr++) = (WCHAR)MakeWordFirstSecond(*(const UCHAR*)lpMultiByteStr >= 0x20 ? 0xF0 : 0, *(lpMultiByteStr));
//					++lpMultiByteStr;
//					}
//				}
//			return (cch);
////SOUTHASIA
//        case CP_THAI:
//            pwchConv = mpwchConv874;
//            break;
//// SOUTHASIA
//		default:
//			// codepage not supported but if all ASCII then convert anyway.
//			pwchConv = nullptr;
//			break;
//		}
//
//	if (lpWideCharStr != nullptr)
//		{
//		while (cchT--)
//			{
//			UCHAR ch;
//
//			ch = static_cast<UCHAR>(*(lpMultiByteStr++));
//			if (ch >= 0x80)
//				{
//				if (pwchConv)
//					*(lpWideCharStr++) = pwchConv[ch - 0x80];
//				else
//					{
//					std::abort(); // SetLastError(ERROR_INVALID_PARAMETER); 
//					return(0);
//				}
//				}
//			else
//				*(lpWideCharStr++) = (WCHAR)MakeWordFirstSecond(0, ch);
//			}
//		}
//
//	return(cch);
}

/**
	MsoWideCharToMultiByte

	This function provides our own layer to WideCharToMultiByte() call.  We
	provide our own lookup table if it's not available from NLS subsystem.
	Currently works for 20 SBCS Win and 5 Mac code pages only.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "Historical annotation issue")
int MsoWideCharToMultiByte(UINT CodePage, DWORD dwFlags, 
	LPCWCH lpWideChar, int cchWideChar,
	LPSTR lpMultiByte, int cchMultiByte32,
	LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar) noexcept
{
	// GAMMA::TODO:: Implement uwsing std::wcstombs
	std::abort();
//	int cch, cchT;
//	const WCHAR *pwchConv = nullptr;
//	static UINT CodePageLast = CP_ACP;
//	static BOOL fValidCodePageLast = true;
//	BOOL fValidCodePage = false;
//	if (lpMultiByte)
//		DebugFill(lpMultiByte, static_cast<size_t>(cchMultiByte32));
//
//	// PERF: calling IsValidCodePage can be about a 2 billion cycle operation.
//	//   let's optimize that out in the case where someone calls MsoWideCharToMultiByte
//	//   multiple times with the same code page.
//	if (CodePage == CP_ACP)
//		fValidCodePage = true;
//	else
//		{
//		if (CodePage != CodePageLast)
//			{
//			fValidCodePageLast = IsValidCodePage(CodePage);
//			CodePageLast = CodePage;
//			}
//		fValidCodePage = fValidCodePageLast;
//		}
//
//	AssertTag(lpWideChar != NULL, 0x0010a240 /* tag_aekja */);
//
//	// Win32 docs state that these must be different buffers
//	AssertTag((char *)lpWideChar != (char *)lpMultiByte, 0x0010a241 /* tag_aekjb */);
//
//	if (lpUsedDefaultChar)
//		*lpUsedDefaultChar = false;
//
//	if (cchWideChar < 0)
//		cch = MsoCchWzLen(lpWideChar) + 1;  // include null terminator
//	else
//		cch = cchWideChar;
//
//	if (CodePage == CP_ACP || CodePage == CP_OEMCP || fValidCodePage)
//		{
//		LPCWSTR lpwch;
//		LPCWSTR lpwchLim;
//		int fSysCall = true;
//		int cchOut = 0;
//
//		if (cchMultiByte32 > 0 && CodePage!=CP_UTF8)
//			{
//			for (lpwch = lpWideChar, lpwchLim = lpwch + cch;
//				lpwch < lpwchLim;
//				lpwch++)
//				{
//				if (*lpwch >= 0x80 && *lpwch <= 0x9f)
//					{
//					fSysCall = false;
//					break;
//					}
//				}
//			}
//		if (fSysCall)
//			{
//			return(WideCharToMultiByte(CodePage, dwFlags, lpWideChar,
//					cchWideChar, lpMultiByte, cchMultiByte32, lpDefaultChar,
//					lpUsedDefaultChar));
//			}
//
//		if (CodePage == CP_UTF8)
//			return(UnicodeToUTF8(lpWideChar, cch, lpMultiByte, cchMultiByte32));
//
//		while (cch--)
//			{
//			WCHAR wch;
//			BYTE rgch[4];
//			BOOL fDef;
//			int cchMB;
//
//			wch = *(lpWideChar++);
//			if (wch >= 0x80 && wch <= 0x9f)
//				{
//				if (cchOut + 1 > cchMultiByte32)
//					return(0);
//				if (lpUsedDefaultChar)
//					*lpUsedDefaultChar = true;
//				if (lpMultiByte)
//					*(lpMultiByte++) = static_cast<CHAR>(LOBYTE(wch));
//				cchOut++;
//				}
//			else
//				{
//				cchMB = WideCharToMultiByte(CodePage, dwFlags, &wch, 1, (CHAR *)rgch, sizeof(rgch), lpDefaultChar, &fDef);
//				if (cchOut+cchMB > cchMultiByte32)
//					return(0);
//				cchOut += cchMB;
//				if (cchMultiByte32)
//					{
//					if (lpMultiByte)
//						{
//						// it should be safe to use memcpy instead of memmove here because rgch is a local buffer
//						// that can't overlap with the lpMultiByte param
//						memcpy(lpMultiByte, rgch, static_cast<size_t>(cchMB));
//						lpMultiByte += cchMB;
//						}
//					}
//				if (lpUsedDefaultChar)
//					*lpUsedDefaultChar |= fDef;
//				}
//			}
//		return(cchOut);
//		}
//
//	// Special handling for UTF8
//	if (CodePage == CP_UTF8)
//		return(UnicodeToUTF8(lpWideChar, cch, lpMultiByte, cchMultiByte32));
//
//	if (cchMultiByte32 == 0)
//		return(cch);
//	if (cch > cchMultiByte32)
//		return(0);
//	cchT = cch;
//
//	switch (CodePage)
//		{
//		case CP_EASTEUROPE:
//			pwchConv = mpwchConv1250;
//			break;
//		case CP_RUSSIAN:
//			pwchConv = mpwchConv1251;
//			break;
//		case CP_WESTEUROPE:
//			pwchConv = mpwchConv1252;
//			break;
//		case CP_GREEK:
//			pwchConv = mpwchConv1253;
//			break;
//		case CP_TURKISH:
//			pwchConv = mpwchConv1254;
//			break;
//		case CP_HEBREW:
//			pwchConv = mpwchConv1255;
//			break;
//		case CP_ARABIC:
//			pwchConv = mpwchConv1256;
//			break;
//		case CP_BALTIC:
//			pwchConv = mpwchConv1257;
//			break;
//		case CP_VIETNAMESE:
//			pwchConv = mpwchConv1258;
//			break;
//		case CP_RUSSIANKOI8R:
//			pwchConv = mpwchConv20866;
//			break;
//		case CP_ISOLATIN1:
//			pwchConv = mpwchConv28591;
//			break;
//		case CP_ISOEASTEUROPE:
//			pwchConv = mpwchConv28592;
//			break;
//		case CP_ISOTURKISH:
//			pwchConv = mpwchConv28593;
//			break;
//		case CP_ISOBALTIC:
//			pwchConv = mpwchConv28594;
//			break;
//		case CP_ISORUSSIAN:
//			pwchConv = mpwchConv28595;
//			break;
//		case CP_ISOARABIC:
//			pwchConv = mpwchConv28596;
//			break;
//		case CP_ISOGREEK:
//			pwchConv = mpwchConv28597;
//			break;
//		case CP_ISOHEBREW:
//			pwchConv = mpwchConv28598;
//			break;
//		case CP_ISOTURKISH2:
//			pwchConv = mpwchConv28599;
//			break;
//		case CP_ISOLATIN9:
//			pwchConv = mpwchConv28605;
//			break;
//		case CP_MACCP:
//		case CP_MAC_ROMAN:
//			pwchConv = mpwchConv10000;
//			break;
//		case CP_MAC_GREEK:
//			pwchConv = mpwchConv10006;
//			break;
//		case CP_MAC_CYRILLIC:
//			pwchConv = mpwchConv10007;
//			break;
//		case CP_MAC_LATIN2:
//			pwchConv = mpwchConv10029;
//			break;
//		case CP_MAC_TURKISH:
//			pwchConv = mpwchConv10081;
//			break;
//		case CP_SYMBOL:
//			if (lpMultiByte)
//				{
//				while (cchT--)
//					*(lpMultiByte++) = (CHAR)(*(lpWideChar++) & 0xFF);
//				}
//			return(cch);
////SOUTHASIA
//        case CP_THAI:
//            pwchConv = mpwchConv874;
//            break;
//// SOUTHASIA
//		default:
//			// code page not supported, but if all ASCII then convert anyway.
//			if (lpMultiByte)
//				{
//				while (cchT--)
//					{
//					WCHAR wch;
//
//					wch = *(lpWideChar++);
//					if (wch < 0x80)
//						*(lpMultiByte++) = (char)wch;
//					else
//						return(0);
//					}
//				}
//			return(cch);
//			break;
//		}
//
//	if (lpMultiByte)
//		{
//		while (cchT--)
//			{
//			WCHAR wch;
//			BYTE ch = 0;
//
//			wch = *(lpWideChar++);
//			if (wch < 0x80)
//				*(lpMultiByte++) = (char)wch;
//			else if (wch >= 0x80 && wch <= 0x9f)
//				{
//				if (lpUsedDefaultChar)
//					*lpUsedDefaultChar = true;
//				*(lpMultiByte++) = static_cast<CHAR>(LOBYTE(wch));
//				}
//			else
//				{
//				int ich;
//
//				// REVIEW nobuyah: very inefficient; speed up later
//					{
//					for (ich = 0; ich < 128; ich++)
//						{
//						if (pwchConv[ich] == wch)
//							{
//							ch = (BYTE)(ich + 128);
//							break;
//							}
//						}
//					}
//				if (ch == 0)
//					{
//					if (lpUsedDefaultChar)
//						*lpUsedDefaultChar = true;
//					ch = static_cast<BYTE>(lpDefaultChar ? *lpDefaultChar : '?');
//					}
//				*(lpMultiByte++) = static_cast<CHAR>(ch);
//				}
//			}
//		}
//
//	return(cch);
}

/**
	UTF8ToUnicode

	Maps a UTF-8 character string to its wide character string counterpart.
	Created 02-06-96 by JulieB. Surrogate pairs support added by WalterPu.
	See UnicodeToUTF8 comment for info on Unicode surrogate pairs.

	UTF8 (0zzzzzzz)                   -> Unicode (00000000 0zzzzzzz)
	UTF8 (110xxxyy 10zzzzzz)          -> Unicode (00000xxx yyzzzzzz)
	UTF8 (1110wwww 10xxxxyy 10zzzzzz) -> Unicode (wwwwxxxx yyzzzzzz)

	UTF8 (11110bbb 10bbvvvv 10wwxxyy 10zzzzzz) ->
		Unicode pair (110110aa aavvvvww) (110111xx yyzzzzzz)
		where aaaa == bbbbb - 1
*/
int UTF8ToUnicode(_In_count_(*pcchSrc) const char* lpSrcStr, _Inout_ int *pcchSrc, _Out_writes_to_opt_(cchDest, return) wchar_t* lpDestStr, int cchDest) noexcept
{
	LPCSTR pb = lpSrcStr, pbMax;
	LPWSTR pw = lpDestStr;
	LPCWSTR pwMax;

	VerifyElseCrashTag(lpSrcStr != nullptr, 0x0034c18c /* tag_anmgm */);
	VerifyElseCrashTag(pcchSrc != nullptr, 0x0010a242 /* tag_aekjc */);
	VerifyElseCrashTag(*pcchSrc >= 0 && cchDest >= 0, 0x0048d3d5 /* tag_asnpv */);
	VerifyElseCrashTag(lpDestStr != nullptr || cchDest == 0, 0x0048d3d6 /* tag_asnpw */);

	if (cchDest > 0)
		{
		DebugFill(lpDestStr, cchDest*sizeof(WCHAR));
		}

	pbMax = lpSrcStr + *pcchSrc;
	pwMax = lpDestStr + cchDest;
	while (pb < pbMax && (pw < pwMax || cchDest == 0))
		{
		if (BIT7(*pb) == 0)
			{
			// Found ASCII.
			if (cchDest)
				{
				OACR_ASSUME(pw);
				*pw = (WCHAR)(*pb);
				}
			pw++;
			pb++;
			}
		else if (HIGH_3_MASK(*pb) == 0xc0)
			{
			// Found 2 byte sequence.
			if (pb + 1 >= pbMax)
				break;
			//AssertSz(HIGH_2_MASK(*(pb + 1)) == 0x80, "Bad trail byte in 2 byte sequence");
			if (cchDest)
				*pw = (WCHAR)((LOWER_5_BIT(*pb) << 6) | LOWER_6_BIT(*(pb + 1)));
			pw++;
			pb += 2;
			}
		else if (HIGH_4_MASK(*pb) == 0xe0)
			{
			// Found 3 byte sequence.
			if (pb + 2 >= pbMax)
				break;
			//AssertSz(HIGH_2_MASK(*(pb + 1)) == 0x80, "Bad trail byte #1 in 3 byte sequence");
			//AssertSz(HIGH_2_MASK(*(pb + 2)) == 0x80, "Bad trail byte #2 in 3 byte sequence");
			if (cchDest)
				{
				*pw = (WCHAR)((LOWER_4_BIT(*pb) << 12) | (LOWER_6_BIT(*(pb + 1)) << 6) | LOWER_6_BIT(*(pb + 2)));
				}
			pw++;
			pb += 3;
			}
		else if (HIGH_5_MASK(*pb) == 0xf0)
			{
			// Found 4 byte sequence forming surrogate pair.
			if (pb + 3 >= pbMax)
				break;
			//AssertSz(HIGH_2_MASK(*(pb + 1)) == 0x80, "Bad trail byte #1 in 4 byte sequence");
			//AssertSz(HIGH_2_MASK(*(pb + 2)) == 0x80, "Bad trail byte #2 in 4 byte sequence");
			//AssertSz(HIGH_2_MASK(*(pb + 3)) == 0x80, "Bad trail byte #3 in 4 byte sequence");
			if (cchDest)
				{
				if (pw + 1 >= pwMax)
				{
					// GAMMA
					std::abort();
					// SetLastError(ERROR_INSUFFICIENT_BUFFER);
					return(0);
				}
				int wPlane = (((*pb & 7) << 2) | ((*(pb + 1) & 0x30) >> 4)) - 1;
				//AssertSz(wPlane < 0x10000, "Bit plane too big");
				*pw = (WCHAR)(UCS2_1ST_OF_2 | (wPlane << 6) |
					(LOWER_4_BIT(*(pb + 1)) << 2) | ((*(pb + 2) & 0x30) >> 4));
				*(pw + 1) = (WCHAR)(UCS2_2ND_OF_2 |
					(LOWER_4_BIT(*(pb + 2)) << 6) | LOWER_6_BIT(*(pb + 3)));
				}
			pw += 2;
			pb += 4;
			}
		else
			{
			//AssertSz(false, "Bad start byte");
			pb++;
			}
		}

	// Make sure the destination buffer was large enough.
	if (cchDest && pw >= pwMax && pb < pbMax)
	{
		// SetLastError(ERROR_INSUFFICIENT_BUFFER);
		// GAMMA
		std::abort();
		return(0);
	}
	*pcchSrc = (int)(pb - lpSrcStr);
	// return SafeInt<size_t>(pw - lpDestStr);
	return static_cast<size_t>(pw - lpDestStr);
}


/**
	UnicodeToUTF8Core

	Maps a Unicode character string to its UTF-8 string counterpart. Created
	02-06-96 by JulieB. Unicode surrogate pairs support added by WalterPu.

	Unicode (00000000 0zzzzzzz) -> UTF8 (0zzzzzzz)
	Unicode (00000xxx yyzzzzzz) -> UTF8 (110xxxyy 10zzzzzz)
	Unicode (wwwwxxxx yyzzzzzz) -> UTF8 (1110wwww 10xxxxyy 10zzzzzz)

	Unicode pair (110110aa aavvvvww) (110111xx yyzzzzzz) ->
		UTF8 (11110bbb 10bbvvvv 10wwxxyy 10zzzzzz) where bbbbb == aaaa + 1

	Note: A Unicode surrogate pair is where two Unicode words are used to
	represent one 32 bit UCS-4 value. The UCS-4 range that may be represented
	as a surrogate pair is from 0x00010000 to 0x0010ffff. We don't convert
	between UCS-4 and Unicode surrogate pairs here, only between existing
	surrogate pairs and UTF-8. For informational purposes only:

	UCS-4 (00000000 000bbbbb xxxxxxyy zzzzzzzz) ->
		Unicode (110110aa aaxxxxxx) (110111yy zzzzzzzz) where aaaa == bbbbb - 1.
*/
int UnicodeToUTF8Core(const wchar_t* lpSrcStr, int cchSrc, int *pcchSrcLeft, _Out_opt_cap_(cchDest) char* lpDestStr, int cchDest) noexcept
{
	LPCWSTR lpWC = lpSrcStr;
	int cchU8 = 0;           // Number of UTF8 bytes generated
	int wPlane;

	VerifyElseCrashTag(lpSrcStr != nullptr, 0x01659659 /* tag_bzzzz */);

	DebugFill(lpDestStr, static_cast<size_t>(cchDest));

	while (cchSrc && (cchU8 < cchDest || cchDest == 0))
		{
		OACR_ASSUME(lpDestStr != nullptr || cchDest == 0);

		if (*lpWC <= ASCII)
			{
			// Generate 1 byte sequence (ASCII) if <= 0x7F (7 bits).
			if (cchDest)
				lpDestStr[cchU8] = (CHAR)*lpWC;
			cchU8++;
			lpWC++;
			cchSrc--;
			}
		else if (*lpWC <= UTF8_2_MAX)
			{
			// Generate 2 byte sequence if <= 0x07ff (11 bits).
			if (cchDest)
				{
				if (cchU8 + 2 <= cchDest)
					{
					// Use upper 5 bits in first byte.
					// Use lower 6 bits in second byte.
					lpDestStr[cchU8++] = (CHAR)(UTF8_1ST_OF_2 | (*lpWC >> 6));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | LOWER_6_BIT(*lpWC));
					}
				else
					{
					// Error - buffer too small.
					break;
					}
				}
			else
				{
				cchU8 += 2;
				}
			lpWC++;
			cchSrc--;
			}
		else if (HIGH_6_MASK(*lpWC) != UCS2_1ST_OF_2 || (cchSrc >= 2 ?
			HIGH_6_MASK(*(lpWC + 1)) != UCS2_2ND_OF_2 : pcchSrcLeft == nullptr))
			{
			// Generate 3 byte sequence.
			if (cchDest)
				{
				if (cchU8 + 3 <= cchDest)
					{
					// Use upper  4 bits in first byte.
					// Use middle 6 bits in second byte.
					// Use lower  6 bits in third byte.
					lpDestStr[cchU8++] = (CHAR)(UTF8_1ST_OF_3 | (*lpWC >> 12));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | MIDDLE_6_BIT(*lpWC));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | LOWER_6_BIT(*lpWC));
					}
				else
					{
					// Error - buffer too small.
					break;
					}
				}
			else
				{
				cchU8 += 3;
				}
			lpWC++;
			cchSrc--;
			}
		else
			{
			// Generate 4 byte sequence involving two source Unicode characters.
			if (cchSrc < 2)
				{
				// Split surrogate pair at end of input buffer.
				AssertTag(pcchSrcLeft != NULL, 0x0010a245 /* tag_aekjf */);
				break;
				}
			AssertTag(HIGH_6_MASK(*lpWC) == UCS2_1ST_OF_2, 0x0010a246 /* tag_aekjg */);
			AssertTag(HIGH_6_MASK(*(lpWC + 1)) == UCS2_2ND_OF_2, 0x0010a247 /* tag_aekjh */);

			if (cchDest)
				{
				if (cchU8 + 4 <= cchDest)
					{
					wPlane = MIDDLE_4_BIT(*lpWC) + 1;
					lpDestStr[cchU8++] = (CHAR)(UTF8_1ST_OF_4 | (wPlane >> 2));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | (LOWER_2_BIT(wPlane) << 4) | MIDLOW_4_BIT(*lpWC));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | (LOWER_2_BIT(*lpWC) << 4) | MIDDLE_4_BIT(*(lpWC + 1)));
					lpDestStr[cchU8++] = (CHAR)(UTF8_TRAIL    | LOWER_6_BIT(*(lpWC + 1)));
					}
				else
					{
					// Error - buffer too small.
					break;
					}
				}
			else
				{
				cchU8 += 4;
				}
			lpWC += 2;
			cchSrc -= 2;
			}
		}

	// Make sure the destination buffer was large enough.
	if (pcchSrcLeft == nullptr)
		{
		if (cchDest && cchSrc > 0)
			{
			// GAMMA
			std::abort();
			// SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return(0);
			}
		}
	else
		{
		AssertTag(cchSrc == 0 || cchSrc == 1, 0x0010a248 /* tag_aekji */);
		*pcchSrcLeft = cchSrc;
		}

	// Return the number of UTF-8 bytes written.
	return(cchU8);
}

/**
	MsoFSurrogatePairToUCS4

	Given two Unicode characters representing a Unicode surrogate pair,
	determine the UCS-4 character corresponding to them. Return false if the
	two Unicode characters don't form a legal surrogate pair.

	Unicode (110110aa aaxxxxxx) (110111yy zzzzzzzz) ->
		UCS-4 (00000000 000bbbbb xxxxxxyy zzzzzzzz) where bbbbb == aaaa + 1
*/
BOOL MsoFSurrogatePairToUCS4(WCHAR w1, WCHAR w2, DWORD *plUCS4) noexcept
{
	AssertTag(plUCS4 != NULL, 0x0010a249 /* tag_aekjj */);
	if (HIGH_6_MASK(w1) != UCS2_1ST_OF_2 || HIGH_6_MASK(w2) != UCS2_2ND_OF_2)
		return(false);
	*plUCS4 = static_cast<DWORD>(((MIDDLE_4_BIT(w1) + 1) << 16) + (LOWER_6_BIT(w1) << 10) + LOWER_10_BIT(w2));
	return(true);
}

/**
	MsoFUCS4ToSurrogatePair

	Given a UCS-4 character, determine the two Unicode characters that
	represent it as a Unicode surrogate pair. Return false if the UCS-4
	character isn't in the range that can be represented by a surrogate pair.

	UCS-4 (00000000 000bbbbb xxxxxxyy zzzzzzzz) ->
		Unicode (110110aa aaxxxxxx) (110111yy zzzzzzzz) where aaaa = bbbbb - 1
*/
BOOL MsoFUCS4ToSurrogatePair(DWORD lUCS4, WCHAR *pw1, WCHAR *pw2) noexcept
{
	AssertTag(pw1 != NULL && pw2 != NULL, 0x0010a24a /* tag_aekjk */);
	if (lUCS4 < 0x00010000L || lUCS4 > 0x0010ffffL)
		return(false);
	*pw1 = (WCHAR)(UCS2_1ST_OF_2 | ((LOWER_5_BIT(lUCS4 >> 16) - 1) << 6) | (HIGH_6_MASK(lUCS4) >> 10));
	*pw2 = (WCHAR)(UCS2_2ND_OF_2 | LOWER_10_BIT(lUCS4));
	return(true);
}

/**
	MsoFConvertCharCode
*/
int MsoFConvertCharCode(const wchar_t* pwtSrc, WCHAR *pwchPrefix, DWORD *pdwval) noexcept
{
	WCHAR wchPrefix = (WCHAR)'+';
	const WCHAR *pwch;
	DWORD dwval = 0;
	int cchSrc;
	int fPrefix = false, fNum = false;
	int cDigit = 0, cDigitMax = 6;
	int fConvert = true;

	// parse source string
	for (pwch = pwtSrc + 1, cchSrc = *pwtSrc; cchSrc--; pwch++)
		{
		if (!fPrefix && !fNum && (*pwch == 'x' || *pwch == 'X'))
			{
			fPrefix = true;
			wchPrefix = 'x';
			cDigitMax = 4;
			continue;
			}
		else if (!fPrefix && !fNum && (*pwch == 'u' || *pwch == 'U'))
			{
			fPrefix = true;
			wchPrefix = 'u';
			continue;
			}
		else if (fPrefix && !fNum && wchPrefix == 'u' && *pwch == '+')
			{
			wchPrefix = '+';
			continue;
			}
		else if (*pwch >= '0' && *pwch <= '9')
			{
			fNum = true;
			if (++cDigit > cDigitMax)
				{
				fConvert = false;
				break;
				}
			dwval = (dwval * 16) + (*pwch - '0');
			}
		else if ((*pwch >= 'a' && *pwch <= 'f') ||
				 (*pwch >= 'A' && *pwch <= 'F'))
			{
			fNum = true;
			if (++cDigit > cDigitMax)
				{
				fConvert = false;
				break;
				}
			dwval = (dwval * 16) + (*pwch - (*pwch >= 'a' ? 'a' : 'A') + 10);
			}
		else
			{
			fConvert = false;
			break;
			}
		}

	// last validity check
	if (fConvert)
		{
		if (cDigit < 2)
			fConvert = false;
		else if (dwval > 0x10FFFF)
			fConvert = false;
		if (fPrefix && wchPrefix != '+' && wchPrefix != 'x')
			fConvert = false;
		}

	if (pwchPrefix != nullptr)
		*pwchPrefix = wchPrefix;

	if (pdwval != nullptr)
		*pdwval = dwval;

	return fConvert;
}

/**
	Toggles character code and a character
	(only converts a character above or equal to ' ')
	This routine assumes pwtDest is big enough to hold the result

	If fExact is true and pwtSrc contains invalid characters,
		it returns false without doing any conversion
	If fExact is false and pwtSrc contains invalid characters,
		it does a reverse conversion for the last character

	Returns false if no conversion took place
*/
int MsoFToggleCharCode(const wchar_t* pwtSrc, wchar_t* pwtDest, int cchDest, int fExact) noexcept
{
	WCHAR wchPrefix = (WCHAR)'+';
	DWORD val = 0;

	// do the conversion
	if (MsoFConvertCharCode(pwtSrc, &wchPrefix, &val))
		{ // convert code to a character
		if (val >= 0x10000)
			{ // UCS-4 character
			if (cchDest < 3)
				return(false);
			*pwtDest++ = 2;
			if (!MsoFUCS4ToSurrogatePair(val, pwtDest, pwtDest + 1) || !MsoFWchHighSurrogate(*pwtDest) ||
				!MsoFWchLowSurrogate(*(pwtDest + 1)))
				return(false);
			}
		else
			{
			WCHAR wchT = (WCHAR)(val & 0xFFFF);
			if (cchDest < 2)
				return(false);
			*pwtDest++ = 1;
			if (wchPrefix == 'x')
				{
				char rgchT[2];
				WCHAR rgwchT[2];
				int cchT;
				if (wchT > 0xFF)
					{
					rgchT[0] = static_cast<char>(HIBYTE(wchT));
					rgchT[1] = static_cast<char>(LOBYTE(wchT));
					cchT = 2;
					}
				else
					{
					rgchT[0] = static_cast<char>(LOBYTE(wchT));
					cchT = 1;
					}
				if (MsoMultiByteToWideChar(Mso::Platform::MsoGetACP(), 0, rgchT, cchT, rgwchT, 2) != 1)
					{ // didn't exist in local code page - nothing happens
					return(false);
					}
				wchT = rgwchT[0];
				}

			if (wchT < ' ')
				return(false);
			*pwtDest++ = wchT;
			}
		}
	else
		{ // convert the last character to code
		DWORD value;
		DWORD iHex, iT;
		int fSupressZero = false;

		MsoWtCopy(pwtSrc, pwtDest, cchDest);
		WCHAR *pwchOut = pwtDest + *pwtDest;
		cchDest -= *pwtDest;

		if (*pwtSrc >= 2 &&
			MsoFWchHighSurrogate(*(pwchOut - 1)) && MsoFWchLowSurrogate(*pwchOut))
			{
			if ((fExact && *pwtSrc > 2) || !MsoFSurrogatePairToUCS4(*(pwchOut - 1), *pwchOut, &value))
				return(false);
			pwchOut--;
			cchDest++;
			iHex = 0x100000;
			fSupressZero = true;
			}
		else
			{
			if (fExact && *pwtSrc > 1)
				return(false);
			iHex = 0x1000;
			value = *pwchOut;
			if (value < ' ')
				return(false);
			if (wchPrefix == 'x' && *pwtDest == 2)
				{
				int fDefault;
				char rgchT[2];
				WCHAR rgwchT[2];
				rgwchT[0] = LOWORD(value);
				if (MsoWideCharToMultiByte(Mso::Platform::MsoGetACP(), 0, rgwchT, 1, rgchT, 2, nullptr, &fDefault) == 1)
					{
					value = (DWORD)rgchT[0];
					iHex = 0x10; // only output 2 digits
					}
				else
					value = (DWORD)MakeWordFirstSecond(rgchT[0], rgchT[1]);
				if (fDefault)
					return(false);
				}
			}
		do
			{
			if (cchDest-- <= 0)
				return(false);
			*pwchOut++ = (WCHAR)c_rgchHex[iT = (value / iHex)];
			if (fSupressZero)
				{
				if (iT == 0)
					{
					pwchOut--;
					cchDest++;
					}
				else
					fSupressZero = false;
				}
			value = (DWORD)(value % iHex);
			iHex /= 16;
			} while (iHex != 0);
		*pwtDest = (WCHAR)(pwchOut - pwtDest - 1);
		}

	return(true);
}

MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToUpperLid(WCHAR wch, LID lid, WORD wflags) noexcept
{
	if (wflags & msoStrip && (ilLangBase(lid) == PRIMARYLANGID(lidFrench) || ilLangBase(lid) == PRIMARYLANGID(lidGreek)))
	{
		WCHAR wchNoAccent;
		WCHAR wchUpper;

		wchNoAccent = MsoWchStripAccent(wch);
		wchUpper = MsoWchToUpperLid(wchNoAccent, lid, 0);
		if (wchNoAccent == wchUpper)
			return(wch);
		return(wchUpper);
	}

	const WCHAR *pTbl = &adjUpper[3 - 3];
	do pTbl += 3; while (wch > pTbl[1 - 3]);
	if (wch >= pTbl[0 - 3])
	{
		wch = (WCHAR)(wch - pTbl[2 - 3]);
		if ((short)pTbl[2 - 3] > 0x00FF)		// signed compare
			wch = mpccToUpper[wch];
		else if (pTbl[2 - 3] <= 1)			// unsigned compare
		{
			wch &= ~1;
			wch = (WCHAR)(wch + pTbl[2 - 3]);
		}
		else if ((wch == 0x0069 - 0x20) && ((ilLangBase(lid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lid) == PRIMARYLANGID(lidAzeriLatin))))
			wch = 0x0130;
	}
	return(wch);
}

WCHAR Mso::StringCore::WchToUpperCultureTag(WCHAR wch, _In_z_ const WCHAR *wzTag, WORD wflags) noexcept
{
	WCHAR parentCultureTag[LOCALE_NAME_MAX_LENGTH];

	// Get the parent culture
	GetParentCultureTag(wzTag, parentCultureTag, _countof(parentCultureTag));

	OACR_WARNING_SUPPRESS(AVOID_CRT_COMPARESTRING, "longstanding issue")
	if (wflags & msoStrip && (_wcsicmp(parentCultureTag, L"fr") == 0 || _wcsicmp(parentCultureTag, L"el") == 0))
	{
		WCHAR wchNoAccent;
		WCHAR wchUpper;

		wchNoAccent = MsoWchStripAccent(wch);
		wchUpper = Mso::StringCore::WchToUpperCultureTag(wchNoAccent, wzTag, 0);
		if (wchNoAccent == wchUpper)
			return(wch);
		return(wchUpper);
	}

	const WCHAR *pTbl = &adjUpper[3 - 3];
	do pTbl += 3; while (wch > pTbl[1 - 3]);
	if (wch >= pTbl[0 - 3])
	{
		wch = (WCHAR)(wch - pTbl[2 - 3]);
		if ((short)pTbl[2 - 3] > 0x00FF)		// signed compare
			wch = mpccToUpper[wch];
		else if (pTbl[2 - 3] <= 1)			// unsigned compare
		{
			wch &= ~1;
			wch = (WCHAR)(wch + pTbl[2 - 3]);
		}
		else if ((wch == 0x0069 - 0x20) && IsCultureTagParentTurkishOrAzeri(wzTag))
			wch = 0x0130;
	}
	return(wch);
}

MSODLL_MANDATED_C_EXPORT WCHAR MsoWchToLowerLid(WCHAR wch, LID lid, WORD /*wflags*/) noexcept
{
	const WCHAR *pTbl = &adjLower[3 - 3];
	do pTbl += 3; while (wch > pTbl[1 - 3]);
	if (wch >= pTbl[0 - 3])
	{
		wch = (WCHAR)(wch + pTbl[2 - 3]);
		if ((short)pTbl[2 - 3] < 0)
			wch = mpccToLower[wch];
		else if ((wch == 0x0049 + 0x20) && ((ilLangBase(lid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lid) == PRIMARYLANGID(lidAzeriLatin))))
			wch = 0x0131;
	}
	return(wch);
}

WCHAR Mso::StringCore::WchToLowerCultureTag(WCHAR wch, _In_z_ const WCHAR *wzTag) noexcept
{
	const WCHAR *pTbl = &adjLower[3 - 3];
	do pTbl += 3; while (wch > pTbl[1 - 3]);
	if (wch >= pTbl[0 - 3])
	{
		wch = (WCHAR)(wch + pTbl[2 - 3]);
		if ((short)pTbl[2 - 3] < 0)
			wch = mpccToLower[wch];
		else if ((wch == 0x0049 + 0x20) && IsCultureTagParentTurkishOrAzeri(wzTag))
			wch = 0x0131;
	}
	return(wch);
}

void MsoSzUpperCore(CHAR *sz, IMsoMemHeap *pmmh, LCID lcid) noexcept
{
    std::abort();
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<wchar_t *> wz(MsoWzMarkSzCore(sz, pmmh), pmmh);
//	if (wz != nullptr)
//	{
//		MsoWzUpperCore(wz, lcid);
//		MsoWzToSzCore(wz, sz, MsoCchSzLen(sz) + 1, pmmh);
//	}
}

void Mso::StringCore::SzUpperCultureTag(_Inout_z_ CHAR *sz, _In_z_ const WCHAR *wzTag, _In_opt_ IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<wchar_t *> wz(MsoWzMarkSzCore(sz, pmmh), pmmh);
//	if (wz != nullptr)
//	{
//		Mso::StringCore::WzUpperCultureTag(wz, wzTag);
//		MsoWzToSzCore(wz, sz, MsoCchSzLen(sz) + 1, pmmh);
//	}
}

static void SurrogatePairUpperCase(const WCHAR* wchLeading, WCHAR* wchTrailing) noexcept
{
	if (!wchLeading || !wchTrailing)
		return;
	if (0xD801 == *wchLeading)
	{
		if (*wchTrailing >= 0xDC28 && *wchTrailing <= 0xDC4F)
		{
			*wchTrailing -= 0x0028; // DESERET script
		}
		else if (*wchTrailing >= 0xDCD8 && *wchTrailing <= 0xDCFB)
		{
			*wchTrailing -= 0x0028; // OSAGE script
		}
	}
	else if (0xD803 == *wchLeading)
	{
		if (*wchTrailing >= 0xDCC0 && *wchTrailing <= 0xDCF2)
		{
			*wchTrailing -= 0x0040; // OLD HUNGARIAN script
		}
	}
	else if (0xD806 == *wchLeading)
	{
		if (*wchTrailing >= 0xDCC0 && *wchTrailing <= 0xDCDF)
		{
			*wchTrailing -= 0x0020; // WARANG CITI script
		}
	}
	else if (0xD81B == *wchLeading)
	{
		if (*wchTrailing >= 0xDE60 && *wchTrailing <= 0xDE7F)
		{
			*wchTrailing -= 0x0020; // MEDEFAIDRIN script
		}
	}
	else if (0xD83A == *wchLeading)
	{
		if (*wchTrailing >= 0xDD22 && *wchTrailing <= 0xDD43)
		{
			*wchTrailing -= 0x0022; // ADLAM script
		}
	}
}

static void SurrogatePairLowerCase(const WCHAR* wchLeading, WCHAR* wchTrailing) noexcept
{
	if (!wchLeading || !wchTrailing)
		return;
	if (0xD801 == *wchLeading)
	{
		if (*wchTrailing >= 0xDC00 && *wchTrailing <= 0xDC27)
		{
			*wchTrailing += 0x0028; // DESERET script
		}
		else if (*wchTrailing >= 0xDCB0 && *wchTrailing <= 0xDCD3)
		{
			*wchTrailing += 0x0028; // OSAGE script
		}
	}
	else if (0xD803 == *wchLeading)
	{
		if (*wchTrailing >= 0xDC80 && *wchTrailing <= 0xDCB2)
		{
			*wchTrailing += 0x0040; // OLD HUNGARIAN script
		}
	}
	else if (0xD806 == *wchLeading)
	{
		if (*wchTrailing >= 0xDCA0 && *wchTrailing <= 0xDCBF)
		{
			*wchTrailing += 0x0020; // WARANG CITI script
		}
	}
	else if (0xD81B == *wchLeading)
	{
		if (*wchTrailing >= 0xDE40 && *wchTrailing <= 0xDE5F)
		{
			*wchTrailing += 0x0020; // MEDEFAIDRIN script
		}
	}
	else if (0xD83A == *wchLeading)
	{
		if (*wchTrailing >= 0xDD00 && *wchTrailing <= 0xDD21)
		{
			*wchTrailing += 0x0022; // ADLAM script
		}
	}
}

TASKLIBTECHDEBT_(void) MsoWzUpperCore(WCHAR * wz, LCID lcid) noexcept
{
	bool isTurkishOrAzeri = (ilLangBase(lcid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lcid) == PRIMARYLANGID(lidAzeriLatin));
	WCHAR wchLeadingSurrogate = 0;
	if (isTurkishOrAzeri)
	{
		for (; *wz; wz++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, wz);
				wchLeadingSurrogate = 0;
			}
			else if (*wz >= 0xD800 && *wz <= 0xDBFF)
			{
				wchLeadingSurrogate = *wz;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*wz, 'a', 'z') && *wz != 0x0069)
					*wz -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*wz, 'A', 'Z'))
						*wz = MsoWchToUpperCore(*wz, lcid);
				}
			}
		}
	}
	else
	{
		for (; *wz; wz++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, wz);
				wchLeadingSurrogate = 0;
			}
			else if (*wz >= 0xD800 && *wz <= 0xDBFF)
			{
				wchLeadingSurrogate = *wz;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*wz, 'a', 'z'))
					*wz -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*wz, 'A', 'Z'))
						// Do it the hard way
						*wz = MsoWchToUpperCore(*wz, lcid);
				}
			}
		}
	}
}

TASKLIBTECHDEBT_(void) MsoPwchUpperCore(_Inout_count_(cch) WCHAR *pwch, int cch, LCID lcid) noexcept
{
	if (0 >= cch)
	{
		AssertTag(0 == cch, 0x0010a24b /* tag_aekjl */);
		return;
	}

	bool isTurkishOrAzeri = (ilLangBase(lcid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lcid) == PRIMARYLANGID(lidAzeriLatin));
	const WCHAR *pwchEnd = pwch + cch - 1;
	WCHAR wchLeadingSurrogate = 0;

	if (isTurkishOrAzeri)
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z') && *pwch != 0x0069)
					*pwch -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
						*pwch = MsoWchToUpperCore(*pwch, lcid);
				}
			}
		}
	}
	else
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
					*pwch -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
						// Do it the hard way
						*pwch = MsoWchToUpperCore(*pwch, lcid);
				}
			}
		}
	}
}

void Mso::StringCore::PwchUpperCultureTag(_Inout_count_(cch) WCHAR *pwch, int cch, _In_z_ const WCHAR *wzTag) noexcept
{
	if (0 >= cch)
	{
		AssertTag(0 == cch, 0x00706311 /* tag_a2gmr */);
		return;
	}

	const WCHAR *pwchEnd = pwch + cch - 1;
	WCHAR wchLeadingSurrogate = 0;

	if (IsCultureTagParentTurkishOrAzeri(wzTag))
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z') && *pwch != 0x0069)
					*pwch -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
						*pwch = Mso::StringCore::WchToUpperCultureTag(*pwch, wzTag, 0);
				}
			}
		}
	}
	else
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairUpperCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
					*pwch -= 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
						// Do it the hard way
						*pwch = Mso::StringCore::WchToUpperCultureTag(*pwch, wzTag, 0);
				}
			}
		}
	}
}

void MsoSzLowerCore(CHAR *sz, IMsoMemHeap *pmmh, LCID lcid) noexcept
{
    std::abort();
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<wchar_t *> wz(MsoWzMarkSzCore(sz, pmmh), pmmh);
//	if (wz != nullptr)
//	{
//		MsoWzLowerCore(wz, lcid);
//		MsoWzToSzCore(wz, sz, MsoCchSzLen(sz) + 1, pmmh);
//	}
}

void Mso::StringCore::SzLowerCultureTag(_Inout_z_ CHAR *sz, _In_z_ const WCHAR *wzTag, _In_opt_ IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<wchar_t *> wz(MsoWzMarkSzCore(sz, pmmh), pmmh);
//	if (wz != nullptr)
//	{
//		Mso::StringCore::WzLowerCultureTag(wz, wzTag);
//		MsoWzToSzCore(wz, sz, MsoCchSzLen(sz) + 1, pmmh);
//	}
}

TASKLIBTECHDEBT_(void) MsoWzLowerCore(WCHAR *wz, LCID lcid) noexcept
{
	bool isTurkishOrAzeri = (ilLangBase(lcid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lcid) == PRIMARYLANGID(lidAzeriLatin));
	WCHAR wchLeadingSurrogate = 0;
	if (isTurkishOrAzeri)
	{
		for (; *wz; wz++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, wz);
				wchLeadingSurrogate = 0;
			}
			else if (*wz >= 0xD800 && *wz <= 0xDBFF)
			{
				wchLeadingSurrogate = *wz;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*wz, 'A', 'Z') && *wz != 0x0049)
					*wz += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*wz, 'a', 'z'))
						*wz = MsoWchToLowerCore(*wz, lcid);
				}
			}
		}
	}
	else
	{
		for (; *wz; wz++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, wz);
				wchLeadingSurrogate = 0;
			}
			else if (*wz >= 0xD800 && *wz <= 0xDBFF)
			{
				wchLeadingSurrogate = *wz;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*wz, 'A', 'Z'))
					*wz += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*wz, 'a', 'z'))
						// Do it the hard way
						*wz = MsoWchToLowerCore(*wz, lcid);
				}
			}
		}
	}
}

TASKLIBTECHDEBT_(void) MsoPwchLowerCore(_Inout_count_(cch) WCHAR *pwch, int cch, LCID lcid) noexcept
{
	if (0 >= cch)
	{
		AssertTag(0 == cch, 0x0010a24c /* tag_aekjm */);
		return;
	}

	bool isTurkishOrAzeri = (ilLangBase(lcid) == PRIMARYLANGID(lidTurkish)) || (ilLangBase(lcid) == PRIMARYLANGID(lidAzeriLatin));
	const WCHAR *pwchEnd = pwch + cch - 1;
	WCHAR wchLeadingSurrogate = 0;

	if (isTurkishOrAzeri)
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z') && *pwch != 0x0049)
					*pwch += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
						*pwch = MsoWchToLowerCore(*pwch, lcid);
				}
			}
		}
	}
	else
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
					*pwch += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
						// Do it the hard way
						*pwch = MsoWchToLowerCore(*pwch, lcid);
				}
			}
		}
	}
}

void Mso::StringCore::PwchLowerCultureTag(_Inout_count_(cch) WCHAR *pwch, int cch, _In_z_ const WCHAR *wzTag) noexcept
{
	if (0 >= cch)
	{
		AssertTag(0 == cch, 0x00706312 /* tag_a2gms */);
		return;
	}

	const WCHAR *pwchEnd = pwch + cch - 1;
	WCHAR wchLeadingSurrogate = 0;

	if (IsCultureTagParentTurkishOrAzeri(wzTag))
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z') && *pwch != 0x0049)
					*pwch += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
						*pwch = Mso::StringCore::WchToLowerCultureTag(*pwch, wzTag);
				}
			}
		}
	}
	else
	{
		for (; pwch <= pwchEnd; pwch++)
		{
			if (wchLeadingSurrogate)
			{
				SurrogatePairLowerCase(&wchLeadingSurrogate, pwch);
				wchLeadingSurrogate = 0;
			}
			else if (*pwch >= 0xD800 && *pwch <= 0xDBFF)
			{
				wchLeadingSurrogate = *pwch;
			}
			else
			{
				if (Mso::StringTraits::Details::FBetween(*pwch, 'A', 'Z'))
					*pwch += 'a' - 'A';
				else
				{
					if (!Mso::StringTraits::Details::FBetween(*pwch, 'a', 'z'))
						// Do it the hard way
						*pwch = Mso::StringCore::WchToLowerCultureTag(*pwch, wzTag);
				}
			}
		}
	}
}

WCHAR MsoWchToUpperInvariant(WCHAR wch) noexcept
{
	return(Mso::StringCore::WchToUpperCultureTag(wch, L"", 0));
}

/**
	MsoSzToWzSimple

	Convert to Unicode assuming the sz contains only characters < 128
*/
void MsoSzToWzSimple(const CHAR *sz, WCHAR *wz, int cchMax) noexcept
{
	AssertTag(FAllSimpleSz(sz), 0x0010a24d /* tag_aekjn */);
	while (*sz && cchMax > 1)
		{
		*wz++ = (WCHAR)*sz++;
		cchMax--;
		}
	if (cchMax > 0)
		*wz = (WCHAR) NULL;			// copy the NULL
}

/**
*/
void MsoWzToSzSimple(const WCHAR *wz, CHAR *sz, int cchMax) noexcept
{
	AssertTag(FAllSimpleWz(wz), 0x0010a24e /* tag_aekjo */);
	AssertTag(sz || 0 == cchMax, 0x0010a24f /* tag_aekjp */);
	if (sz)
   	{
		while (*wz && cchMax > 1)
			{
			AssertTag(!(*wz & 0xFF80), 0x0010a250 /* tag_aekjq */);
			*sz++ = (CHAR)*wz++;
			cchMax--;
			}
		if (cchMax > 0)
			*sz = 0;
   	}
}

/**
	MsoWzToSzSimpleInPlace
*/
int MsoWzToSzSimpleInPlace(_Inout_z_ WCHAR *wz) noexcept
{
	AssertTag(FAllSimpleWz(wz), 0x0010a251 /* tag_aekjr */);
	PSTR sz = (PSTR) wz;
	PCSTR szStart = sz;

	OACR_WARNING_SUPPRESS(STRCPY_LOOP, "[DanJump] This is safe because it is in-place");
	while (*wz)
		*sz++ = (CHAR) *wz++;
	*sz = '\0';
	// return SafeInt<size_t>(sz - szStart);
    return static_cast<size_t>(sz - szStart);
}

/**
	MsoRgwchToRgchSimple

	Converts the Unicode array of charaters in rgwchFrom with length cchFrom
	into an Ansi array of charaters in rgchTo.  Assumes rgwchFrom contains
	only characters < 128.
*/
void MsoRgwchToRgchSimple(const WCHAR * rgwchFrom, int cchFrom, char * rgchTo, int cchTo) noexcept
{
	DebugFill(rgchTo, cchTo * sizeof(rgchTo[0]));

	AssertTag(FAllSimpleRgwch(rgwchFrom, cchFrom), 0x0010a252 /* tag_aekjs */);

	while(cchTo > 0 && cchFrom > 0)
		{
		*rgchTo++ = (char)*rgwchFrom++;

		cchTo--;
		cchFrom--;
		}
}

/**
	MsoSzToSt

	Converts a C-style null-terminated string sz into a pascal-type
	string st.  Returns the length of the string.  This routine will
	work if sz == st.
*/
int MsoSzToSt(const CHAR *sz, BYTE *st, int cchMax) noexcept
{
	const CHAR *pchFrom = sz;
	BYTE *pchTo = st + 1;
	BYTE chSav = static_cast<BYTE>(*pchFrom);
	BYTE chT;
	const BYTE *pchLim = st + cchMax;

	AssertTag(sz != NULL && st != NULL && cchMax > 0, 0x0010a255 /* tag_aekjv */);
	while (chSav && pchTo < pchLim)
		{
		chT = chSav;
		chSav = static_cast<BYTE>(*++pchFrom);
		*pchTo++ = chT;
		}
	AssertTag(pchFrom - sz <= 255, 0x0010a256 /* tag_aekjw */); // cchMax should be <= 256 to avoid this.
	st[0] = (BYTE)(pchFrom - sz);
	DebugFill(st + st[0] + 1, static_cast<size_t>(cchMax - st[0] - 1));
	return(st[0]);
}

/**
*/
MSOEXTERN_C int MsoSzToWz(const CHAR *sz, WCHAR *wz, int cch) noexcept
{
	AssertTag(sz != NULL, 0x0010a257 /* tag_aekjx */);
	AssertTag((wz != nullptr) == (cch > 0), 0x0010a258 /* tag_aekjy */);
	if (wz != nullptr && cch > 0)
		wz[0] = 0;

	DWORD cchNeed = static_cast<DWORD>(MsoMultiByteToWideChar(CP_ACP, 0, sz, -1, nullptr, 0));
	if (cchNeed <= (DWORD)cch)
		{
		MsoMultiByteToWideChar(CP_ACP, 0, sz, -1, wz, cch);
		if(wz != nullptr)
		{
			DebugFill(wz + cchNeed, (cch - cchNeed)*sizeof(WCHAR));
		}
		AssertTag(MsoCchWzULen(wz)+1 == cchNeed, 0x0010a259 /* tag_aekjz */);
		};
	return static_cast<int>(cchNeed-1);
}

/**
*/
int MsoSzToWtzCore(const CHAR *sz, WCHAR *wtz, int cchMax, IMsoMemHeap *pmmh) noexcept
{
	VerifyElseCrashTag(!(cchMax < 2), 0x0031968e /* tag_amz0o */);
	wtz[0] = wtz[1] = 0;

	int cch = MsoCchSzLen(sz);
	wtz[0] = (WCHAR)MsoCpRgchToRgwchCore(CP_ACP, sz, cch, MsoWzFromWtz(wtz), cchMax-2, pmmh);
	OACR_ASSUME(wtz[0] >= 0 && wtz[0] + 1 < cchMax);
	wtz[wtz[0]+1] = 0; // Ensure always null term
	return(wtz[0]);
}

/**
	MsoStToSz

	Converts the pascal-type string st into a null-terminated string sz.
	Returns length of the string, excluding the trailing null.  This API
	will work if sz == st
*/
int MsoStToSz(const BYTE *st, CHAR *sz, int cchMax) noexcept
{
	int cch = __min(*st, cchMax-1);
	st++;
	char* pch = sz;
	for (; cch--; )
		*pch++ = static_cast<char>(*st++);
	*pch = 0;
	return((int)(pch-sz));
}

/**
*/
int MsoWzToWtz(const WCHAR *wz, WCHAR *wtz, int cchMax) noexcept
{
	if (cchMax == 0)
		return(0);

	int cch = MsoCchWzLen(wz)+2;
	if (cch > cchMax)
		cch = cchMax;
	if (cch > 0x7FFF)
		cch = 0x7FFF;
	int cchRet = cch-2;

	wz += cch-2;
	wtz += cch;

	DebugFill(wtz, (cchMax - cch)*sizeof(WCHAR));

	// Copy the NULL (truncating string of necessary)
	*--wtz = 0;
	cch--;

	while(--cch)
		*--wtz = *--wz;
	*--wtz = (WCHAR)cchRet;

	return(cchRet);
}

/**
*/
int MsoWtzToWz(const WCHAR *wtz, WCHAR *wz, int cchMax) noexcept
{
	int cch = __min(wtz[0], cchMax-1);
	memmove(wz, &wtz[1], cch*sizeof(WCHAR));
	wz[cch] = 0;
	return(cch);
}

/**
	If sz == null, then ret size of (eventual) conversion.
	If wz != sz, no memory allocation is needed.
*/
MSOEXTERN_C int MsoWzToSzCore(const WCHAR *wz, CHAR *sz, int cchMax, IMsoMemHeap *pmmh) noexcept
{
	AssertTag((wz != NULL), 0x0010a25a /* tag_aekj0 */);
	AssertSzTag((sz != nullptr) == (cchMax > 0), "Tiny string buffer?", 0x0010a25b /* tag_aekj1 */);
	AssertSzTag(wz != reinterpret_cast<WCHAR*>(sz), "This API doesn't support writing out to the same buffer", 0x004212c0 /* tag_aq7la */);

	if (sz != nullptr && cchMax > 0)
		sz[0] = 0;

	UINT cch = MsoCchWzULen(wz)+1;
	int iRet = MsoRgwchToRgchCore(wz, static_cast<int>(cch), sz, cchMax, pmmh) - 1;
	AssertTag(iRet < cchMax || sz == NULL, 0x0010a25c /* tag_aekj2 */);
	if (iRet < 0)
		iRet = 0;  // iRet can be -1 if the input string is invalid Unicode
	if (sz != nullptr && iRet < cchMax)
		sz[iRet] = '\0';

	return iRet;
}

/**
*/
int MsoWzToSzNotInPlace(const WCHAR *wz, CHAR *sz, int cchMax) noexcept
{
//	ShipAssertSzTag(reinterpret_cast<const void*>(wz) != reinterpret_cast<const void*>(sz), "This API does not work in-place", 0x0010a25d /* tag_aekj3 */);
//	return MsoWzToSzCore(wz, sz, cchMax, Mso::Memory::GetMsoMemHeap());
    std::abort();
}

/**
	MsoFSzToWzCore

	Converts the Ansi string in rgchFrom with length cchFrom into a
	Unicode string at rgwchTo.  This API will work in-place.
	Returns false if the target buffer is not large enough for the
	conversion.  Returns true on success.
*/
BOOL MsoFSzToWzCore(const CHAR *szFrom, WCHAR *wzTo, int cchTo, IMsoMemHeap *pmmh) noexcept
{
	VerifyElseCrashTag(!(cchTo < 1), 0x0031968f /* tag_amz0p */);
	wzTo[0] = 0;

	DWORD dw;
	UINT cchFrom = MsoCchSzULen(szFrom)+1;
	if (((UINT)cchTo > (dw = static_cast<DWORD>(MsoCpRgchToRgwchCore(CP_ACP, szFrom, static_cast<int>(cchFrom), wzTo, cchTo, pmmh)))) ||
	    ((dw == (UINT)cchTo) && (wzTo[cchTo-1] == L'\0') ) )
	{
		return(true);
	}
	wzTo[cchTo-1] = L'\0';	// assure that the buffer terminates
	return(false);
}

/**
	MsoFWzToSzCore

	Converts the Unicode string in rgwchFrom into an Ansi string at
	rgchTo.  This API will work in-place.
	Returns false if the target buffer is not large enough for the
	conversion.  Returns true on success.
*/
BOOL MsoFWzToSzCore(const WCHAR *wzFrom, CHAR *szTo, int cbTo, IMsoMemHeap *pmmh) noexcept
{
	VerifyElseCrashTag(!(cbTo < 1), 0x00319690 /* tag_amz0q */);
	szTo[0] = 0;

	DWORD dw;
	UINT cchFrom = MsoCchWzULen(wzFrom) + 1;
	if (((UINT)cbTo > (dw = static_cast<DWORD>(MsoRgwchToRgchCore(wzFrom, static_cast<int>(cchFrom), szTo, cbTo, pmmh)))) ||
	    ((dw == (UINT)cbTo) && (szTo[cbTo - 1] == '\0') ) )
	{
		return(true);
	}
	szTo[cbTo - 1] = '\0';	// assure that the buffer terminates
	return(false);
}

/**
*/
int MsoCpSzToWzCore(UINT cp, const CHAR *sz, WCHAR *wz, int cchMax, IMsoMemHeap *pmmh) noexcept
{
	// Callers can ask for the required buffer size, then call again with a valid wz
	if (wz != nullptr)
		{
		VerifyElseCrashTag(!(cchMax < 1), 0x00319691 /* tag_amz0r */);
		wz[0] = 0;
		}

	int cch = MsoCchSzLen(sz)+1;
	int cchConv = MsoCpRgchToRgwchCore(cp, sz, cch, wz, cchMax, pmmh)-1;
	if (cchConv<=0)
		return(MsoCpRgchToRgwchCore(CP_ACP, sz, cch, wz, cchMax, pmmh)-1);
	return(cchConv);
}

/**
	MsoCpRgchToRgwchCore

	Converts the MultiByte string of code page CodePage in rgchFrom with
	length cchFrom into a Unicode string at rgwchTo.  The rgwchTo buffer
	is assumed to be cchTo characters long.  Returns the length of the
	converted string.  This API will work in-place.
*/
MSOEXTERN_C int MsoCpRgchToRgwchCore(UINT CodePage, const CHAR *rgchFrom, int cchFrom, WCHAR *rgwchTo, int cchTo, IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	if (cchFrom == 0 || cchFrom < -1 || cchTo < 0)
//	{
//		// Seems that cchFrom is 0 sometimes when called from LoadPsstIdsInline
//		std::abort(); // SetLastError(ERROR_INVALID_PARAMETER);
//		return(0);
//	}
//
//	if ((void*)rgchFrom == (void*)rgwchTo)
//	{
//		if (rgwchTo == nullptr || cchFrom < 0)
//		{
//			ShipAssertSzTag(false, "Calculate length feature not supported for in-place conversion.", 0x0010a25e /* tag_aekj4 */);
//			std::abort(); // SetLastError(ERROR_INVALID_PARAMETER);
//			return(0);
//		}
//
//		/*
//			VSO.2123220
//			Memheap allocation has significant overhead when caller calls this function with a few character(s) at a time.
//			Optimization: use stack to avoid allocating using memheap when the character count is small
//		*/
//		static const int cchStackBufferMax = 256;
//		if (cchFrom <= cchStackBufferMax)
//		{
//			CHAR rgchTempStack[cchStackBufferMax];
//
//			// it should be safe to use memcpy instead of memmove here because rgchTempStack is a local buffer
//			// that can't overlap with the lpMultiByte param
//			memcpy_s(rgchTempStack, sizeof(rgchTempStack), rgchFrom, sizeof(CHAR) * cchFrom);
//			return MsoMultiByteToWideChar(CodePage, 0, rgchTempStack, cchFrom, rgwchTo, cchTo);
//		}
//		else
//		{
//			Debug(Mso::Memory::AutoShutdownLeakScope scope);
//			Mso::MemHeapPtr<CHAR *> rgchTempHeap;
//			if (!rgchTempHeap.CloneElem(pmmh, rgchFrom, static_cast<size_t>(cchFrom)))
//			{
//				SetLastError(ERROR_NOT_ENOUGH_MEMORY);
//				return(0);
//			}
//
//			return MsoMultiByteToWideChar(CodePage, 0, rgchTempHeap, cchFrom, rgwchTo, cchTo);
//		}
//	}
//
//	if (cchFrom > cchTo || cchFrom == (-1))
//	{
//		// SetLastError called by MsoMultiByteToWideChar when appropriate
//		return(MsoMultiByteToWideChar(CodePage, 0, rgchFrom, cchFrom, rgwchTo, cchTo));
//	}
//
//	const CHAR *pchFrom = rgchFrom;
//	const CHAR *pchMax = pchFrom + cchFrom;
//	if (rgwchTo != nullptr)
//		{
//		OACR_WARNING_SUPPRESS(POSSIBLE_STRCPY_LOOP, "longstanding issue")
//		while (static_cast<unsigned char>(*pchFrom) < 128)
//			{
//			*rgwchTo++ = static_cast<unsigned char>(*pchFrom++);
//			if (pchFrom >= pchMax)
//				return(cchFrom); // Success
//			}
//		}
//	int iRet = MsoMultiByteToWideChar(CodePage, 0, pchFrom, (int)(pchMax-pchFrom),
//			rgwchTo, (int)(cchTo-(pchFrom-rgchFrom)));
//	if (!iRet)
//	{
//		// SetLastError called by MsoMultiByteToWideChar in this case
//		return(0);
//	}
//	return((int)(pchFrom-rgchFrom+iRet));
}

/**
	MsoRgwchToCpRgchNotInPlace

	MsoRgwchToCpRgch version that is guaranteed to not do any
	memory allocation.
*/
int MsoRgwchToCpRgchNotInPlace(UINT CodePage, const WCHAR *rgwchFrom, int cchFrom, CHAR *rgchTo, int cchTo) noexcept
{
	int iRet;
	if (StringCoreFInPlace((const void *)rgwchFrom, (const void *)rgchTo))
		return(0);

	iRet = MsoRgwchToCpRgchCore(CodePage, rgwchFrom, cchFrom, rgchTo, cchTo, (IMsoMemHeap *)NULL);

	return(iRet);
}

/**
	MsoRgwchToCpRgchEx

	Converts a unicode string rgwchFrom (with length cchFrom) into the
	equivalent string in the specified character set. If rgchTo
	is non-NULL, the converted string is stored there.  The length in
	bytes of the converted string is returned, even if rgchTo is NULL.
	Supports CP_ACP and CP_OEMCP defines

	!!! MsoRgwchToCpRgchEx2 (above) relies on MsoRgwchToCpRgchEx
	only needing to allocate memory for the "in-place" copy case !!!
*/
MSOEXTERN_C int MsoRgwchToCpRgchExCore(UINT CodePage, const WCHAR *rgwchFrom, int cchFrom, CHAR *rgchTo, int cchTo, BOOL *pfDefault, IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	AssertTag(nullptr != rgwchFrom, 0x0059609a /* tag_awwc0 */);
//
//	// From MSDN: For strings that require validation, such as file, resource, and user names,
//	//the application should always use the WC_NO_BEST_FIT_CHARS flag.
//	DWORD dwFlags = WC_NO_BEST_FIT_CHARS;
//
//	if ( CodePage == CP_UTF8 || CodePage == CP_CHINAGB18030 )
//		dwFlags = 0;
//
//	if (pfDefault)
//		*pfDefault = false;
//
//	if ((void*)rgwchFrom == (void*)rgchTo)
//	{
//		Debug(Mso::Memory::AutoShutdownLeakScope scope);
//		Mso::MemHeapPtr<WCHAR *> rgwchT;
//
//		AssertTag(cchFrom > 0, 0x0010a25f /* tag_aekj5 */);
//		if (!rgwchT.CloneElem(pmmh, rgwchFrom, static_cast<size_t>(cchFrom)))
//			return 0;
//		return MsoWideCharToMultiByte(CodePage, dwFlags, rgwchT, cchFrom, rgchTo, cchTo, nullptr, pfDefault);
//	}
//
//	int ich = 0;
//	if (rgchTo == nullptr)
//		{
//		for ( ; cchFrom > 0; ich++, cchFrom--, rgwchFrom++)
//			{
//			if ((*rgwchFrom & 0xff80) != 0)
//				return(MsoWideCharToMultiByte(CodePage, dwFlags, rgwchFrom, cchFrom, nullptr, 0, nullptr, pfDefault) + ich);
//			}
//		}
//	else
//		{
//		for ( ; cchFrom > 0 && ich < cchTo; ich++, cchFrom--, rgwchFrom++, rgchTo++)
//			{
//			if ((*rgwchFrom & 0xff80) != 0)
//				return(MsoWideCharToMultiByte(CodePage, dwFlags, rgwchFrom, cchFrom, rgchTo, cchTo-ich, nullptr, pfDefault) + ich);
//			*rgchTo = (CHAR)*rgwchFrom;
//			}
//		}
//	return(ich);
}

/**
*/
int TranslateCpRgchCore(const CHAR *rgchFrom, WORD cbFrom, UINT CodePageFrom, 
	_Out_bytecap_(cbTo) CHAR *rgchTo, WORD cbTo, UINT CodePageTo, IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	if (!cbFrom)
//		return(0);
//
//	// Worst Case scenario is if code page is 54936 (GB18030) or UTF8, so multiply by 4
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<WCHAR *> rgwchT;
//	if (!rgwchT.AllocElem(pmmh, static_cast<size_t>(cbFrom * 4)))
//		return 0;
//
//	int cchRet = MsoMultiByteToWideChar(CodePageFrom, 0, rgchFrom, cbFrom, rgwchT, cbFrom);
//	AssertTag(cchRet <= cbFrom, 0x0010a281 /* tag_aekkb */);
//#ifdef DEBUG
//	BOOL bUsedDefaultChar = false;
//	cchRet = MsoWideCharToMultiByte(CodePageTo, 0, rgwchT, cchRet, rgchTo, cbTo, nullptr, (CodePageTo != CP_UTF7 && CodePageTo != CP_UTF8) ?&bUsedDefaultChar : nullptr);
////	if(bUsedDefaultChar == TRUE)
////		DTraceWz1Tag(0x0010a282 /* tag_aekkc */, msoulscatUnknown, msutlVerbose, L"TranslateCpRgchCore had to use default chars when converting %S", rgchFrom);
//#else
//	cchRet = MsoWideCharToMultiByte(CodePageTo, 0, rgwchT, cchRet, rgchTo, cbTo, NULL, NULL);
//#endif
//	AssertTag(cchRet, 0x0010a283 /* tag_aekkd */);
//	return(cchRet);
}

/**
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoFTranslateCp(UINT ucp1, UINT ucp2, _Inout_cap_(cch) char *pchInOut, int cch) noexcept
{
    std::abort();
//	SafeInt<UINT16> cch16(cch);
//	OACR_ASSUME(cch16 <= cch);
//	OACR_WARNING_SUPPRESS(POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "Oacr is confused");
//	return (TranslateCpRgchCore(pchInOut, cch16, ucp1, pchInOut, cch16, ucp2, nullptr) > 0);
}


/**
*/
__inline WORD CchWz4(const WCHAR *wz) noexcept
{
	WORD cch = 0;
	AssertTag(wz != NULL, 0x0010a284 /* tag_aekke */);
	if (!*wz)
		return(1);	// implies the \0 char inside the string
	while (*wz++ && cch < 4)
		cch++;
	return(cch);
}

/**
	DecompressToWzCore

	Given a Huffman compressed array of cchIn bytes, decompresses it to
	min(cchOut, decompressed length of rgIn) and puts it in wide chars in
	wzOut.

	Be careful when adding any memory management calls to this function -
	code that calls it depends on the (flags & flagUnicode) case not
	needing any memory, and passes in a NULL IMsoMemHeap.
*/
WORD DecompressToWzCore(UINT CodePage, const MSODSTE *rgdste, const BYTE *rgIn, const WORD cchIn, _Out_cap_x_(flags affect meaning of cch) BYTE *rgParam, const WORD cchOut, BYTE flags, IMsoMemHeap *pmmh) noexcept
{
    std::abort();
//	const BYTE *rgEndIn = rgIn + cchIn;
//	Debug(Mso::Memory::AutoShutdownLeakScope scope);
//	Mso::MemHeapPtr<WCHAR *> rgwchOut;
//	WCHAR *pwchOut;
//	const WCHAR *pwchEndOut, *pwchStart;
//	WORD idste = 0;
//	WORD iNextState = 0;
//
//	if (rgdste == nullptr)
//		{
//		AssertTag(false, 0x0010a285 /* tag_aekkf */);
//		return 0;
//		}
//
//	if (flags & flagUnicode)
//		{
//		if (flags & flagNoLen)
//			pwchOut = (WCHAR *)rgParam;
//		else
//			{
//			pwchOut = (WCHAR *)rgParam+1;
//			*(pwchOut-1) = 0;
//			}
//		if (cchIn == 0)
//			{
//			*pwchOut = 0;
//			return(0);
//			}
//		pwchEndOut = pwchOut + cchOut - 1;
//		}
//	else
//		{
//		if (cchIn == 0)
//			{
//			*rgParam = 0;
//			if (flags & flagLen)
//				*(rgParam+1) = 0;
//			return(0);
//			}
//		if (!rgwchOut.AllocElem(pmmh, cchOut))
//			return(0);
//		pwchOut = rgwchOut;
//		pwchEndOut = rgwchOut + cchOut - 1;
//		}
//	pwchStart = pwchOut;
//	while (rgIn < rgEndIn && pwchOut < pwchEndOut)
//		{
//		BYTE ch = *rgIn++;
//		// Decode current byte using lookup table
//		for (int ibit=8; ibit; ibit--, ch=(BYTE)(ch<<1))
//			{
//			if (rgdste[idste].mpbidste[iNextState = (WORD)(((ch & 0x80) ? 1 : 0))])
//				idste=rgdste[idste].mpbidste[iNextState];
//			else
//				{
//				WORD cch = CchWz4(rgdste[idste].rgwchKey);
//				if (pwchOut + cch > pwchEndOut)
//					{
//					// output buffer is smaller than necessary to load the entire string
//					// from resources. the output string will be truncated to fit.
//					// is this really what you want?
//					AssertSzTag(0, "Output buffer for string load too short. String truncated.", 0x0010a286 /* tag_aekkg */);
//					cch = (WORD)(pwchEndOut - pwchOut);
//					}
//				memmove(pwchOut, rgdste[idste].rgwchKey, cch*sizeof(WCHAR));
//				pwchOut += cch;
//				idste=rgdste[0].mpbidste[iNextState];
//				}
//			}
//		}
//	if (!rgdste[idste].mpbidste[iNextState])
//		{
//		WORD cch = CchWz4(rgdste[idste].rgwchKey);
//		if (pwchOut + cch > pwchEndOut)
//			{
//			// output buffer is smaller than necessary to load the entire string
//			// from resources. the output string will be truncated to fit.
//			// is this really what you want?
//			AssertSzTag(0, "Output buffer for string load too short. String truncated.", 0x0010a287 /* tag_aekkh */);
//			cch = (WORD)(pwchEndOut - pwchOut);
//			}
//		memmove(pwchOut, rgdste[idste].rgwchKey, cch*sizeof(WCHAR));
//		pwchOut += cch;
//		}
//#if DEBUG
//	if (rgIn < rgEndIn)
//		{
//		// output buffer is smaller than necessary to load the entire string
//		// from resources. the output string will be truncated to fit.
//		// is this really what you want?
//		AssertSzTag(0, "Output buffer for string load too short. String truncated.", 0x0010a288 /* tag_aekki */);
//		}
//#endif // DEBUG
//
//	// remove trailing zeros
//	while (--pwchOut >= pwchStart && !(*pwchOut))
//		;	// intentionally empty loop
//	pwchOut++;
//	if (flags & flagUnicode)
//		{
//		*pwchOut = 0;
//		if (flags & flagLen)
//			{
//			*(WORD *)rgParam = (WORD)(pwchOut - (WCHAR *)rgParam - 1);
//			return(*rgParam);
//			}
//		return((WORD)(pwchOut - (WCHAR *)rgParam));
//		}
//	else
//		{
//		OACR_ASSUME(NULL != rgwchOut);
//		int iRet;
//		if (flags & flagNoLen)
//			{
//			iRet = MsoRgwchToCpRgchNotInPlace(CodePage, rgwchOut, (int)(pwchOut-rgwchOut), (CHAR *)rgParam, cchOut);
//			if (cchOut && iRet == cchOut)
//				iRet--;
//			rgParam[iRet] = 0;
//			}
//		else
//			{
//			iRet = MsoRgwchToCpRgchNotInPlace(CodePage, rgwchOut, (int)(pwchOut-rgwchOut), (CHAR *)rgParam+1, cchOut);
//			if (cchOut && iRet == cchOut)
//				iRet--;
//			*rgParam = (BYTE)iRet;
//			rgParam[iRet+1] = 0;
//			}
//		return((WORD)iRet);
//		}
}

/**
	SimpleDecompressToWtz

	Simple decompression - just converts ANSI to Unicode.
*/
int SimpleDecompressToWtzCore(const BYTE *rgch, WORD cb, _Out_cap_x_("flags & flagLen ? cch+1 : cch") WCHAR *wtz, int cch, BYTE flags, IMsoMemHeap *pmmh) noexcept
{
	DebugFill(wtz, cch + (flags & flagLen ? 1 : 0) * sizeof(WCHAR));
	int cbOut;
	if (cch < cb)
		{
		// output buffer is smaller than necessary to load the entire string
		// from resources. the output string will be truncated to fit.
		// is this really what you want?
		// NOTE: flagHintBufferSize flag was added to return the correct buffer size.
		if (flags & flagHintBufferSize)
			{
			return(-cb);
			}
		else
			{
			AssertSzTag(0, "Output buffer for string load too short. String truncated.", 0x0010a289 /* tag_aekkj */);
			cb = (WORD)cch;
			}
		}
	if (flags & flagLen)
		{
		cbOut = MsoCpRgchToRgwchCore(1252, (const CHAR *)rgch, cb, &wtz[1], cch, pmmh);
		*wtz = (WCHAR)cbOut;
		wtz[cbOut+1] = 0;
		}
	else
		{
		cbOut = MsoCpRgchToRgwchCore(1252, (const CHAR *)rgch, cb, wtz, cch, pmmh);
		wtz[cbOut] = 0;
		}
	return(cbOut);
}

/**
	FSzFromWzCore

	This function allocates and creates an ANSI string from a wz.

	Parameters:
		CodePage - codepage to use
		wz - input string (unicode)
		psz - output string (ansi)
		cbFudge - if the caller wants some extra bytes allocated.
				These bytes comes at the beginning of the string.
				I.e. the string will start at *ppsz+cbFudge.
*/
BOOL FSzFromWzCore(UINT CodePage, const WCHAR *wz, char **psz, DWORD cbFudge, IMsoMemHeap *pmmh) noexcept
{
	UINT cch, cchT;

	if ((psz == nullptr) || (wz == nullptr))
		return(false);

	cch = MsoCchWzULen(wz)+1;	// Include zero-termninator

	// Get the length we need
	cchT = static_cast<UINT>(MsoRgwchToCpRgchCore(CodePage, wz, static_cast<int>(cch), nullptr, 0, pmmh));

	if (((int)cchT < 0) || (cchT + cbFudge < cchT))
		return(false);

	cchT += cbFudge;		// Add caller's fudge factor

	Debug(Mso::Memory::AutoShutdownLeakScope scope);
	if (FAILED(HrMsoAllocHost(cchT, (void **)psz, pmmh)) || (!*psz))
		return(false);

	cchT = static_cast<UINT>(MsoRgwchToCpRgchCore(CodePage, wz, static_cast<int>(cch), *psz+cbFudge, static_cast<int>(cchT-cbFudge), pmmh));
	// String should be zero-terminated
	AssertTag((*psz)[cchT+cbFudge-1] == 0, 0x0010a28a /* tag_aekkk */); (void)cchT;
	return(true);
}

/**
	StringFromWz

	This function creates a std::string from a wz

	Parameters:
		wzSource - const wz* to use
		maxLength - maximum length of the string wzSource to read.
*/
std::string Mso::StringCore::StringFromWz(_In_z_ const wchar_t* wzSource, size_t maxLength) noexcept
{
	auto length = wcsnlen_s(wzSource, maxLength);
	VerifyElseCrashTag(length < maxLength, 0x01542053 /* tag_bvcbt */);
	std::string outputString;
	if (length > 0)
	{
		int sizeNeeded = MsoWideCharToMultiByte(CP_UTF8, 0, wzSource, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded > 0)
		{
			outputString.resize(sizeNeeded);
			MsoWideCharToMultiByte(CP_UTF8, 0, wzSource, static_cast<int>(length), &outputString[0], sizeNeeded, nullptr, nullptr);
		}
	}
	return outputString;
}

/**
	StringFromWString

	This function creates a std::string from a std::wstring.

	Parameters:
		sourceWString - Source std::wstring to use
*/
std::string Mso::StringCore::StringFromWString(const std::wstring& sourceWString) noexcept
{
	return StringFromWz(sourceWString.c_str(), sourceWString.length() + 1 /*1 extra character for null*/);
}



/**
	
	Functions exported for legacy reasons

*/

/**
*/
int MsoRgchToRgwch(_In_count_(cchFrom) const CHAR *rgchFrom, int cchFrom, _Out_opt_cap_(cchTo) WCHAR *rgwchTo, int cchTo) noexcept
{
	return MsoCpRgchToRgwchCore(CP_ACP, rgchFrom, cchFrom, rgwchTo, cchTo, nullptr);
}

TASKLIBTECHDEBT_(int) MsoRgwchToCpRgch(UINT CodePage, _In_count_(cchFrom) const WCHAR *rgwchFrom, int cchFrom, _Out_opt_cap_(cchTo) CHAR *rgchTo, int cchTo) noexcept
{
	return(MsoRgwchToCpRgchCore(CodePage, rgwchFrom, cchFrom, rgchTo, cchTo, nullptr));
}
