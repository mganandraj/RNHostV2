#include <cstddef>
#include <cstdlib>
#include "stub.h"
#include "precomp.h"

extern "C" int __cdecl swprintf_s(wchar_t * _Dst, size_t _SizeInWords, const wchar_t * _Format, ...) {std::abort();}

BOOL SystemTimeToFileTime(const SYSTEMTIME* pst, FILETIME* pft)
{
    std::abort();
}

BOOL FileTimeToSystemTime(const FILETIME* pft, SYSTEMTIME* pst) {
    std::abort();
}

void GetSystemTime(SYSTEMTIME *ioTime) {
    std::abort();
}

TASKLIBTECHDEBT_(HRESULT) HrMsoAllocHost(size_t cbAlloc, void **ppvRes, IMsoMemHeap* pmmh) noexcept
{
std::abort();
}
