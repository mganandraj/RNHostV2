//
// Created by anandrag on 9/16/2023.
//

#ifndef ORNHOST_GETRUNTIMEEXECUTOR_ANDROID_H
#define ORNHOST_GETRUNTIMEEXECUTOR_ANDROID_H

#include "ReactNativeHost/React.h"
#include <ReactCommon/RuntimeExecutor.h>

namespace Mso::React {
facebook::react::RuntimeExecutor GetRuntimeExecutor2(Mso::React::IReactInstance &instance) noexcept;
}

#endif //ORNHOST_GETRUNTIMEEXECUTOR_ANDROID_H
