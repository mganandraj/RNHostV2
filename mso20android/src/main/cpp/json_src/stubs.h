//
// Created by anandrag on 9/20/2023.
//

#ifndef ORNHOST_STUBS_H
#define ORNHOST_STUBS_H

BOOL SystemTimeToFileTime(const SYSTEMTIME* pst, FILETIME* pft);
BOOL FileTimeToSystemTime(const FILETIME* pft, SYSTEMTIME* pst);
void GetSystemTime(SYSTEMTIME *ioTime);

TASKLIBTECHDEBT_(HRESULT) HrMsoAllocHost(size_t cbAlloc, void **ppvRes, IMsoMemHeap* pmmh) noexcept;


#endif //ORNHOST_STUBS_H
