/*-------------------------------------------------------------------------
	stringParsing.h

	Copyright (c) 1994 Microsoft Corporation

	Functions related to parsing data to/from strings

	%%Owner: AskIntl
	%%Id: 96dc3da1-b7a4-424b-bdc2-7c21d7423405
-------------------------------------------------------------------------*/
#pragma once
#include "stringcore/parsing.h"

LIBLET_PUBLICAPI int IDecodeDigit(double dDec, double dHolder) noexcept;
LIBLET_PUBLICAPI int IDigitValueOfWch(WCHAR wch) noexcept;
int ParseIntSz(const CHAR *rgch, int *pdw, BOOL fSigned) noexcept;
int ParseIntWz(const WCHAR *rgwch, int *pdw, BOOL fSigned) noexcept;
unsigned ParseHexUIntWz(const WCHAR *rgwch, unsigned *pu) noexcept;
void MsoHexFromCh(_Out_cap_(cchHex) WCHAR *pchHex, int cchHex, WCHAR ch) noexcept;

inline int ValHexCh(char ch) noexcept
	{ 
	return MsoValHexWch(static_cast<WCHAR>(ch)); 
	}

extern const char c_rgchHex[];

/*---------------------------------------------------------------------------
	REVIEW: can't we just memcmp them?
--------------------------------------------------------------------RajM---*/
__inline int SgnCompareSystemTime(const SYSTEMTIME *pst1, const SYSTEMTIME *pst2) noexcept
{
	int sgn = 2;

	AssertTag(pst1 && pst2, 0x1f545685 /* tag_5vf0f */);
	if (pst1 && pst2)
		{
		sgn = pst1->wYear > pst2->wYear ? 1 : (pst1->wYear < pst2->wYear ? -1 : 0);

		if (sgn == 0)
			{
			sgn = pst1->wMonth > pst2->wMonth ? 1 : (pst1->wMonth < pst2->wMonth ? -1 : 0);

			if (sgn == 0)
				{
				sgn = pst1->wDay > pst2->wDay ? 1 : (pst1->wDay < pst2->wDay ? -1 : 0);

				if (sgn == 0)
					{
					sgn = pst1->wHour > pst2->wHour ? 1 : (pst1->wHour < pst2->wHour ? -1 : 0);

					if (sgn == 0)
						{
						sgn = pst1->wMinute > pst2->wMinute ? 1 : (pst1->wMinute < pst2->wMinute ? -1 : 0);

						if (sgn == 0)
							{
							sgn = pst1->wSecond > pst2->wSecond ? 1 : (pst1->wSecond < pst2->wSecond ? -1 : 0);

							if (sgn == 0)
								sgn = pst1->wMilliseconds > pst2->wMilliseconds ? 1 :
									(pst1->wMilliseconds < pst2->wMilliseconds ? -1 : 0);
							}
						}
					}
				}
			}
		}

	return sgn;
}

/*-----------------------------------------------------------------------------
	SgnCompareDatesWzWz

	Compares 2 date strings, in format "yyyy-MM-dd hh:mm:ss "

	%%Owner: RAJM
	%%Id: 77838678-0f5b-40a2-96d9-ce37b8519059
--------------------------------------------------------------------RajM-----*/
__inline int SgnCompareDatesWzWz(const WCHAR *wz1, const WCHAR *wz2) noexcept
{
	if (!wz1 ||!wz2)
		return wz1 ? 1 : (wz2 ? -1 : 0);

	SYSTEMTIME st1 = {0}, st2 = {0};
	AssertDoTag(MsoParseTimeWz(wz1, &st1), 0x1f545684 /* tag_5vf0e */);
	AssertDoTag(MsoParseTimeWz(wz2, &st2), 0x1f545683 /* tag_5vf0d */);
	return SgnCompareSystemTime(&st1, &st2);
}

