#include <precomp.h>
#include "JRekaBridgeOptions.h"

using namespace facebook::jni;
using namespace Mso::JSHost;

namespace reactreka {

/*static*/ facebook::jni::local_ref<JRekaBridgeOptions::jhybridobject>
JRekaBridgeOptions::create(Mso::JSHost::RekaBridgeOptions &&rekaOptions) {
    return newObjectCxxArgs(std::move(rekaOptions));
}

}

// extern "C" int __cdecl swprintf_s(wchar_t * _Dst, size_t _SizeInWords, const wchar_t * _Format, ...) {std::abort();}
