#include "precomp.h"
#include "jni.h"
#include <ReactNativeHost/React.h>
#include <ReactNativeHost/React_Android.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL Java_com_microsoft_office_reacthost_ReactHostStatics_makeReactHost
        (JNIEnv *, jobject, jobject jOptions) {
    auto options = Mso::React::ToReactOptions(reinterpret_cast<Mso::React::JReactOptions_::JavaPart::javaobject>(jOptions));
    Mso::TCntPtr<Mso::React::IReactHost> reactHost = Mso::React::MakeReactHost(std::move(options));
    return Mso::React::ToJReactHost(reactHost);
}

#ifdef __cplusplus
}
#endif
