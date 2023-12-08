#include <cstddef>
#include <cstdlib>
#include "stub.h"

extern "C" int __cdecl swprintf_s(wchar_t * _Dst, size_t _SizeInWords, const wchar_t * _Format, ...) {std::abort();}