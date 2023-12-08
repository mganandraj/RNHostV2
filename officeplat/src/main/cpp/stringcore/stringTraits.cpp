/**
	stringTraits.cpp

	Copyright (c) 1994 Microsoft Corporation

	Functions related to string and character characteristics.
*/
#include "precomp.h"
#include "insetof.h"
// #include <msostd.h>
#include <core_root/msostringConversion.h>
#include <core_root/msostringTraits.h>
#include "stringcore/core/stringTraits.h"

#include <cstdlib>

//
//  CType 1 Flag Bits.
//
#define C1_UPPER                  0x0001      // upper case
#define C1_LOWER                  0x0002      // lower case
#define C1_DIGIT                  0x0004      // decimal digits
#define C1_SPACE                  0x0008      // spacing characters
#define C1_PUNCT                  0x0010      // punctuation characters
#define C1_CNTRL                  0x0020      // control characters
#define C1_BLANK                  0x0040      // blank characters
#define C1_XDIGIT                 0x0080      // other digits
#define C1_ALPHA                  0x0100      // any linguistic character
#define C1_DEFINED                0x0200      // defined character


//
//  CType 2 Flag Bits.
//
#define C2_LEFTTORIGHT            0x0001      // left to right
#define C2_RIGHTTOLEFT            0x0002      // right to left

#define C2_EUROPENUMBER           0x0003      // European number, digit
#define C2_EUROPESEPARATOR        0x0004      // European numeric separator
#define C2_EUROPETERMINATOR       0x0005      // European numeric terminator
#define C2_ARABICNUMBER           0x0006      // Arabic number
#define C2_COMMONSEPARATOR        0x0007      // common numeric separator

#define C2_BLOCKSEPARATOR         0x0008      // block separator
#define C2_SEGMENTSEPARATOR       0x0009      // segment separator
#define C2_WHITESPACE             0x000A      // white space
#define C2_OTHERNEUTRAL           0x000B      // other neutrals

#define C2_NOTAPPLICABLE          0x0000      // no implicit directionality

//
//  CType 3 Flag Bits.
//
#define C3_NONSPACING             0x0001      // nonspacing character
#define C3_DIACRITIC              0x0002      // diacritic mark
#define C3_VOWELMARK              0x0004      // vowel mark
#define C3_SYMBOL                 0x0008      // symbols

#define C3_KATAKANA               0x0010      // katakana character
#define C3_HIRAGANA               0x0020      // hiragana character
#define C3_HALFWIDTH              0x0040      // half width character
#define C3_FULLWIDTH              0x0080      // full width character
#define C3_IDEOGRAPH              0x0100      // ideographic character
#define C3_KASHIDA                0x0200      // Arabic kashida character
#define C3_LEXICAL                0x0400      // lexical character
#define C3_HIGHSURROGATE          0x0800      // high surrogate code unit
#define C3_LOWSURROGATE           0x1000      // low surrogate code unit

#define C3_ALPHA                  0x8000      // any linguistic char (C1_ALPHA)

#define C3_NOTAPPLICABLE          0x0000      // ctype 3 is not applicable


//
//  String Flags.
//
#define NORM_IGNORECASE           0x00000001  // ignore case
#define NORM_IGNORENONSPACE       0x00000002  // ignore nonspacing chars
#define NORM_IGNORESYMBOLS        0x00000004  // ignore symbols

#define LINGUISTIC_IGNORECASE     0x00000010  // linguistically appropriate 'ignore case'
#define LINGUISTIC_IGNOREDIACRITIC 0x00000020  // linguistically appropriate 'ignore nonspace'

#define NORM_IGNOREKANATYPE       0x00010000  // ignore kanatype
#define NORM_IGNOREWIDTH          0x00020000  // ignore width
#define NORM_LINGUISTIC_CASING    0x08000000  // use linguistic rules for casing

/**
	_WGetCType?Wch

	These static functions are built because of the modification below in
	the API MsoGetStringTypeExW. These functions are used to be in the file
	unistgen.inc, which was generated automatilly during thge build process
	We keep them to minimize the modification because of deleting that file
*/

WORD _WGetCType1Wch(WCHAR wch) noexcept
{
	std::abort();
	return (WORD)0;
//	WORD wCharType;
//	return(OACR_REVIEWED_CALLEX( GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, &wch, 1, &wCharType)) ?
//		wCharType : (WORD)0);
};

/**
*/
WORD _WGetCType3Wch(WCHAR wch) noexcept
{
	std::abort();
	return (WORD)0;
//	WORD wCharType;
//	return(OACR_REVIEWED_CALLEX( GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE3, &wch, 1, &wCharType)) ?
//		wCharType : (WORD)0);
};

/**
   MsoFDigitWch

	Returns true if and only if wch is a digit.
*/
BOOL MsoFDigitWch(WCHAR wch) noexcept
{
	if (wch <= 0x7F)
		{ // we're ANSI
		AssertTag(!(_WGetCType1Wch(wch) & C1_DIGIT) == !Mso::StringTraits::Details::FBetween(wch, '0', '9'), 0x00107503 /* tag_aehud */);
		return(Mso::StringTraits::Details::FBetween(wch, '0', '9'));
		}
	else
		return(_WGetCType1Wch(wch) & C1_DIGIT);
}


/**
	MsoFSzAllDigits

	Returns True if and only if sz contains all digits.
*/
BOOL MsoFSzAllDigitsSimple(const CHAR *sz) noexcept
{
	if (sz == nullptr)
		return FALSE;

	while (*sz != 0)
		{
		if (!Mso::StringTraits::Details::FBetween(*sz, '0', '9'))
			return(false);
		++sz;
		}

	return TRUE;
}

/**
	MsoFWzAllDigits

	Returns True if and only if wz contains all digits.
*/
BOOL MsoFWzAllDigits(const WCHAR *wz) noexcept
{
	if (wz == nullptr)
		return FALSE;

	while (*wz != 0)
		{
		if (!MsoFDigitWch(*wz))
				return(false);
		++wz;
		}

	return TRUE;
}

/**
	MsoFAlphaNumWch

	Returns true if and only if wch is an alphanumeric character.
*/
BOOL MsoFAlphaNumWch(WCHAR wch) noexcept
{
	if (wch <= 0x7F) // we're ANSI
		return(Mso::StringTraits::Details::FBetween(wch | 0x20, 'a', 'z') || Mso::StringTraits::Details::FBetween(wch, '0', '9'));
	else
		return(_WGetCType1Wch(wch) & (C1_DIGIT | C1_ALPHA));
}

/**
	MsoFHexDigitWch

	Returns true if and only if wch is a hexadecimal digit.
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoFHexDigitWch(WCHAR wch) noexcept
{
	return((wch >= L'0' && wch <= L'9') || (wch >= L'a' && wch <= L'f') || (wch >= L'A' && wch <= L'F'));
}

/**
   FChIsTerminator

Returns true if and only if ch is a character that marks the end of a word.
If fBreakAtPunc is true, some additional punctuation characters are
considered to be acceptable end-of-word markers.
*/
bool FChIsTerminator(WCHAR ch, bool fBreakAtPunc) noexcept
{
	return(ch == L' ' || ch <= 0x0D || (fBreakAtPunc && (ch == L'\\' || ch == L'/')));
}

/**
	MsoFPuncWch

	returns true if wch is a punctuation char and false otherwise.

	don't move this function to kjstr_mso.cpp because it calls a
	static function _WGetCType1Wch.
*/
MSODLL_MANDATED_C_EXPORT BOOL MsoFPuncWch(WCHAR wch) noexcept
{
	return(_WGetCType1Wch(wch) & C1_PUNCT);
}

/**
	MsoFAlphaWch

	returns true if wch is an alphabetic char and false otherwise.
*/
MSOEXTERN_C BOOL MsoFAlphaWch(WCHAR wch) noexcept
{
	if (wch <= 0x7F)	// we're ANSI
		return(Mso::StringTraits::Details::FBetween(wch | 0x20, 'a', 'z'));
	else
		return( _WGetCType1Wch(wch) & C1_ALPHA );

}

/**
	MsoFExtenderWch

	returns true if wch is an Extender char and false otherwise. (QFE 15139)
	Extender characters are definied here: 
	http://www.w3.org/TR/2000/REC-xml-20001006.html#CharClasses
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoFExtenderWch(WCHAR wch) noexcept
{
	if ((wch == 0x00b7) || (wch == 0x02d0) || (wch == 0x02d1) || (wch == 0x0387) || 
		(wch == 0x0640) || (wch == 0x0e46) || (wch == 0x0ec6) || (wch == 0x3005) ||
		Mso::StringTraits::Details::FBetween(wch, 0x3031, 0x3035) || Mso::StringTraits::Details::FBetween(wch, 0x309d, 0x309e) || Mso::StringTraits::Details::FBetween(wch, 0x30fc, 0x30fe))
		{
		return true;
		}
	else
		{
		return false;
		}
}

/**
	MsoFSpecChWch

	returns true if wch is a special char and false otherwise.
*/
BOOL MsoFSpecChWch(WCHAR wch) noexcept
{
	WORD wCharType;

	if(!wch)
		return(false);

	wCharType = _WGetCType1Wch(wch);
	if(_WGetCType1Wch(wch) & C1_ALPHA)
		return(true);

	if ( wCharType & C1_PUNCT )
		return( (wch != 0x5E) && (wch != 0x60) // chSkill, chBSQuote
				 && _WGetCType3Wch(wch) & (C3_ALPHA | C3_DIACRITIC | C3_NONSPACING | C3_VOWELMARK));
	else if ( (wch > 0xFE) && (wCharType & C1_DEFINED) )
		return(true);
	else
		return(false);
}

/**
	MsoFComplexMarkWch

	returns true if wch is a diacritic, tonemark, or a non-spacing character.
*/
BOOL MsoFComplexMarkWch(WCHAR wch) noexcept
{
	WORD wCharType;

	if(!wch)
		return(false);

	wCharType = _WGetCType1Wch(wch);
	if(_WGetCType1Wch(wch) & C1_ALPHA)
		return(false);

	if ( wCharType & C1_PUNCT )
		return(_WGetCType3Wch(wch) & (C3_DIACRITIC | C3_NONSPACING | C3_VOWELMARK));
	else if(wCharType & C1_DEFINED)
		return(true);
	else
		return(false);

}

/**
Returns true if and only if wch is a East Asian Char.
*/
// C linkage required: Used by GetProcAddress
MSOEXTERN_C BOOL MsoFFEWch(WCHAR wch) noexcept
{
	std::abort();
	return false;
//	char rgch[2];
//
//	return (MsoRgwchToRgchCore(&wch, 1, rgch, 2, Mso::Memory::GetMsoMemHeap()) == 2);
}

/*----------------------------------------------------------------------------
	MsoFWchSpace
	MsoFChSpace

	%%Owner: ASlepak
	%%Id: 4b5d1c7b-33ce-4f4c-97de-48b52c458c37
------------------------------------------------------------------ t-hailiu--*/
MSODLL_MANDATED_C_EXPORT BOOL MsoFSpaceWch(WCHAR wch) noexcept
{
	if (wch <= 0x7F)	// we're ANSI
		return(InBiasSetOf6(wch, 9,10,11,12,13,32));	//InSetOf6(wch-1, 31,8,9,10,11,12);
	else
		return(_WGetCType1Wch(wch) & C1_SPACE);
}

/**
*/
MSOEXTERN_C BOOL MsoFSpaceCh(CHAR ch) noexcept
{
	return(InBiasSetOf6(ch, 9,10,11,12,13,32));	//InSetOf6(ch-1, 31,8,9,10,11,12);
}

/**

Function name: MsoIsWchIndicVowel

Description: Check if character is a Indic dependant vowel or combination character

shirazs
*/
BOOL MsoIsWchIndicVowel(WCHAR wch) noexcept
{
	if ( wch > 0x7F && ( Mso::StringTraits::Details::FBetween(wch, 0x0901, 0x0903) || Mso::StringTraits::Details::FBetween(wch, 0x093C, 0x0954) || Mso::StringTraits::Details::FBetween(wch, 0x0962, 0x0963) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0981, 0x0983) || Mso::StringTraits::Details::FBetween(wch, 0x09BC, 0x09D7) || Mso::StringTraits::Details::FBetween(wch, 0x09E2, 0x09E3) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0A02, 0x0A02) || Mso::StringTraits::Details::FBetween(wch, 0x0A3C, 0x0A4D) || Mso::StringTraits::Details::FBetween(wch, 0x0A70, 0x0A71) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0A81, 0x0A83) || Mso::StringTraits::Details::FBetween(wch, 0x0ABC, 0x0AD0) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0B01, 0x0B03) || Mso::StringTraits::Details::FBetween(wch, 0x0B3C, 0x0B63) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0B82, 0x0B83) || Mso::StringTraits::Details::FBetween(wch, 0x0BBE, 0x0BD7) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0C01, 0x0C03) || Mso::StringTraits::Details::FBetween(wch, 0x0C3E, 0x0C56) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0C82, 0x0C83) || Mso::StringTraits::Details::FBetween(wch, 0x0CBE, 0x0CD6) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0D02, 0x0D03) || Mso::StringTraits::Details::FBetween(wch, 0x0D3E, 0x0D57) ||
			Mso::StringTraits::Details::FBetween(wch, 0x0D82, 0x0D83) || Mso::StringTraits::Details::FBetween(wch, 0x0DCA, 0x0DF3) )
			)
		{
		return TRUE;
    	}
	else
		return FALSE;
}


/**

Function name: FIsThaiChar

Description: Check if character is a Thai Character

aarayas
*/
bool MsoFIsThaiChar(WCHAR wch) noexcept
{
	return ( (wch >= 0x0e01) && (wch <= 0x0e5b) );
}

/**

Function name: MsoIsWchVietToneMark

Description: Check if character is a Vietnamese Tonemark

schai
*/
BOOL MsoIsWchVietToneMark(WCHAR wch) noexcept
{
	if ( Mso::StringTraits::Details::FBetween(wch, 0x0300, 0x323) )
		{
		switch (wch)
			{
			case 0x0300:    // Combining grave accent
			case 0x0301:    // Combining acute accent
			case 0x0303:    // Combining tilde
			case 0x0309:    // Combining hook above
			case 0x0323:    // Combining dot below
				return TRUE;
			default:
				return FALSE;
			}
		}
	else
		return FALSE;
}

/**
	MsoFLowAsciiRgch
*/
BOOL MsoFLowAsciiRgch(LPCSTR sz, int cch) noexcept
{
	const DWORD dwMask = 0x80808080;

	do
	{
	while (cch > 0 && ( cch < 4 || ((DWORD_PTR)sz & 3)))
		{
		if (static_cast<unsigned char>(*sz) > 127)
			return(false);
		cch--;
		sz++;
		}
	if (cch == 0)
		return(true);
	while (cch >= 4)
		{
		AssertTag(((DWORD_PTR)sz & 3) == 0, 0x00107508 /* tag_aehui */);
		DWORD dw = *((const DWORD*)sz);
		if (dw & dwMask)
			return(false);
		cch -=4;
		sz +=4;
		}
	} while(true);
}


/**
	MsoFLowAsciiRgwch
*/
BOOL MsoFLowAsciiRgwch(LPCWSTR wz, int cch) noexcept
{
	const DWORD dwMask = 0xff80ff80;

	do
	{
	while (cch > 0 && ( cch < 2 || ((DWORD_PTR)wz & 3)))
		{
		if (*wz > 127)
			return(false);
		cch--;
		wz++;
		}
	if (cch == 0)
		return(true);
	while (cch >= 2)
		{
		AssertTag(((DWORD_PTR)wz & 3) == 0, 0x00107509 /* tag_aehuj */);
		DWORD dw = *((const DWORD*)wz);
		if (dw & dwMask)
			return(false);
		cch -=2;
		wz +=2;
		}
	} while (true);
}

/**
	ValHexDigitWch

	Returns the value of the hex digit in wch.
*/
int ValHexDigitWch(WCHAR wch) noexcept
{
	AssertTag(MsoFHexDigitWch(wch), 0x0010750a /* tag_aehuk */);
	if (Mso::StringTraits::Details::FBetween(wch, L'0', L'9'))
		return wch - L'0';
	else if (Mso::StringTraits::Details::FBetween(wch, L'a', L'f'))
		return wch - L'a' + 10;
	else
		{
		AssertTag(Mso::StringTraits::Details::FBetween(wch, L'A', L'F'), 0x0010750b /* tag_aehul */);
		return wch - L'A' + 10;
		}
}


/**
    MetroFValidPartChar

    The routine determines if wch is a Valid Metro pchar.

    Valid Metro PChars are all ASCII except:
        control chars 0x00-0x1F
        space 0x20
        " 0x22
        # 0x23
        / 0x2F
        : 0x3A
        < 0x3C
        > 0x3E
        ? 0x3F
        [ 0x5B
        \ 0x5C
        ] 0x5D
        ^ 0x5E
        ` 0x60
        { 0x7B
        | 0x7C
        } 0x7D
        DEL 0x7F
*/
BOOL MetroFValidPartChar(const WCHAR wch) noexcept
{
	const static BOOL vrgfMetroPchar[] =
	{
//		0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F         (Disallowed)
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - Control chars
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1 - Control chars
		0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 2 - Space, ", #, /
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, // 3 - :, <, >, ?
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4 - (none)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 5 - [, \, ], ^
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6 - `
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, // 7 - {, |, }, DEL
	};

	static_assert(_countof( vrgfMetroPchar ) == 0x80, "array/enum out of sync");
	if( wch < _countof( vrgfMetroPchar ) )
		return vrgfMetroPchar[ wch ];
	return FALSE;
}

#ifdef __cplusplus
//Surrogate Variation Selectors are in the UTF32 range E0100...E01EF
bool Mso::StringTraits::MsoIsSurrogateVariationSelector( const WCHAR* pwz, const int & cwch ) noexcept
{
	if( cwch <= 1 )
		return false;
	if( pwz[ 0 ] == 0xDB40 && Mso::StringTraits::Details::FBetween( pwz[ 1 ], 0xDD00, 0xDDEF ) )
		return true;

	return false;
}
#endif

#ifdef DEBUG
/**
*/
bool FAllSimpleRgch(const CHAR *sz, int cch) noexcept
{
	while (cch--)
		if (*sz++ & 0x80)
			return false;
	return true;
}


/**
*/
bool FAllSimpleRgwch(const WCHAR *wz, int cch) noexcept
{
	while (cch--)
		if (*wz++ & 0xFF80)
			return false;
	return true;
}

/**
	MsoFValidWtz

	Checks if wtz is a valid string
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoFValidWtz(const WCHAR *wtz) noexcept
{
	HRESULT hr = S_OK;

	CheckBoolATag(wtz != NULL, 0x00210687 /* tag_aiq0h */);
	CheckBoolATag(wtz[0] <= 0x7fff, 0x00210688 /* tag_aiq0i */);
	CheckBoolATag(wtz[1+wtz[0]] == 0, 0x00210689 /* tag_aiq0j */);

LError:
	return SUCCEEDED(hr);
}
#endif // DEBUG
