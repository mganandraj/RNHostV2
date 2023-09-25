//
// Created by anandrag on 9/24/2023.
//

#ifndef ORNHOST_REACT_ANDROID_H
#define ORNHOST_REACT_ANDROID_H

#include "React.h"

namespace facebook {

namespace jsi {
    class Runtime;
}

namespace react {
    class CallInvoker;
}

}

namespace Mso::React {
facebook::jsi::Runtime* GetJsiRuntime(IReactInstance& instance) noexcept;
std::shared_ptr<facebook::react::CallInvoker> GetJSThreadCallInvoker(IReactInstance& instance) noexcept;
}

#endif //ORNHOST_REACT_ANDROID_H
