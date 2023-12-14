/**
	stringParsing.cpp

	Copyright (c) 1994 Microsoft Corporation

	Functions related to parsing data to/from strings
*/
#include "precomp.h"
#include "comparison.h"
#include <core_root/msoStringConversion.h>
#include <core_root/msoStringTraits.h>
#include <core_root/msoStringUtils.h>
#include <stringcore/core/stringparsing.h>
// #include <intsafe.h>
#include "stringcore/core/stringTraits.h"
#include "stringcore/parsing.h"

#include <memory>

#include <crash/verifyElseCrash.h>
// #include "stringParsingInternal.h"

const char c_rgchHex[] = "0123456789ABCDEF";

inline static void StringCoreConvertMinTo100ns(_Inout_ LPFILETIME lpTime) noexcept
{
	ULARGE_INTEGER ul;
	ul.u.LowPart = lpTime->dwLowDateTime;
	ul.u.HighPart = lpTime->dwHighDateTime;

	// AssertSzTag(ul.QuadPart < (UINT64_MAX / 600000000), "Overflow has occurred during conversion", 0x376c7265 /* tag_7lre */);

	ul.QuadPart *= 600000000;
	lpTime->dwLowDateTime = ul.u.LowPart;
	lpTime->dwHighDateTime = ul.u.HighPart;
}

/**
	IDecodeDigit

	Returns the digit from double dDec at place dHolder.
	dHolder should me a multiple or fraction of 10.  (100, 0.001, etc).
	Helper function for MsoWzDecodeDouble below.
*/
int IDecodeDigit(double dDec, double dHolder) noexcept
{
	//iDigit = ((int)(10 * dDec))/(10 * dHolder);	// loses precision
	int iDigit = 0;
	for (int i=9; i>0; i--)
		{
		if (dDec >= i * dHolder)
			{
			iDigit = i;
			break;
			}
		}
	return(iDigit);
}

/**
	IDigitValueOfWch

	Digit value of a character.
*/
int IDigitValueOfWch(WCHAR wch) noexcept
	{
	// ASCII digits, Fullwidth digits.
	// Arabic-Indic, Extended Arabic-Indic.
	// Superscript 0, 4..9.  Subscript 0..9.
	if (Mso::StringTraits::Details::FBetween(wch,0x30,0x39) || Mso::StringTraits::Details::FBetween(wch,0xFF10,0xFF19) ||
		Mso::StringTraits::Details::FBetween(wch,0x0660,0x0669) || Mso::StringTraits::Details::FBetween(wch,0x06F0,0x06F9) ||
		Mso::StringTraits::Details::FBetween(wch,0x2070,0x2079) || Mso::StringTraits::Details::FBetween(wch,0x2080,0x2089))
		{
		return(wch & 0x000F);
		}
	// Devanagari, Bengali, Gurmukhi, Gujarati, Oriya, Tamil, Telugu, Kannada,
	// Malayalam.
	else if (Mso::StringTraits::Details::FBetween(wch,0x0966,0x096F) || Mso::StringTraits::Details::FBetween(wch,0x09E6,0x09EF) ||
			 Mso::StringTraits::Details::FBetween(wch,0x0A66,0x0A6F) || Mso::StringTraits::Details::FBetween(wch,0x0AE6,0x0AEF) ||
			 Mso::StringTraits::Details::FBetween(wch,0x0B66,0x0B6F) || Mso::StringTraits::Details::FBetween(wch,0x0BE6,0x0BEF) ||
			 Mso::StringTraits::Details::FBetween(wch,0x0C66,0x0C6F) || Mso::StringTraits::Details::FBetween(wch,0x0CE6,0x0CEF) ||
			 Mso::StringTraits::Details::FBetween(wch,0x0D66,0x0D6F))
		{
		return((wch & 0x000F) - 6);
		}
	// Thai, Lao, Tibetan, Khmer.
	else if (Mso::StringTraits::Details::FBetween(wch,0x0E50,0x0E59) || Mso::StringTraits::Details::FBetween(wch,0x0ED0,0x0ED9) ||
			 Mso::StringTraits::Details::FBetween(wch,0x0F20,0x0F29) || Mso::StringTraits::Details::FBetween(wch,0x17E0,0x17E9))
		{
		return(wch & 0x000F);
		}
	// Superscript 2, 3, 1.
	else if (wch==0xB2 || wch==0xB3 || wch==0xB9)
		{
		return(wch & 0x0007);
		}
	// Tamil 10, 100, 1000;
	else if (Mso::StringTraits::Details::FBetween(wch,0x0BF0,0x0BF2))
		{
		return(wch==0x0BF0 ? 10 : wch==0x0BF1 ? 100 : 1000);
		}
	else
		{
		AssertTag (!MsoFDigitWch(wch), 0x0010a2c1 /* tag_aeklb */);
		return(0);
		}
	}


/**
	ParseIntWz

	This function does the actual conversion of a wz to an int (signed/unsigned)
*/
int ParseIntWz(const WCHAR *rgwch, int *pdw, bool fSigned) noexcept
{
	uint32_t dw;
	uint32_t maxval;
	unsigned digval;
	bool fNeg = false;
	const WCHAR *pwch;

	dw = 0;
	pwch = rgwch;
	// Skip white space
	while (MsoFSpaceWch(*pwch))
		++pwch;

	if (*pwch == L'-')
		{
		// We didn't want signed numbers
		if (!fSigned)
			return(0);
		fNeg = true;
		pwch++;
		}
	else if (*pwch == L'+')		// Skip the sign
		++pwch;

	maxval = UINT_MAX / 10;
	while (MsoFDigitWch(*pwch))
		{
		digval = static_cast<uint32_t>(IDigitValueOfWch(*pwch));

		// Make sure we don't overflow
		if (dw < maxval || (dw == maxval && (unsigned long)digval <= ULONG_MAX % 10))
			dw = 10*dw + digval;
		else
			return(0);  // overflow
		pwch++;
		}
	// We could still have overflowed, let's make sure...
	if ((fSigned) && ((fNeg && (dw > static_cast<uint32_t>(-LONG_MIN))) ||
				(!fNeg && (dw > LONG_MAX))))
		return(0);

	*pdw = static_cast<int>(dw);
	if (fSigned && fNeg)
		*pdw = -(int32_t)dw;
	return((int)(pwch - rgwch));
}

/**
	ParseHexIntSz

	This function does the actual conversion of an sz to an unsigned int. It
	looks for a hexadecimal integer with or without a leading "0x".
*/
unsigned ParseHexIntSz(const CHAR *rgch, unsigned *pu) noexcept
{
	unsigned u;
	unsigned maxval;
	int digval;
	const CHAR *pch;

	u = 0;
	pch = rgch;
	// Skip white space
	while (MsoFSpaceCh(*pch))
		++pch;

	// check for leading 0x
	if (*pch == '0' && *(pch + 1) == 'x')
		pch += 2;

	maxval = UINT_MAX >> 4;
	while ((digval = ValHexCh(*pch)) >= 0)
		{
		// Make sure we don't overflow
		if (u < maxval || (u == maxval && (unsigned)digval <= UINT_MAX % 16))
			u = 16*u + digval;
		else
			return(0);	// overflow
		pch++;
		}
	*pu = u;
	return static_cast<uint32_t>((int)(pch - rgch));
}

/**
	ParseHexUIntWz

	This function does the actual conversion of an wz to an unsigned int. It
	looks for a hexadecimal integer with or without a leading "0x".
*/
unsigned ParseHexUIntWz(const WCHAR *rgwch, unsigned *pu) noexcept
{
	unsigned u;
	unsigned maxval;
	int digval;
	const WCHAR *pwch;

	u = 0;
	pwch = rgwch;
	// Skip white space
	while (MsoFSpaceWch(*pwch))
		++pwch;

	// check for leading 0x
	if (*pwch == '0' && *(pwch + 1) == 'x')
		pwch += 2;

	maxval = UINT_MAX >> 4;
	while ((digval = MsoValHexWch(*pwch)) >= 0)
		{
		// Make sure we don't overflow
		if (u < maxval || (u == maxval && (unsigned)digval <= UINT_MAX % 16))
			u = 16*u + digval;
		else
			return(0);	// overflow
		pwch++;
		}
	*pu = u;
	return static_cast<uint32_t>((int)(pwch - rgwch));
}

/**
*/
void MsoHexFromCh(WCHAR *pchHex, int cchHex, WCHAR ch) noexcept
{
	WCHAR mask = 0xf000;
	int shr = 12;

	if (cchHex >= 4)
		{
		do
			{
			OACR_ASSUME(shr >= 0);
			*pchHex++ = (WCHAR) c_rgchHex[((ch & mask ) >> shr)];
			mask >>= 4;
			shr -= 4;
			}
		while(mask != 0);
		}
	else
		{
		AssertTag (false, 0x0010a2c2 /* tag_aeklc */);
		}
}

/**
	MsoHexFromBytes

	Converts each byte in rgbBytes to two hex characters and returns the hex
	string in rgwchHex.

	Returns the number of characters written, not including the null-terminator.
*/
unsigned int MsoHexFromBytes(
	wchar_t *rgwchEncodedHex,
	unsigned int cchEncodedHex,
	const BYTE* rgbBytesToEncode,
	unsigned int cbBytesToEncode) noexcept
{
	AssertTag(rgwchEncodedHex, 0x002416db /* tag_ajb11 */);
	if (!rgwchEncodedHex)
		return 0;

	AssertTag(rgbBytesToEncode, 0x002416dc /* tag_ajb12 */);
	if (!rgbBytesToEncode)
		return 0;

	// Need room for 2 hex characters per byte, plus null-terminator.
	if (2*cbBytesToEncode + 1 > cchEncodedHex)
	{
		AssertSzTag(false, "Output buffer not large enough.", 0x002416dd /* tag_ajb13 */);
		return 0;
	}

	// OACR warning being ignored:
	// Potential mismatch between sizeof and _countof quantities. Use sizeof() to scale byte sizes.
	OACR_WARNING_SUPPRESS(SIZEOF_COUNTOF_MISMATCH, "This line will never be hit if 2*cbBytesToEncode is off the end of the array");
	rgwchEncodedHex[2*cbBytesToEncode] = L'\0';

	for (unsigned int i = 0; i < cbBytesToEncode; i++)
	{
		const BYTE byte = rgbBytesToEncode[i];
		unsigned char byteHigh = static_cast<unsigned char>(byte >> 4);
		unsigned char byteLow = static_cast<unsigned char>(byte & 0x0F);
		rgwchEncodedHex[2*i + 0] = static_cast<wchar_t>(c_rgchHex[byteHigh]);
		rgwchEncodedHex[2*i + 1] = static_cast<wchar_t>(c_rgchHex[byteLow]);
	}

	return 2*cbBytesToEncode;
}

bool MsoBytesFromHex(
	const wchar_t* wzFrom,
	_Out_bytecap_(cb) BYTE *pb, 
	_Inout_ DWORD& cb) noexcept
{
	AssertTag(pb, 0x0025640b /* tag_ajwql */);
	if(!pb)
		return false;

	if(!wzFrom)
	{
		cb = 0;
		return true;
	}

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfor-loop-analysis"
#endif
	unsigned int i,j=0;
	for(i = 0 ; wzFrom[i] ; i++ )
	{
		if(!wzFrom[i+1])
			return false; //Wrong format
		if(j >= cb)
			return false; //Not enough size
		int hexdigit1 = MsoValHexWch(wzFrom[i]);
		if(hexdigit1 == -1)
			return false; //Wrong format
		int hexdigit2 = MsoValHexWch(wzFrom[i+1]);
		if(hexdigit2 == -1)
			return false; //Wrong format
		pb[j++]=(BYTE)(16*hexdigit1+hexdigit2);
		i++;
	}
#if __clang__
#pragma clang diagnostic pop
#endif
	cb=j;
	return true;
}


/**
	MsoWzDecodeInt

	Decodes the signed integer w into Unicode text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
MSOEXTERN_C int MsoWzDecodeInt(WCHAR *rgwch, int cch, int w, int wBase) noexcept
{
	return(MsoWzDecodeInt64(rgwch, cch, static_cast<int64_t>(w), wBase));
}

/**
	MsoWzDecodeInt64

	Decodes the signed int64 w into Unicode text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
MSOEXTERN_C int MsoWzDecodeInt64(WCHAR *rgwch, int cch, int64_t w, int wBase) noexcept
{
	VerifyElseCrashTag(!(cch <= 0), 0x0034c192 /* tag_anmgs */);
	if (w < 0)
		{
		*rgwch = '-';
		return(MsoWzDecodeUint64(rgwch+1, cch-1, static_cast<UINT64>(-w), wBase) + 1);
		}
	return(MsoWzDecodeUint64(rgwch, cch, static_cast<UINT64>(w), wBase));
}

/**
	MsoWzDecodeUint

	Decodes the unsigned integer w into Unicode text in base wBase.
	Returns the length of the text decoded.
*/
MSOEXTERN_C int MsoWzDecodeUint(WCHAR *rgwch, int cch, unsigned u, int wBase) noexcept
{
	return MsoWzDecodeUint64(rgwch, cch, static_cast<UINT64>(u), wBase);
}

/**
	MsoWzDecodeUint64

	Decodes the UINT64 w into Unicode text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
int MsoWzDecodeUint64(WCHAR *rgwch, int cch, UINT64 u, int wBase) noexcept
{
	AssertTag(wBase >= 2 && wBase <= 16, 0x0010a2c3 /* tag_aekld */);
	DebugFill(rgwch, cch*sizeof(WCHAR));

	if (cch >= 1)
		*rgwch = 0;
	if (cch <= 1)
		return(0);

	if (u == 0)
		{
		rgwch[0] = L'0';
		rgwch[1] = 0;
		return(1);
		}

	int cDigits = 0;
	UINT64 uT = u;

	while(uT)
		{
		cDigits++;
		uT /= wBase;
		};
	if (cDigits >= cch)
		return(0);
	rgwch += cDigits;
	*rgwch-- = 0;
	uT = u;
	while(uT)
		{
		*rgwch-- = static_cast<WCHAR>(c_rgchHex[uT % wBase]);
		uT /= wBase;
		};

	return(cDigits);
}

/**
	MsoWzDecodeUIntFill

	Decodes the integer w into Unicode text in base wBase,
	filling any unused space with leading zeros.
	Returns number of significant digits.
*/
int MsoWzDecodeUIntFill(_Out_z_cap_(cch) WCHAR *rgwch, int cch, unsigned u, int wBase) noexcept
{
	AssertTag(wBase >= 2 && wBase <= 16, 0x0010a2c4 /* tag_aekle */);
	DebugFill(rgwch, cch*sizeof(WCHAR));

	if (cch >= 1)
		*rgwch = 0;
	if (cch <= 1)
		return(0);

	int cDigits = 0;
	unsigned uT = u;

	while(uT)
		{
		cDigits++;
		uT /= wBase;
		};
	if (cDigits >= cch)
		return(0);
	rgwch += cch-1;
	*rgwch-- = 0;
	uT = u;
	while(uT)
		{
		*rgwch-- = static_cast<WCHAR>(c_rgchHex[uT % wBase]);
		uT /= wBase;
		};

	for (int i = 0; i < cch - cDigits - 1; i++)
		*rgwch-- = L'0';

	return(cDigits);
}

/**
	MsoWzDecodeDouble

	Decodes the double d into Unicode text. The string is stored in the
	rgwch buffer.  Returns the length of the text decoded.
*/
MSOEXTERN_C int MsoWzDecodeDouble(WCHAR *rgwch, int cchMax, double d) noexcept
{
	return MsoWzDecodeDoubleEx(rgwch, cchMax, L'.', d);
}

/**
	MsoWzDecodeDoubleEx

	Decodes the double d into Unicode text. The string is stored in the
	rgwch buffer.  Returns the length of the text decoded.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
int MsoWzDecodeDoubleEx(WCHAR *rgwch, int cchMax, const WCHAR wchDp, double d) noexcept
{
	int cch;

	DebugFill(rgwch, cchMax*sizeof(WCHAR));

	if (cchMax <= 1)		// check that we have room for the sign
		{
		if (cchMax > 0)
			rgwch[0] = L'\0';
		return(0);
		}

	cch = 0;

	int fNegativeSign = d < 0;
	if (fNegativeSign)	// output sign if necessary
		rgwch[cch++] = L'-';
	double dVal = 1;
	double dMag = ((d > 0) ? d : -d);	// get absolute value of d

	if (dMag > DBL_MAX)
		{
		if (6 >= cchMax)
			{
			rgwch[0] = L'\0';
			return(0);
			}
		// if the number is too darn big just write out the INF thing
		// MsoWzCopy(L"1.#INF", &rgwch[cch], cchMax - cch);
		wcsncpy(L"1.#INF", &rgwch[cch], cchMax - cch);
		return(__min(cch + 6, cchMax - 1));
		}

	int iExp;
	// find the base 10 exponent
	if (dMag == 0 || dMag == 1)
		iExp = 0;
	else if (dMag > 1)
		{
		for (iExp=0; dVal<=dMag/10; iExp++)
			dVal *= 10;
		}
	else
		{
		for (iExp=0; dVal>dMag; iExp--)
			dVal /= 10;
		}

	int iFigs;					// current number of significant figures converted
	double dDec = dMag;		// decremented magnitude remaining to convert
	double dAcc = 0;			// accumulated value converted so far
	double dHolder = 0.1;	// place holder
	int iDigit = 0;

	// output the whole integer digits
	if (iExp < 6 && iExp >= 0)			// output floats >= 1 and < 1000000 in normal notation
		{
		dHolder = dVal;
		while (dHolder >= 1)
			{
			iDigit = IDecodeDigit(dDec, dHolder);
			if (cch >= cchMax)
				{
				rgwch[0] = L'\0';
				return(0);
				}
			rgwch[cch++] = (WCHAR)(L'0' + iDigit);
			dAcc += iDigit * dHolder;
			dDec -= iDigit * dHolder;
			dHolder /= 10;
			}
		iFigs = iExp;		// 10^iExp yield iExp+1 digits before the decimal
		iExp = 0;
		dVal = 1;
		}
	else if (iExp < 0 && iExp >= -4)	// output floats < 1 and >= 0.0001 in normal notation
		{
		if (cch >= cchMax)
			{
			rgwch[0] = L'\0';
			return(0);
			}
		rgwch[cch++] = L'0';
		iFigs = iExp + 1;	// provides zero padding after the decimal, still giving 15 sig figs
		iExp = 0;
		}
	else							// otherwise use scientific notation
		{
		dDec /= dVal;	// divide out the exponent
		iDigit = IDecodeDigit(dDec, 1);
		if (cch >= cchMax)
			{
			rgwch[0] = L'\0';
			return(0);
			}
		rgwch[cch++] = (WCHAR)(L'0' + iDigit);
		iFigs = 1;
		dAcc = iDigit;
		dDec -= iDigit;
		}

	if (cch >= cchMax)
		{
		rgwch[0] = L'\0';
		return(0);
		}
	rgwch[cch++] = wchDp;	// output the decimal

	// output the fractional digits
	while (iFigs<16)
		{
		iDigit = IDecodeDigit(dDec, dHolder);
		if (iFigs == 15)		// skip to rounding...
			break;
		if (cch >= cchMax)
			{
			rgwch[0] = L'\0';
			return(0);
			}
		rgwch[cch++] = (WCHAR)(L'0' + iDigit);
		dAcc += iDigit * dHolder;
		dDec -= iDigit * dHolder;
		dHolder *= 0.1;
		if (dAcc*dVal >= dMag)
			break;
		iFigs++;
		}

	// rounding
	if ((iFigs == 15 && dAcc*dVal < dMag && iDigit > 4 && iDigit <= 9) ||
		(iFigs == 14 && dAcc*dVal >= dMag && iDigit == 9)) // round least sig fig now
		{
		int cchPeriod = -1;
		cch--;	// go back one char
		AssertSzTag(MsoFDigitWch(rgwch[cch]), "Only round digits!", 0x01781291 /* tag_b4bkr */);
		rgwch[cch] = (WCHAR)(L'0' + (rgwch[cch] - L'0' + 1)%10);		// round up last digit
		while (rgwch[cch] == L'0' || rgwch[cch] == L'-')
			{
			cch--;											// safe, trailing zeros will be removed anyway
			if (cch < 0)
				{
				cchPeriod = -1;
				// hey, we rounded all the way back to the beginning!
				AssertTag(cch == -1, 0x0010a2c5 /* tag_aeklf */);
				cch = fNegativeSign;
				// find the data we need to shift...
				while (rgwch[cch] == L'0')
					cch++;
				AssertTag(rgwch[cch] == wchDp, 0x0010a2c6 /* tag_aeklg */);
				cch++;
				AssertTag(rgwch[cch] == L'0', 0x0010a2c7 /* tag_aeklh */);
				if (iExp == 0)
					{
					// shift everything over...
					int cchT = cch;
					while (cchT >= 0 + fNegativeSign) // leave negative sign in place
						{
						rgwch[cchT + 1] = rgwch[cchT];
						cchT--;
						}
					// insert the overflow...
					rgwch[0 + fNegativeSign] = L'1';
					}
				else
					{
					AssertTag(rgwch[1 + fNegativeSign] == wchDp, 0x0010a2c8 /* tag_aekli */);
					AssertTag(rgwch[2 + fNegativeSign] == L'0', 0x0010a2c9 /* tag_aeklj */);
					AssertTag(cch == 2 + fNegativeSign, 0x0010a2ca /* tag_aeklk */);
					// inc the exponent to account for the overflow...
					iExp++;
					// account for the overflow...
					rgwch[0 + fNegativeSign] = L'1';
					}
				break;
				}
			if (rgwch[cch] == wchDp) // if we hit the decimal, skip over it
				{
				cchPeriod = cch;
				cch--;
				AssertTag(cch >= 0, 0x0010a2cb /* tag_aekll */);
				}
			if (rgwch[cch] != L'-')
				{
				AssertSzTag(MsoFDigitWch(rgwch[cch]), "Only round digits!", 0x01781292 /* tag_b4bks */);
				rgwch[cch] = (WCHAR)(L'0' + (rgwch[cch] - L'0' + 1)%10);	// keep rounding 9's up the chain...
				}
			}
		// if we hit period while rounding the "9"s, move cch back
		if (cchPeriod >= 0)
			cch = cchPeriod;
		cch++;	// skip ahead again
		}

	while (rgwch[cch-1] == L'0')	// remove any trailing zeros
		cch--;
	if (rgwch[cch-1] == wchDp)	// if we hit the decimal, tack one zero back on
		cch++;

	if (iExp != 0)	// export the base 10 exponent if necessary
		{
		if (cch >= cchMax)
			{
			rgwch[0] = L'\0';
			return(0);
			}
		rgwch[cch++] = L'E';
		if (iExp < 0)
			{
			if (cch >= cchMax)
				{
				rgwch[0] = L'\0';
				return(0);
				}
			rgwch[cch++] = L'-';
			iExp *= -1;
			}
		if (iExp >= 100)
			{
			if (cch+2 >= cchMax)
				{
				rgwch[0] = L'\0';
				return(0);
				}
			rgwch[cch++] = (WCHAR)(L'0' + iExp/100);
			rgwch[cch++] = (WCHAR)(L'0' + (iExp/10)%10);
			rgwch[cch++] = (WCHAR)(L'0' + iExp%10);
			}
		else if (iExp >= 10)
			{
			if (cch+1 >= cchMax)
				{
				rgwch[0] = L'\0';
				return(0);
				}
			rgwch[cch++] = (WCHAR)(L'0' + iExp/10);
			rgwch[cch++] = (WCHAR)(L'0' + iExp%10);
			}
		else
			{
			if (cch >= cchMax)
				{
				rgwch[0] = L'\0';
				return(0);
				}
			rgwch[cch++] = (WCHAR)(L'0' + iExp);
			}
		}

	if (cch >= cchMax)
		{
		rgwch[0] = L'\0';
		return(0);
		}
	rgwch[cch] = 0;
	return(cch);
}

/**
	MsoSzDecodeInt

	Decodes the signed integer w into ASCII text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
MSOEXTERN_C int MsoSzDecodeInt(CHAR *rgch, int cch, int w, int wBase) noexcept
{
	VerifyElseCrashTag(!(cch <= 0), 0x0034c193 /* tag_anmgt */);

	if (w < 0)
		{
		*rgch = '-';
		return static_cast<int>(MsoSzDecodeUint(rgch+1, cch-1, static_cast<uint32_t>(-w), wBase) + 1);
		}
	return static_cast<int>(MsoSzDecodeUint(rgch, cch, static_cast<ULONG>(w), wBase));

}

/**
	MsoSzDecodeUint

	Decodes the unsigned integer u into ASCII text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
MSOEXTERN_C int MsoSzDecodeUint(CHAR *rgch, int cch, unsigned u, int wBase) noexcept
{
	return MsoSzDecodeSize_t(rgch, cch, static_cast<size_t>(u), wBase);
}

/**
	MsoSzDecodeSize_t

	Decodes the size_t u into ASCII text in base wBase. The string
	is stored in the rgch buffer, which is assumed to be large enough to hold
	the number's text and a null terminator. Returns the length of the text
	decoded.
*/
OACR_WARNING_SUPPRESS(RETURNING_BAD_RESULT, "historical issue")
int MsoSzDecodeSize_t(CHAR *rgch, int cch, size_t u, int wBase) noexcept
{
	AssertTag(wBase >= 2 && wBase <= 16, 0x0010a2cc /* tag_aeklm */);
	DebugFill(rgch, static_cast<size_t>(cch));

	if (cch >= 1)
		*rgch = 0;
	if (cch <= 1)
		return(0);

	if (u == 0)
		{
		rgch[0] = '0';
		rgch[1] = 0;
		return(1);
		}

	int cDigits = 0;
	size_t uT = u;

	while(uT)
		{
		cDigits++;
		uT /= wBase;
		};
	if (cDigits >= cch)
		return(0);
	rgch += cDigits;
	*rgch-- = 0;
	uT = u;
	while(uT)
		{
		*rgch-- = c_rgchHex[uT % wBase];
		uT /= wBase;
		};

	return(cDigits);
}

/**
	MsoWzDecodeTime

	Decodes the SystemTime st into Unicode text, in ISO8061 format.
	The string is stored in the rgwch buffer
	Returns the length of the string (not including the null)
*/
int MsoWzDecodeTime(WCHAR *rgwch, int cch, const SYSTEMTIME *pst) noexcept
{
	AssertTag(rgwch && pst, 0x0010a2cd /* tag_aekln */);
	if (!rgwch || !pst)
		return(false);
	DebugFill(rgwch, cch*sizeof(WCHAR));

	//assert that the SYSTEMTIME passed in is valid;
	AssertTag(0 == (pst->wYear / 10000) &&
			0 == (pst->wMonth / 100) &&
			0 == (pst->wDay / 100) &&
			0 == (pst->wHour / 100) &&
			0 == (pst->wMinute / 100) &&
			0 == (pst->wSecond / 100), 0x0010a2ce /* tag_aeklo */);

	std::abort();
	return 0;
	//use mod operation to ensure the string fits with 20 characters
//	return _snwprintf_s(rgwch, static_cast<size_t>(cch), _TRUNCATE, L"%04d-%02d-%02dT%02d:%02d:%02dZ",
//		pst->wYear % 10000, pst->wMonth % 100, pst->wDay % 100,
//		pst->wHour % 100, pst->wMinute % 100, pst->wSecond % 100);
}

/**
	SystemTimeToISO8601

	Decodes the system time to an ISO8601 GMT time.
	For use when you know where the SYSTEMTIME came from (FileTimeToSystemTime
	doesn't usually give you bad outputs) and don't need to do 6 division operations.
*/
_Use_decl_annotations_
bool SystemTimeToISO8601(
	const SYSTEMTIME * pstTime,
	wchar_t* wzISO8601Time,
	ULONG cch) noexcept
{
	if (wzISO8601Time == nullptr || cch == 0)
		return false;

	wzISO8601Time[0] = '\0';

	if (pstTime == nullptr || cch < CchMaxISO8601Time)
		return false;

	std::abort();
	/*if (_snwprintf_s(wzISO8601Time, cch, _TRUNCATE, L"%04d-%02d-%02dT%02d:%02d:%02dZ",
		         pstTime->wYear,
				 pstTime->wMonth,
				 pstTime->wDay,
				 pstTime->wHour,
				 pstTime->wMinute,
				 pstTime->wSecond) == -1)
	{
		return false;
	}*/

	return true;
}

/**
   MsoParseIntWz

   Parses the Unicode text at rgwch into *pw. Returns the count of
   characters considered part of the number. Continues reading characters
	and encoding them into *pw until it encounters a non-digit.
	Handles overflow by returning zero.
*/
MSOEXTERN_C int MsoParseIntWz(const WCHAR *rgwch, int *pw) noexcept
{
	return(ParseIntWz(rgwch, pw, true));
}

/**
	MsoParseInt64Wz

	Parses the Unicode text at rgwch into *pw. Returns the count of
	characters considered part of the number. Continues reading characters
	and encoding them into *pw until it encounters a non-digit.
	Handles overflow by returning zero.

	wzIn			[IN]	String we are converting.
	pl64			[OUT]	int64_t value from wzIn.
*/
MSOEXTERN_C int MsoParseInt64Wz(const WCHAR* rgwch, int64_t *pw) noexcept
{
	uint64_t ui64 = 0;
	uint64_t ui64Max = UINT64_MAX / 10;
	bool fNeg = false;
	const WCHAR *pwch;

	AssertTag((rgwch != NULL), 0x0010a2cf /* tag_aeklp */);
	AssertTag((pw != NULL), 0x0010a2d0 /* tag_aeklq */);

	pwch = rgwch;
	// Skip white space
	while (MsoFSpaceWch(*pwch))
		++pwch;

	if (*pwch == L'-')
		{
		fNeg = true;
		pwch++;
		}
	else if (*pwch == L'+')		// Skip the sign
		++pwch;

	while (MsoFDigitWch(*pwch))
		{
		unsigned digVal = 0;

		digVal = static_cast<uint32_t>(IDigitValueOfWch(*pwch));

		// Make sure we don't overflow
		if (ui64 < ui64Max || (ui64 == ui64Max && (uint64_t)digVal <= UINT64_MAX % 10))
			ui64 = 10*ui64 + digVal;
		else
			return 0;  // overflow
		pwch++;
		}

	// We could still have overflowed, let's make sure...
	if ((fNeg && (ui64 > static_cast<uint64_t>(-INT64_MIN))) ||
		(!fNeg && (ui64 > INT64_MAX)))
		return(0);

	*pw = static_cast<int64_t>(ui64);
	if (fNeg)
		*pw = -(int64_t)ui64;

	return((int)(pwch - rgwch));
}

/**
   MsoParseUIntWz

   Parses the Unicode text at rgwch into *pw. Returns the count of
   characters considered part of the number. Continues reading characters
	and encoding them into *pw until it encounters a non-digit.
	Handles overflow by returning zero.
*/
MSOEXTERN_C int MsoParseUIntWz(const WCHAR *rgwch, unsigned *pw) noexcept
{
	return(ParseIntWz(rgwch, (int*) pw, false));
}

/**
	MsoParseUInt64Wz

	Mostly copied from MsoParseInt64Wz

	wzIn			[IN]	String we are converting.
	pl64			[OUT]	uint64_t value from wzIn.
*/
MSOEXTERN_C int MsoParseUInt64Wz(const WCHAR* rgwch, uint64_t *pw) noexcept
{
	uint64_t ui64 = 0;
	uint64_t ui64Max = UINT64_MAX / 10;
	const WCHAR *pwch;

	AssertTag((rgwch != NULL), 0x0010a2d1 /* tag_aeklr */);
	AssertTag((pw != NULL), 0x0010a2d2 /* tag_aekls */);

	pwch = rgwch;
	// Skip white space
	while (MsoFSpaceWch(*pwch))
		++pwch;

	if (*pwch == L'+')		// Skip the sign
		++pwch;

	while (MsoFDigitWch(*pwch))
		{
		unsigned digVal = 0;

		digVal = static_cast<uint32_t>(IDigitValueOfWch(*pwch));

		// Make sure we don't overflow
		if (ui64 < ui64Max || (ui64 == ui64Max && (uint64_t)digVal <= UINT64_MAX % 10))
			ui64 = 10*ui64 + digVal;
		else
			return(0);  // overflow
		pwch++;
		}

	*pw = ui64;

	return((int)(pwch - rgwch));
}

/**
	MsoParseHexIntWz
*/
MSOEXTERN_C int MsoParseHexIntWz(const WCHAR *wz, int *pdw) noexcept
{
	return static_cast<int>(ParseHexUIntWz(wz, (unsigned int *) pdw));
}

/**
	MsoParseHexUIntWz
*/
int MsoParseHexUIntWz(const WCHAR *wz, unsigned *pu) noexcept
{
	return static_cast<int>(ParseHexUIntWz(wz, pu));
}

/**
   MsoParseDoubleWz

   Parses the Unicode text at rgwch into *pd. Returns the count of
   characters considered part of the number. Can handle with or without
   decimal fraction and exponent. This version of the call assumes that the
   decimal characters is '.'
*/
int MsoParseDoubleWz(const WCHAR *rgwch, double *pd) noexcept
{
	return MsoParseDoubleWzEx(rgwch, L'.', pd);
}


namespace Mso::StringCore::Flighting
{

static bool g_fFixMsoParseDoubleWzEx = true;

void FixMsoParseDoubleWzEx(bool fFix) noexcept
{
	g_fFixMsoParseDoubleWzEx = fFix;
}

} // namespace Mso::StringCore::Flighting

/**
   MsoParseDoubleWz

   Parses the Unicode text at rgwch into *pd. Returns the count of
   characters considered part of the number. Can handle with or without
   decimal fraction and exponent.
*/
int MsoParseDoubleWzEx(const WCHAR *rgwch, const WCHAR wchDp, double *pd) noexcept
{
	static_assert(sizeof(double) == 8, "Compiler error?");

	AssertTag(rgwch != NULL, 0x0010a2d3 /* tag_aeklt */);
	AssertTag(pd != NULL, 0x0010a2d4 /* tag_aeklu */);
	if (rgwch == nullptr || pd == nullptr)
		return(0);

	int iSign = 1;
	const WCHAR *pwch = rgwch;

	while (MsoFSpaceWch(*pwch)) // scan past any leading whitespace
		pwch++;

	if (*pwch == L'+') // get sign if any
		pwch++;
	else if (*pwch == L'-')
	{
		iSign = -1;
		pwch++;
	}

	// Note: this is a case-insensitive compare for legacy compat
	bool isInfinity = false;
	wchar_t wz1DotINF[] = L"1.#INF"; // Need to overwrite the decimal point with wchDp
	if (Mso::StringCore::Flighting::g_fFixMsoParseDoubleWzEx)
		wz1DotINF[1] = wchDp;
	if (Mso::StringAscii::Equal(pwch, wz1DotINF))
	{
		isInfinity = true;
		pwch += 6;
	}
	else if (Mso::StringExact::Equal(pwch, L"INF"))
	{
		isInfinity = true;
		pwch += 3;
	}

	if (isInfinity)
	{
		*pd = (iSign == 1) ? INFINITY : -INFINITY;
		return((int)(pwch - rgwch));
	}

	bool isNaN = false;
	wchar_t wz1DotQNAN[] = L"1.#QNAN"; // Need to overwrite the decimal point with wchDp
	if (Mso::StringCore::Flighting::g_fFixMsoParseDoubleWzEx)
		wz1DotQNAN[1] = wchDp;
	if (Mso::StringAscii::Equal(pwch, wz1DotQNAN))
	{
		isNaN = true;
		pwch += 7;
	}
	else if (Mso::StringExact::Equal(pwch, L"NaN"))
	{
		isNaN = true;
		pwch += 3;
	}

	if (isNaN)
	{
		*pd = NAN;
		return((int)(pwch - rgwch));
	}

	// When decoding the fractional portion of the mantissa, the old implementation of this function relied
	// on multiplying the digit by 0.1, which is not precisely representable in binary.
	if (Mso::StringCore::Flighting::g_fFixMsoParseDoubleWzEx)
	{
		int cchConsumed = static_cast<int>(pwch - rgwch);

		// If we have a non-. decimal separator, then we have to copy the string into a temporary buffer,
		// remapping this separator to the C-locale '.' separator. Note that it's not safe to overwrite the
		// decimal separator in-place, as the string could be in a read-only memory segment.
		std::unique_ptr< wchar_t[] > pBuffer;
		if (wchDp != L'.')
		{
			// Find the decimal separator, or end of string
			const wchar_t* pwchDp = pwch;
			while (MsoFDigitWch(*pwchDp))
				++pwchDp;

			// If we found the decimal separator, time to clone the string, replacing the separator in the process.
			if (*pwchDp == wchDp)
			{
				// Need to figure out how many more characters are in the string
				const wchar_t* pwchEnd = pwchDp + 1;
				while (MsoFDigitWch(*pwchEnd))
					++pwchEnd;
				if (*pwchEnd == L'e' || *pwchEnd == L'E') // handle exponent if any
				{
					++pwchEnd;
					if (*pwchEnd == L'+' || *pwchEnd == L'-') // get sign if any
						++pwchEnd;
					while (MsoFDigitWch(*pwchEnd))
						++pwchEnd;
				}

				size_t cch = pwchEnd - pwch;
				pBuffer = std::make_unique<wchar_t[]>(cch + 1);
				::memcpy(pBuffer.get(), pwch, cch * sizeof(wchar_t));
				pBuffer[cch] = L'\0';
				pBuffer[pwchDp - pwch] = L'.';
				pwch = pBuffer.get();
			}
		}

		// Parse the value using the "C" locale '.' decimal separator
		const wchar_t* pwchEnd = nullptr;
		// *pd = iSign * Mso::StringCore::wcstod_c_locale(pwch, &pwchEnd);
		// *pd = iSign * Mso::StringCore::wcstod_c_locale(pwch, &pwchEnd);
		*pd = iSign * wcstod(pwch, const_cast<wchar_t **>(&pwchEnd));

		cchConsumed += static_cast<int>(pwchEnd - pwch);
		return cchConsumed;
	}
	else
	{
		double d;
		for (d = 0; MsoFDigitWch(*pwch); pwch++)	// build up integer value
			d = (d * 10.0) + (iSign * IDigitValueOfWch(*pwch));

		if (*pwch == wchDp) // skip over decimal char
			pwch++;

		for (double dHolder = 0.1; MsoFDigitWch(*pwch); pwch++, dHolder *= 0.1)
			d += iSign * dHolder * IDigitValueOfWch(*pwch); // build up decimal value

		if (*pwch == L'e' || *pwch == L'E') // handle exponent if any
		{
			pwch++;
			bool fExpPositive = true;
			if (*pwch == L'+') // get sign if any
				pwch++;
			else if (*pwch == L'-')
			{
				fExpPositive = false;
				pwch++;
			}
			
			int iExp; // the exponent power: 10^iExp
			double dExp = 1.0; // the actual value of the exponent: dExp = 10^iExp
			for (iExp = 0; MsoFDigitWch(*pwch); pwch++)
				iExp = iExp * 10 + IDigitValueOfWch(*pwch); // build up exponent magnitude
				
			while (iExp > 0)
			{
				dExp *= fExpPositive ? 10.0 : 0.1; // build up the exponent value
				iExp--;
			}
			d *= dExp;
		}

	   if( d == 0 && iSign < 0 )
		  d = -0.0;

		*pd = d; // fix the sign and set pd

		return((int)(pwch - rgwch)); // return num of chars used
	}
}

/**
	MsoParseHexIntSz
*/
int MsoParseHexIntSz(const CHAR *sz, int *pdw) noexcept
{
	return static_cast<int>(ParseHexIntSz(sz, (unsigned int*)pdw));
}

/**
   MsoParseTimeWz

   Parses the Unicode text at rgwch into *pst.  Expects time to be in
   ISO8061 format.  (ISO8061 == yyyy-mm-ddThh:mm:ssZ and the Z can be
   replaced by a zone adjustment of +hh:mm or -hh:mm)  If only date is present,
   will set time to 00:00:00Z.  Time returned as Z-time, not local time.
   Returns 0 if cannot parse time, otherwise returns number of characters
	considered part of the time specification.
*/
int MsoParseTimeWz(const WCHAR* rgwch, SYSTEMTIME *pst) noexcept
{
	unsigned u;
	int cchT;
	int ich = 0;

	// clear pst
	memset(pst, 0, sizeof(SYSTEMTIME));

	// parse date
	cchT = MsoParseUIntWz(rgwch, &u);
	if (!cchT) // no valid integer parsed
		return 0;

	if (rgwch[cchT] == L':') // hey there was no date!
		{
		// mark date as today then go find the proper time
		SYSTEMTIME stCur;
		GetSystemTime(&stCur);
		pst->wYear = stCur.wYear;
		pst->wMonth = stCur.wMonth;
		pst->wDay = stCur.wDay;
		OACR_WARNING_SUPPRESS(GOTO, "Historical goto")
		goto LParseTime;
		}

	if (cchT == 8)
		{
		// found short date format: "yyyymmdd"
		pst->wDay = (WORD)(u%100);
		u /= 100;
		pst->wMonth = (WORD)(u%100);
		u /= 100;
		pst->wYear = (WORD)u;
		ich = cchT + 1;
		}
	else if (cchT == 6)
		{
		// found short date format: "yyyymm"
		pst->wDay = 1;
		pst->wMonth = (WORD)(u%100);
		u /= 100;
		pst->wYear = (WORD)u;
		ich = cchT + 1;
		}
	else if (cchT == 4)
		{
		// found date format: "yyyy"
		pst->wYear = (WORD)u;
		pst->wMonth = 1;
		pst->wDay = 1;
		ich = cchT + 1;
		if (rgwch[ich - 1] == L'-')
			{
			// found "mm"
			cchT = MsoParseUIntWz(&rgwch[ich], &u);
			if (cchT != 2) // no valid integer parsed
				return ich;
			pst->wMonth = (WORD)u;
			ich += cchT + 1;
			if (rgwch[ich - 1] == L'-')
				{
				// found "dd"
				cchT = MsoParseUIntWz(&rgwch[ich], &u);
				if (cchT != 2) // no valid integer parsed
					return ich;
				pst->wDay = (WORD)u;
				ich += cchT + 1;
				}
			}
		}
	else
		return 0; // we didn't find a date format we recognize

	if (rgwch[ich - 1] != L'T') // the string doesn't contain a time!
		return ich - 1; // but that is OK - just let time be 00:00:00

LParseTime:
	// parse time
	cchT = MsoParseUIntWz(&rgwch[ich], &u);
	if (cchT != 2) // no valid integer parsed
		return ich;
	pst->wHour = (WORD)u;
	ich += cchT + 1;
	if (rgwch[ich - 1] != L':')
		return ich - 1;
	cchT = MsoParseUIntWz(&rgwch[ich], &u);
	if (cchT != 2) // no valid integer parsed
		return ich;
	pst->wMinute = (WORD)u;
	ich += cchT + 1;
	if (rgwch[ich - 1] != L':')
		return ich - 1;
	cchT = MsoParseUIntWz(&rgwch[ich], &u);
	if (cchT != 2) // no valid integer parsed
		return ich;
	pst->wSecond = (WORD)u;
	ich += cchT;
	if (rgwch[ich] != L'.')
	{
		OACR_WARNING_SUPPRESS(GOTO, "Historical goto")
		goto LParseTZ;
	}
	// parse milliseconds
	if (!MsoFDigitWch(rgwch[ich+1]))
		return 0; // invalid milliseconds format
	cchT = MsoParseUIntWz(&rgwch[ich+1], &u);
	if (cchT == 0)
		return 0; // invalid milliseconds format
	ich  += cchT+1;
	// multiply by 10^(3-cchT)
	while (cchT<3) { u *= 10; cchT++; }
	while (cchT>3) { u /= 10; cchT--; }
	pst->wMilliseconds = (WORD)u;

LParseTZ:
	if (rgwch[ich] == L'Z') // return if in Z time zone
		return ich + 1;
	if (rgwch[ich] == 0) // presume Z time zone
		return ich;

	// [danjump] Although slightly confusing, a negative time zone value
	// means we need to add to the local time.  Consider if you live in
	// Pacific Time Zone (PST, -08:00).  A local time for you must be
	// increased by 8 hours to match Zulu time.  Midnight PST == 8AM Zulu.
	bool fNegAdj;	// is negative time zone adjustment?
	if (rgwch[ich] == L'-')
		fNegAdj = false;
	else if (rgwch[ich] == L'+')
		fNegAdj = true;
	else
		return ich; // we don't have a proper time zone adjustment
	ich++; // skip over the sign character now

	unsigned uZoneAdj; // handle local time zone adjustment
	FILETIME ft, ftAdj;
	DWORD dw;
	  // get hour adjustment
	cchT = MsoParseUIntWz(&rgwch[ich], &uZoneAdj);
	// time zone adjustment may have hh:mm or may be hhmm
	if (cchT != 2 /*hh:mm*/ && cchT != 4 /*hhmm*/) // no valid integer parsed
		return ich - 1;
	if (cchT == 2) // hh:mm
	{
		ich += cchT + 1;
		uZoneAdj *= 60; // convert hours to minutes
		  // get minute adjustment
		cchT = MsoParseUIntWz(&rgwch[ich], &u);
		if (cchT != 2) // no valid integer parsed
			return ich - 1 - 2; // backup past previous hour zone adjustment
		ich += cchT;
	}
	else // hhmm
	{
		ich += cchT;
		u = uZoneAdj % 100; // remember the minutes
		uZoneAdj /= 100; // trim off the minutes
		uZoneAdj *= 60; // convert hours to minutes
	}
	uZoneAdj += u;
	ftAdj.dwLowDateTime = uZoneAdj;
	ftAdj.dwHighDateTime = 0;
	if ( (ftAdj.dwLowDateTime > (12*60) && !fNegAdj) ||   // if the adjustment is positive, must be <=12
		 (ftAdj.dwLowDateTime > (13*60) && fNegAdj) )     // if the adjustment is negative, must be <=13
		return ich - 5; // backup past previous hour & min zone adjustments
	StringCoreConvertMinTo100ns(&ftAdj);
	if (!SystemTimeToFileTime(pst, &ft))
		return 0;
	  // adjust filetime to account for time zone
	dw = ft.dwLowDateTime;
	if (fNegAdj)
		ft.dwLowDateTime -= ftAdj.dwLowDateTime;
	else
		ft.dwLowDateTime += ftAdj.dwLowDateTime;
	  // adjust for overflow/underflow
	if (fNegAdj ? (dw < ft.dwLowDateTime) : (dw > ft.dwLowDateTime))
		fNegAdj ? ft.dwHighDateTime-- : ft.dwHighDateTime++;
	  // no check on high bit overflow...
	if (fNegAdj)
		ft.dwHighDateTime -= ftAdj.dwHighDateTime;
	else
		ft.dwHighDateTime += ftAdj.dwHighDateTime;
	  // finally convert back to system time (now in Z-time zone)
	if (!FileTimeToSystemTime(&ft, pst))
		return 0;

	return ich;	// everything parsed correctly!
}

/**
	ParseISO8601FileTime

	Parses out a file time from an ISO8601 GMT time.

	Note - does not tolerate things like no supplied date, and also 
	does not deal with anything other than universal time. 
*/
_Use_decl_annotations_
bool ParseISO8601FileTime(LPCWSTR wzISO8601Time, FILETIME * pftTime) noexcept
{
	const FILETIME ftZero = {0};
	SYSTEMTIME st = {0};

	wchar_t* wzTmp = const_cast<wchar_t*>(wzISO8601Time);
	OACR_WARNING_SUPPRESS(CROSS_PLATFORM_LONG_INCORRECTLY_USED, "Used to capture the return values of APIs that Office doesn't own")
	unsigned long ul = 0;

	if (pftTime == nullptr)
		return false;

	*pftTime = ftZero;

	if (wzISO8601Time == nullptr)
		return false;

	/*
	As per http://www.w3.org/TR/NOTE-datetime
	date and time formats should be:

	  Year:
      YYYY (eg 1997)
   Year and month:
      YYYY-MM (eg 1997-07)
   Complete date:
      YYYY-MM-DD (eg 1997-07-16)
   Complete date plus hours and minutes:
      YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)
   Complete date plus hours, minutes and seconds:
      YYYY-MM-DDThh:mm:ssTZD (eg 1997-07-16T19:20:30+01:00)
   Complete date plus hours, minutes, seconds and a decimal fraction of a second
      YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+01:00)

	*/

	// First, get year
	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul == 0)
		return false;

	st.wYear = static_cast<WORD>(ul);

	if (*wzTmp != '-')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Else, get the month
	++wzTmp;

	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul < 1 || ul > 12)
		return false;

	st.wMonth = static_cast<WORD>(ul);

	if (*wzTmp != '-')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Now the day
	++wzTmp;

	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul < 1 || ul > 31)
		return false;

	st.wDay = static_cast<WORD>(ul);

	if (*wzTmp != 'T')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Next will be the time, which could be fractional
	// Hours
	++wzTmp;

	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul > 23)
		return false;

	st.wHour = static_cast<WORD>(ul);

	if (*wzTmp != ':')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Minutes
	++wzTmp;

	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul > 59)
		return false;

	st.wMinute = static_cast<WORD>(ul);

	if (*wzTmp != ':')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Seconds
	++wzTmp;

	ul = wcstoul(wzTmp, &wzTmp, 10);

	if (ul > 59)
		return false;

	st.wSecond = static_cast<WORD>(ul);

	if (*wzTmp != '.')
	{
		return !!SystemTimeToFileTime(&st, pftTime);
	}

	// Lastly, milliseconds

	double d = wcstod(wzTmp, &wzTmp);
	d *= 1000;
	st.wMilliseconds = static_cast<WORD>(d);

	return !!SystemTimeToFileTime(&st, pftTime);
}

/**
   MsoFParseBoolWz

   ASCII compares (case-insensitive)  as follows
		if wz is "false", "NO", "0", "F", "N"   returns false
		if wz is "true",  "YES" "1", "T", "Y"	returns true
		if ws is none of the above, Asserts and returns false.

	This is very hard coded routine for the sake of perf.
*/
OACR_WARNING_SUPPRESS(RETURNTYPE_CAN_BE_BOOL, "No it can't.  Used in C code")
int MsoFParseBoolWz(LPCWSTR wz) noexcept
{
#define wz_t             L"TtYy1\0"
#define wz_f             L"FfNn0\0"

	bool fRetVal = true;
	UINT cch = 0;

	cch = static_cast<UINT>(MsoCchWzLen(wz));

	if ((cch > 0) && (cch <= 5)) // "FALSE" is the longest string
		{
		switch (cch)
			{
			case 1: // look for single chars T/t/Y/y/1/F/f/N/n/0
				{
				const WCHAR *pwch;
				for(pwch = wz_t; *pwch; pwch++)
					{
					if (*pwch == *wz)
						{
						return (fRetVal);
						}
					}
				for(pwch = wz_f; *pwch; pwch++)
					{
					if (*pwch == *wz)
						{
						return false;
						}
					}
				}
				break;

			case 2: // NO
				if (((wz[0] == 'n')  || (wz[0] == 'N')) &&
					 ((wz[1] == 'o') || (wz[1] == 'O')))
					{
					return false;
					}
				break;

			case 3: // YES
				if (((wz[0] == 'y')  || (wz[0] == 'Y')) &&
					 ((wz[1] == 'e') || (wz[1] == 'E')) &&
					 ((wz[2] == 's') || (wz[2] == 'S')))
					{
					return (fRetVal);
					}
				break;

			case 4: // TRUE
				if (((wz[0] == 't')  || (wz[0] == 'T')) &&
					 ((wz[1] == 'r') || (wz[1] == 'R')) &&
					 ((wz[2] == 'u') || (wz[2] == 'U')) &&
					 ((wz[3] == 'e') || (wz[3] == 'E')))
					{
					return (fRetVal);
					}
				break;

			case 5: // FALSE
				if (((wz[0] == 'f')  || (wz[0] == 'F')) &&
					 ((wz[1] == 'a') || (wz[1] == 'A')) &&
					 ((wz[2] == 'l') || (wz[2] == 'L')) &&
					 ((wz[3] == 's') || (wz[3] == 'S')) &&
					 ((wz[4] == 'e') || (wz[4] == 'E')))
					{
					return false;
					}
				break;

			default:
				break;
			}
		}

	AssertSzTag(FALSE, "Unexpected Boolean String passed to MsoFParseBooleanWz", 0x0010a2d7 /* tag_aeklx */);

	fRetVal = false;

	return (fRetVal);
}

/**
*/
BOOL MsoFGuidFromWz(const WCHAR *wz, GUID *pguid) noexcept
{
//	const WCHAR *pwch;
//	int i;
//	USHORT s3;
//	bool fStartBrace;
//
//	if (!wz || !pguid)
//		return FALSE;
//
//	pwch = wz;
//
//	fStartBrace = *pwch == L'{';
//	if (fStartBrace)
//		pwch++;
//
//	pguid->Data1 = 0;
//
//	for (i = 0; *pwch != L'-' && *pwch && i < 8; i++)
//		{
//		int digval = MsoValHexWch(*pwch++);
//		if (digval < 0)
//			return FALSE; //Wrong GUID format: non-hex character
//		pguid->Data1 = (ULONG)(pguid->Data1 * 16 + digval);
//		}
//	if (i != 8 || *pwch != L'-')
//		return FALSE; //Wrong GUID format.
//
//	pwch++;
//	pguid->Data2 = 0;
//
//	for (i = 0; *pwch != L'-' && *pwch && i < 4; i++)
//		{
//		int digval = MsoValHexWch(*pwch++);
//		if (digval < 0)
//			return FALSE; //Wrong GUID format: non-hex character
//		pguid->Data2 = (USHORT)(pguid->Data2 * 16 + digval);
//		}
//	if (i != 4 || *pwch != L'-')
//		return FALSE; //Wrong GUID format.
//
//	pwch++;
//	pguid->Data3 = 0;
//
//	for (i = 0; *pwch != L'-' && *pwch && i < 4; i++)
//		{
//		int digval = MsoValHexWch(*pwch++);
//		if (digval < 0)
//			return FALSE; //Wrong GUID format: non-hex character
//		pguid->Data3 = (USHORT)(pguid->Data3 * 16 + digval);
//		}
//
//	if (i != 4 || *pwch != L'-')
//		return FALSE; //Wrong GUID format.
//
//	pwch++;
//	s3 = 0;
//
//	for (i = 0; *pwch != L'-' && *pwch && i < 4; i++)
//		{
//		int digval = MsoValHexWch(*pwch++);
//		if (digval < 0)
//			return FALSE; //Wrong GUID format: non-hex character
//		s3 = (USHORT)(s3 * 16 + digval);
//		}
//
//	if (i != 4 || *pwch != L'-')
//		return FALSE; //Wrong GUID format.
//
//	pguid->Data4[0] = (BYTE)(s3 / 0x100);
//	pguid->Data4[1] = (BYTE)(s3 % 0x100);
//
//	pwch++;
//	for (i = 0; i < 6; i++)
//		pguid->Data4[i + 2] = 0;
//
//	for (i = 0; *pwch != L'}' && *pwch && i < 12; i++)
//		{
//		int digval = MsoValHexWch(*pwch++);
//		if (digval < 0)
//			return FALSE; //Wrong GUID format: non-hex character
//		pguid->Data4[(i / 2) + 2] = (BYTE)(pguid->Data4[(i / 2) + 2] * 16 + digval);
//		}
//
//	// NOTE: We don't check that *pwch == L'\0' or *pwch == L'}' because this changes
//	// the old behavior of this function and there are too many existing callers.
//	if (i != 12 || (fStartBrace && *pwch != L'}'))
//		return FALSE; //Wrong GUID format.

    std::abort();
	return TRUE;
}

/**
	MsoINumFromHebrewWz

	Only used by CDate in mso\dm
*/
int MsoINumFromHebrewWz(LPCWSTR wzHeb) noexcept
{
	int iRet = 0;
	const int cch = MsoCchWzLen(wzHeb);

	for (int i = 0; i < cch; i++)
		{
		if (wzHeb[i] == L'"' || wzHeb[i] == L'\'')
			continue;

		if (wzHeb[i] < 0x05D0 || wzHeb[i] > 0x05EA)
			{
			return(0);
			}

		if (wzHeb[i] > 0x05E6)
			{
			iRet += 100 * (wzHeb[i] - 0x05E6);
			}
		else if (wzHeb[i] > 0x05D8)
			{
			switch (wzHeb[i])
				{
			case 0x05D9:
				iRet += 10;
				break;

			case 0x05DA:
			case 0x05DB:
				iRet += 20;
				break;

			case 0x05DC:
				iRet += 30;
				break;

			case 0x05DD:
			case 0x05DE:
				iRet += 40;
				break;

			case 0x05DF:
			case 0x05E0:
				iRet += 50;
				break;

			case 0x05E1:
				iRet += 60;
				break;

			case 0x05E2:
				iRet += 70;
				break;

			case 0x05E3:
			case 0x05E4:
				iRet += 80;
				break;

			case 0x05E5:
			case 0x05E6:
				iRet += 90;
				break;

			default:
				{
				return(0);
				}
				}
			}
		else
			{
			iRet += wzHeb[i] - 0x05D0 + 1;
			}
		}

	if (cch > 3 && wzHeb[cch - 2] == L'"')		// year
		iRet += 5000;
	else if (iRet > 31)
		{
		return(0);
		}

	return(iRet);
}
//
///**
//	HrRfc1123DateToSystemTime
//*/
HRESULT HrRfc1123DateToSystemTime(_In_z_ const WCHAR* wzRfc1123Date, _Out_ SYSTEMTIME* pSystemTime) noexcept
{
//	HRESULT hRet  = S_OK;
//	SYSTEMTIME st;
//	WCHAR wzMonth[4];
//	WCHAR wzDayOfWeek[10];
//
//	const WCHAR *pwchCur, *pwchEnd;
//	DWORD cwchBufferLength;
//	int iRead;
//
//	// check arguments
//	if ((nullptr == wzRfc1123Date) || (nullptr == pSystemTime))
//		return E_INVALIDARG;
//
//	// clear sytem time contents
//	::memset(&st, 0, sizeof(st));
//
//	// first get DayOfWeek:
//	//
//	pwchCur = wcschr(wzRfc1123Date, L' ');
//	if(!pwchCur)
//	{
//		return E_INVALIDARG;
//	}
//
//	cwchBufferLength = static_cast<DWORD>(( (pwchCur - wzRfc1123Date) > 9 ) ? 9 : (pwchCur - wzRfc1123Date));
//	wcsncpy_s(wzDayOfWeek, _countof(wzDayOfWeek), wzRfc1123Date, cwchBufferLength);
//
//	// remove terminating ',' from day of week
//	if (cwchBufferLength > 0)
//		cwchBufferLength --;
//
//	wzDayOfWeek[cwchBufferLength] = L'\0';
//
//	//advance to the day of the month, and get it
//	while( (L'0' > *pwchCur) || (L'9' < *pwchCur) )
//	{
//		if (0 == *pwchCur)
//			return E_INVALIDARG;
//
//		pwchCur ++;
//	}
//
//	st.wDay = (WORD)_wtoi(pwchCur);
//
//	//advance to the month and get it:
//	pwchCur = wcschr(pwchCur, L' ');
//	if(!pwchCur)
//	{
//		return E_INVALIDARG; // no space delimiter between Day and Month in wzRfc1123Date
//	}
//	while(L' ' == *pwchCur)
//	{
//		pwchCur ++;
//	}
//
//	pwchEnd = wcschr(pwchCur, L' ');
//	if(!pwchEnd)
//	{
//		return E_INVALIDARG; // no space delimiter after Month in wzRfc1123Date
//	}
//
//	cwchBufferLength = static_cast<DWORD>(( (pwchEnd - pwchCur) > 3 ) ? 3 : (pwchEnd - pwchCur));
//	wcsncpy_s(wzMonth, _countof(wzMonth), pwchCur, cwchBufferLength);
//
//	wzMonth[cwchBufferLength] = L'\0';
//
//	//advance to the year and the rest, and get it
//	pwchCur = pwchEnd;
//	while( (L'0' > *pwchCur) || (L'9' < *pwchCur) )
//	{
//		if (0 == *pwchCur)
//			return E_INVALIDARG;
//
//		pwchCur ++;
//	}
//
//	// swscanf
//	iRead = swscanf_s(pwchCur,
//			  L"%hu %hu:%hu:%hu",
//			  &(st.wYear),
//			  &(st.wHour),
//			  &(st.wMinute),
//			  &(st.wSecond));
//
//	if ( iRead != 4 )
//	{
//		return E_INVALIDARG; // wzRfc1123Date does not contain all requisite fields
//	}
//
//	// set day of week
//	if (L'S' == wzDayOfWeek[0])
//	{
//		if (L'u' == wzDayOfWeek[1])
//			st.wDayOfWeek = 0;
//		else if (L'a' == wzDayOfWeek[1])
//			st.wDayOfWeek = 6;
//		else
//			return E_FAIL; // Invalid day of the week
//	}
//	else if (L'M' == wzDayOfWeek[0])
//		st.wDayOfWeek = 1;
//	else if (L'T' == wzDayOfWeek[0])
//	{
//		if (L'u' == wzDayOfWeek[1])
//			st.wDayOfWeek = 2;
//		else if (L'h' == wzDayOfWeek[1])
//			st.wDayOfWeek = 4;
//		else
//			return E_FAIL; // Invalid day of the week
//	}
//	else if (L'W' == wzDayOfWeek[0])
//		st.wDayOfWeek = 3;
//	else if (L'F' == wzDayOfWeek[0])
//		st.wDayOfWeek = 5;
//	else
//		return E_FAIL; // Invalid day of the week
//
//	// set month
//	if (L'J' == wzMonth[0])
//	{
//		if (L'a' == wzMonth[1])
//			st.wMonth = 1;
//		else if (L'u' == wzMonth[1])
//		{
//			if (L'n' == wzMonth[2])
//				st.wMonth = 6;
//			else if (L'l' == wzMonth[2])
//				st.wMonth = 7;
//			else
//				return E_FAIL; // Invalid month
//		}
//		else
//			return E_FAIL; // Invalid month
//	}
//	else if (L'F' == wzMonth[0])
//		st.wMonth = 2;
//	else if (L'M' == wzMonth[0])
//	{
//		if (L'r' == wzMonth[2])
//			st.wMonth = 3;
//		else if (L'y' == wzMonth[2])
//			st.wMonth = 5;
//		else
//			return E_FAIL; // Invalid month
//	}
//	else if (L'A' == wzMonth[0])
//	{
//		if ('p' == wzMonth[1])
//			st.wMonth = 4;
//		else if (L'u' == wzMonth[1])
//			st.wMonth = 8;
//		else
//			return E_FAIL; // Invalid month
//	}
//	else if (L'S' == wzMonth[0])
//		st.wMonth = 9;
//	else if (L'O' == wzMonth[0])
//		st.wMonth = 10;
//	else if (L'N' == wzMonth[0])
//		st.wMonth = 11;
//	else if (L'D' == wzMonth[0])
//		st.wMonth = 12;
//	else
//		return E_FAIL; // Invalid month
//
//	*pSystemTime = st;

    std::abort();
	return -1;
}
