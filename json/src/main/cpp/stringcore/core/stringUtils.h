/**
	stringUtils.h

	Copyright (c) 1994 Microsoft Corporation

	Private locale-invariant functions related to working with strings:
		string copy
		string append
		string search
		string length
		string replace
*/
#pragma once
#include <core_root/msoStringUtils.h>

LIBLET_PUBLICAPI char* SzTruncCopy(_Out_z_cap_(cchDst) CHAR *szDst, const CHAR *szSrc, unsigned cchDst) noexcept;
LIBLET_PUBLICAPI _Ret_z_ WCHAR *WzTruncCopy(_Out_z_cap_(cchDst) WCHAR *wzDst, _In_z_ const WCHAR *wzSrc, unsigned cchDst) noexcept;
LIBLET_PUBLICAPI BOOL FWchInRgwchAscending(WCHAR wch, const WCHAR *rgwch, int cwch) noexcept;
LIBLET_PUBLICAPI char* PchStrStrFast(const char *pch, int cch, const char *pchFind, int cchFind) noexcept;
