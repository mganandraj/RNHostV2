/**
	msoStringCloning.h

	Copyright (c) 1994 Microsoft Corporation

	Functions related to cloning strings
*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGCLONING_H
#define LIBLET_ROOT_MSOSTRINGCLONING_H
// #include <msoCore.h>
#include <core_root/msoStringUtils.h>
typedef struct IMsoMemHeap IMsoMemHeap;

/**
	
	String cloning (e.g. return a copy of the provided string)
	APIs provided for cloning rgch,sz,rgwch,wz.
	Note that file,line are handled automatically (just like 'new').

	wchar_t *wzNew = MsoWzCloneCore(wzIncoming, pmmh);
	wchar_t *wzThis = MsoWzCloneRgwchCore(rgwchOld, cchOld, pmmh);

*/

LIBLET_PUBLICAPI _Ret_opt_z_ _BindReturn_ MSOEXTERN_C CHAR* MsoSzCloneRgchCore(_In_opt_count_(cchLen) const CHAR *rgch, size_t cchLen, _In_opt_ IMsoMemHeap *pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI _Ret_opt_z_ _BindReturn_ MSOEXTERN_C WCHAR* MsoWzCloneRgwchCore(_In_opt_count_(cwchLen) const WCHAR *rgwch, size_t cwchLen, _In_opt_ IMsoMemHeap *pmmh) MSONOEXCEPT;

#define MsoSzCloneCore(sz, pmmh) MsoSzCloneExCore(sz, NULL, pmmh)
#define MsoWzCloneCore(wz, pmmh) MsoWzCloneExCore(wz, NULL, pmmh)

_Ret_opt_z_ _BindReturn_ __inline CHAR* MsoSzCloneExCore(_In_z_ const CHAR *sz, _Out_opt_ size_t *pcch, _In_opt_ IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	size_t cch = (size_t)MsoCchSzLen(sz);
	if (pcch != NULL) *pcch = cch;
	return MsoSzCloneRgchCore(sz, cch, pmmh);
	}

_Ret_opt_z_ _BindReturn_ __inline WCHAR* MsoWzCloneExCore(_In_z_ const WCHAR *wz, _Out_opt_ size_t *pcwch, _In_opt_ IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	size_t cwch = (size_t)MsoCchWzLen(wz);
	if (pcwch != NULL) *pcwch = cwch;
	return MsoWzCloneRgwchCore(wz, cwch, pmmh);
	}

// Clone a null-terminated string of known length (_In_z_count_?)
// TODO: should be __inline - all it does differently is assert
LIBLET_PUBLICAPI _Ret_opt_z_  _BindReturn_ MSOEXTERN_C WCHAR* MsoWzCloneLenCore(_In_opt_count_(cwchLen) const WCHAR *wz, size_t cwchLen, _In_opt_ IMsoMemHeap *pmmh) MSONOEXCEPT;

// Clone a string using the mark buffer
LIBLET_PUBLICAPI WCHAR * MsoWzCopyMarkCore(_In_z_ const WCHAR *wz, IMsoMemHeap *pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI BOOL MsoFMarkWzToWtzCore(const WCHAR *wz, _Deref_out_tz_ WCHAR **pwtz, IMsoMemHeap *pmmh) MSONOEXCEPT;

#define MsoSzClone(sz) MsoSzCloneCore(sz, NULL)
#define MsoWzClone(wz) MsoWzCloneCore(wz, NULL)
#define MsoSzCloneEx(sz, pcch)  MsoSzCloneExCore(sz, pcch, NULL)
#define MsoWzCloneEx(wz, pcwch) MsoWzCloneExCore(wz, pcwch, NULL)
#define MsoWzCloneLen(wz, cwchLen) MsoWzCloneLenCore(wz, cwchLen, NULL)
#define MsoSzCloneRgch(rgch, cchLen)    MsoSzCloneRgchCore(rgch, cchLen, NULL)
#define MsoWzCloneRgwch(rgwch, cwchLen) MsoWzCloneRgwchCore(rgwch, cwchLen, NULL)
__inline WCHAR * MsoWzCopyMark(const WCHAR* wz) MSONOEXCEPT { return MsoWzCopyMarkCore(wz, NULL); }

__inline BOOL MsoFMarkWzToWtz(const WCHAR *wz, _Deref_out_tz_ WCHAR **pwtz) MSONOEXCEPT
{
	return(MsoFMarkWzToWtzCore(wz, pwtz, NULL));
}

/**
	
	String conversion into a new string
	APIs provided for rgch,sz,rgwch,wz.
	Note that file,line are handled automatically (just like 'new').

	char *szNew = MsoSzMarkWzCore(wzIncoming, pmmh);
	char *szThis = MsoSzMarkRgwchCore(rgwchOld, cchOld, pmmh);

*/

LIBLET_PUBLICAPI _Ret_opt_z_ _Ret_opt_count_(*pcch+1) _BindReturn_ MSOEXTERN_C CHAR* MsoSzMarkRgwchCore(_Out_opt_ int *pcch, _In_count_(cch) const WCHAR *rgwch, int cch, IMsoMemHeap *pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI _Ret_opt_z_ _Ret_opt_count_(*pcch+1) _BindReturn_ MSOEXTERN_C WCHAR* MsoWzMarkRgchCore(_Out_opt_ int *pcch, _In_count_(cch) const CHAR *rgch, int cch, IMsoMemHeap *pmmh) MSONOEXCEPT;
LIBLET_PUBLICAPI _Ret_opt_z_ _BindReturn_ MSOEXTERN_C WCHAR* MsoWtzMarkRgchCore(_In_count_(cch) const CHAR *rgch, int cch, IMsoMemHeap *pmmh) MSONOEXCEPT;

_Ret_opt_z_ _BindReturn_ __inline CHAR* MsoSzMarkWtzCore(_In_tz_ const WCHAR* wtz, IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	return MsoSzMarkRgwchCore(NULL, MsoWzFromWtz(wtz), MsoCchWtzLen(wtz), pmmh);
	}

_Ret_opt_z_ _BindReturn_ __inline CHAR* MsoSzMarkWzCore(_In_z_ const WCHAR *wz, IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	return MsoSzMarkRgwchCore(NULL, wz, MsoCchWzLen(wz), pmmh);
	}

_Ret_opt_z_ _BindReturn_ __inline WCHAR* MsoWzMarkSzCore(_In_z_ const CHAR *sz, IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	return MsoWzMarkRgchCore(NULL, sz, MsoCchSzLen(sz), pmmh);
	}

_Ret_opt_tz_ _BindReturn_ __inline WCHAR* MsoWtzMarkSzCore(_In_z_ const CHAR *sz, IMsoMemHeap *pmmh) MSONOEXCEPT
	{
	return MsoWtzMarkRgchCore(sz, MsoCchSzLen(sz), pmmh);
	}

#define MsoSzMarkWz(wz) MsoSzMarkWzCore(wz, NULL)
#define MsoWzMarkSz(sz) MsoWzMarkSzCore(sz, NULL)
#define MsoWtzMarkSz(sz) MsoWtzMarkSzCore(sz, NULL)








/**
	
	Random glue that should be ignored

*/

// Legacy functions - remove at some point
#define MsoWzCloneMemHeap(wz, pmmh) MsoWzCloneExCore(wz, NULL, pmmh)
#define MsoWzCloneRgwchMemHeap(rgwch, cwchLen, pmmh) MsoWzCloneRgwchCore(rgwch, cwchLen, pmmh)
#define MsoOzMarkWz(wz) ((OLECHAR*)(wz))
#define MsoReleaseOzWz(oz)
#define MsoWzMarkOz(oz) ((WCHAR*)(oz))
#define MsoReleaseWzOz(wz)
#define MsoWzCreateFromWz(wz) MsoWzCloneCore(wz, nullptr)

#endif // LIBLET_ROOT_MSOSTRINGCLONING_H
