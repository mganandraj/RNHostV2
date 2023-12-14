/**
	stringConversion.cpp

	Copyright (c) 1994 Microsoft Corporation

	Functions related to converting strings
*/
#pragma once
#include "core_root/msoStringConversion.h"

LIBLET_PUBLICAPI int TranslateCpRgchCore(const CHAR *rgchFrom, WORD cbFrom, UINT CodePageFrom, 
	_Out_bytecap_(cbTo) CHAR *rgchTo, WORD cbTo, UINT CodePageTo, IMsoMemHeap *pmmh) noexcept;

// Huffman Table Resource format
struct MSODSTE
	{   
	// Decode State Table Entry
	WORD mpbidste[2];
	WCHAR rgwchKey[4];
	};

LIBLET_PUBLICAPI WORD DecompressToWzCore(UINT CodePage, const MSODSTE *rgdste, const BYTE *rgIn, const WORD cchIn, 
	_Out_cap_x_(flags affect meaning of cch) BYTE *rgParam, const WORD cchOut, 
	BYTE flags, IMsoMemHeap *pmmh) noexcept;

LIBLET_PUBLICAPI int SimpleDecompressToWtzCore(const BYTE *rgch, WORD cb, 
	_Out_cap_x_("flags & flagLen ? cch+1 : cch") WCHAR *wtz, int cch, 
	BYTE flags, IMsoMemHeap *pmmh) noexcept;

/**
	RgwchLowerFast

	Convert a range of Unicode text to lower case. Unlike MsoWzLowerCore, this
	only operates on the low Ascii characters. Works in place - ???.
*/
__inline void RgwchLowerFast(_In_count_(cch) const WCHAR *rgwchSrc, _Out_cap_(cch) WCHAR *rgwchDst, int cch) noexcept
{
	WCHAR wch;
	AssertTag(rgwchSrc != NULL && rgwchDst != NULL, 0x1f545688 /* tag_5vf0i */);

	for (; cch > 0; cch--)
		{
		wch = *rgwchSrc++;
		if (wch >= 'A' && wch <= 'Z')
			wch += ('a' - 'A');
		*rgwchDst++ = wch;
		}
}

LIBLET_PUBLICAPI BOOL FSzFromWzCore(UINT CodePage, const WCHAR *wz, char **psz, DWORD cbFudge, IMsoMemHeap *pmmh) noexcept;

inline BOOL FSzFromWz(UINT CodePage, const WCHAR *wz, char **psz, DWORD cbFudge) noexcept
{
	return(FSzFromWzCore(CodePage, wz, psz, cbFudge, NULL /*pmmh*/));
}
