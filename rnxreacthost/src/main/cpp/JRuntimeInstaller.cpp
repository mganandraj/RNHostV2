#include "ReactNativeHeaders.h"
#include "JRuntimeInstaller.h"

namespace facebook {
namespace react {

void JRuntimeInstaller::DoInstall(facebook::jni::local_ref<JJsiRuntimeRef::JavaPart> jsiRuntimeRef) {
  static const auto method = getClass()->getMethod<void(facebook::jni::local_ref<JJsiRuntimeRef::JavaPart>)>("DoInstall");
  method(self(), jsiRuntimeRef);
}

} // namespace react
} // namespace facebook
