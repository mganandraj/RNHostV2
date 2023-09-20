//
// Created by anandrag on 9/20/2023.
//

#ifndef ORNHOST_PRECOMP_H
#define ORNHOST_PRECOMP_H

#include <smartPtr/cntPtr.h>

namespace Mso{
    template<typename T> using TCntPtr = Mso::CntPtr<T>;
}




#include <vector>
#include <stack>
#include <cerrno>
#include <cstdint>

#include <guid/msoGuid.h>
#include <platformAdapters/IUnknownShim.h>

#include <crash/verifyElseCrash.h>

#include <sal.h>

typedef int64_t __int64;

// #define MS_TARGET_ANDROID

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#define SafeMin __min

#define DECLARE_COPYCONSTR_AND_ASSIGNMENT( C )  explicit C( const C& ); void operator=( const C& )
#define MSO_NO_COPYCONSTR( C )  C(const C&) = delete;
#define MSO_NO_ASSIGNMENT( C ) const C& operator=(const C&) = delete;
#define MSO_NO_DEFAULTCONSTR( C ) explicit C() = delete;


#define LIBLET_PUBLICAPI
#define MSOEXTERN_C extern "C"
#define MSONOEXCEPT noexcept
typedef char CHAR;

#include <cstdint>
#include "sal.h"

#define ARRAYSIZE(ary) (sizeof(ary) / sizeof(ary[0]))
#define cElements(ary) ARRAYSIZE(ary)
#define RgC(pszBuffer) (pszBuffer), cElements(pszBuffer)

#if !defined(_countof)
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE (0L)
#endif

#ifndef TRUE
#define TRUE (1L)
#endif

#define CONST const

typedef char CHAR;
typedef const CHAR *LPCSTR;

typedef _Null_terminated_ CHAR *NPSTR, *LPSTR, *PSTR;
typedef _Null_terminated_ PSTR *PZPSTR;
typedef _Null_terminated_ CONST PSTR *PCZPSTR;
typedef _Null_terminated_ CONST CHAR *LPCSTR, *PCSTR;
typedef _Null_terminated_ PCSTR *PZPCSTR;
typedef _Null_terminated_ CONST PCSTR *PCZPCSTR;

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef int64_t      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef uint64_t   UINT64, *PUINT64;

//
// The following types are guaranteed to be signed and 32 bits wide.
//

typedef signed int LONG32, *PLONG32;

//
// The following types are guaranteed to be unsigned and 32 bits wide.
//

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;


typedef int             BOOL;
typedef long            LONG;
typedef short           SHORT;
typedef unsigned long   DWORD;
typedef unsigned short  WORD;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef BYTE*           PBYTE;
typedef char*           PSTR;
typedef int             INT;
typedef INT*            PINT;
typedef wchar_t         WCHAR;
typedef WCHAR*          PWCHAR;
typedef const WCHAR*    PCWCHAR;
typedef WCHAR*          LPWSTR;
typedef const WCHAR*    LPCWSTR;
typedef unsigned long long ULONGLONG;

typedef WCHAR * PWSTR;
typedef const  WCHAR * PCWSTR;

typedef unsigned long   LCHAR;
typedef WORD            GLYPHID;

typedef DWORD           OPENTYPE_TAG;

typedef unsigned short  SHAPING_CLUSTERMAP;
typedef void*           SHAPING_HANDLE;



constexpr inline size_t CchMaxISO8601Time = 24;

#define UINT64_MAX std::numeric_limits<uint64_t>::max()
#define INT64_MAX std::numeric_limits<int64_t>::max()
#define INT64_MIN std::numeric_limits<int64_t>::min()

typedef long LONG;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;

#define _TRUNCATE ((size_t)-1)

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, * LPFILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

BOOL SystemTimeToFileTime(const SYSTEMTIME* pst, FILETIME* pft);
BOOL FileTimeToSystemTime(const FILETIME* pft, SYSTEMTIME* pst);
void GetSystemTime(SYSTEMTIME *ioTime);

typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

#define OACR_ASSUME

#define OACR_WARNING_SUPPRESS(a,b)
#define OACR_WARNING_DISABLE(a,b)
#define OACR_WARNING_ENABLE(a,b)
#define UNREFERENCED_OACR(a) ((void)a)
#define OACR_WARNING_PUSH
#define OACR_WARNING_POP
#define OACR_REVIEWED_CALL(dev, exp) (exp)

#define AssertTag(x, y) // if(!x) { std::abort(); }
#define AssertSzTag(x, y, z) // if(!x) { std::abort(); }
#define AssertDoTag(f, tag)	(void)(f)

#define ShipAssertTag(f, tag) if(!x) { std::abort(); }
#define ShipAssertSzTag(x, y, z)  if(!x) { std::abort(); }
#define ShipAssertSz1Tag(f, sz, a, tag)  if(!f) { std::abort(); }
#define ShipAssertSz2Tag(f, sz, a, b, tag)  if(!f) { std::abort(); }
#define ShipAssertSz3Tag(f, sz, a, b, c, tag) if(!f) { std::abort(); }
#define ShipAssertSz4Tag(f, sz, a, b, c, d, tag)  if(!f) { std::abort(); }
#define ShipAssertSz5Tag(f, sz, a, b, c, d, e, tag)if(!f) { std::abort(); }

#define _Deref_out_tz_
#define _In_tz_
#define _Ret_opt_tz_

/* Checks if wtz is a valid string */
#if DEBUG
MSOAPI_(BOOL) MsoFValidWtz(const WCHAR* wtz);
#else
#define MsoFValidWtz(wtz) (1)
#endif

#define _Post_tz_



typedef int LCID;
#define LANGIDFROMLCID(lcid)   ((USHORT)(lcid))

#define CP_ACP		0
#define CP_OEMCP	1
#define CP_MACCP	2

#define TASKLIBTECHDEBT_(t) MSOEXTERN_C t
#define MSODLL_MANDATED_C_EXPORT MSOEXTERN_C

#define _In_NLS_string_(a)
typedef const CHAR *LPCCH, *PCCH;
typedef const WCHAR *LPCWCH, *PCWCH;
typedef _Null_terminated_ CHAR *NPSTR, *LPSTR, *PSTR;
typedef BOOL  *LPBOOL;

#define _In_t_
#define _In_tz_
#define _Out_t_cap_(a)
#define _Out_tz_cap_(a)

typedef WORD LID; // language id


#define _In_opt_tz_
#define _Inout_tz_cap_(a)
#define _Inout_t_cap_(a)

#define _Inout_tz_

#define MSONOEXCEPT noexcept

typedef intptr_t INT_PTR;
typedef uintptr_t UINT_PTR;


//
// SCRIPT_ID Defined in mlang.h (temporarily defining here till mlang.h is available)
//
typedef BYTE SCRIPT_ID;

#define LOCALE_NAME_MAX_LENGTH   85

#define LOCALE_NAME_USER_DEFAULT            NULL
#define LOCALE_NAME_INVARIANT               L""
#define LOCALE_NAME_SYSTEM_DEFAULT          L"!x-sys-default-locale"

struct IMsoMemHeap
{
    STDMETHOD_(HRESULT, HrAlloc)(size_t cbAlloc, _Out_ _Deref_post_bytecap_(cbAlloc) void** ppvOut) = 0;
    STDMETHOD_(HRESULT, HrRealloc)(size_t cbNew, _Inout_ void** ppvMem) = 0;
    STDMETHOD_(void, Free)(void* pvFree) = 0;

    // destroy the class instance
    STDMETHOD_(void, Destroy)() = 0;

#ifdef DEBUG
    STDMETHOD_(BOOL, FMemHeapSaveBe)(void* pinst, LPARAM lParam, const void* pvBlock, LONG_PTR cb) = 0;
	STDMETHOD_(size_t, CbSizePv)(const void* pvMem) = 0;
#endif // DEBUG
};

//TASKLIBTECHDEBT_(HRESULT) HrMsoAllocHost(size_t cbAlloc, void **ppvRes, IMsoMemHeap* pmmh) noexcept
//{
//    std::abort();
//}

typedef unsigned char UCHAR;
#define MakeWordFirstSecond(b1, b2) ((WORD)(b1) << 8 | (WORD)(UCHAR)(b2))


typedef unsigned short USHORT;
#define MAKELANGID(p, s)       ((((USHORT)(s)) << 10) | (USHORT)(p))
#define PRIMARYLANGID(lgid)    ((USHORT)(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((USHORT)(lgid) >> 10)

#define ASCII           0x007f

#define DWORD_PTR               DWORD
#define ULONG_PTR               ULONG

#define MAKEWORD(a, b)          ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)          ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)               ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)               ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)               ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)               ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define wcsnlen_s(a,b) wcsnlen(a,b)

#endif //ORNHOST_PRECOMP_H
