//
// Created by anandrag on 9/20/2023.
//

#include "precomp.h"
#include "stubs.h"
#include <cstdlib>

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

