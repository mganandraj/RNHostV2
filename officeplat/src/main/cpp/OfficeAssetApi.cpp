#include <dlfcn.h>
#include <wchar16/wc16_wstring.h>

void* loadCommonLibrary(const wchar_t *libNickNameW, int flags) {
    // Convert to string
    std::wstring libNickNameW_(libNickNameW);
    std::string libNickName(libNickNameW_.begin(), libNickNameW_.end());

    // Make it look like lib{nickname}.so
    libNickName.insert(0, "lib");
    libNickName.append(".so");

    dlerror();
    void* handle =  dlopen(libNickName.c_str(), flags);
    char*error = dlerror();
    return handle;
}