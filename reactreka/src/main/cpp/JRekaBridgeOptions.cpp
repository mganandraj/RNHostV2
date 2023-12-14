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
