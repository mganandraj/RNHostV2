#include "precomp.h"

#include <fbjni/detail/utf8.h>
#include <dlfcn.h>

using namespace facebook::jni;

void* loadCommonLibrary(const wchar_t *fileNameW, int flags) {
    std::wstring fileNameW_(fileNameW);
    std::string fileNameA(fileNameW_.begin(), fileNameW_.end());
    dlerror();
    void* handle =  dlopen(fileNameA.c_str(), flags);
    char*error = dlerror();
    return handle;

}