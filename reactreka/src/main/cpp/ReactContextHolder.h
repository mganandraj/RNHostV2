#pragma once
#ifndef ORNHOST_REACTCONTEXTHOLDER_H
#define ORNHOST_REACTCONTEXTHOLDER_H

#include <fbjni/fbjni.h>

namespace reactreka {
struct ReactContextHolder : public facebook::jni::JavaClass<ReactContextHolder> {
    constexpr static auto kJavaDescriptor =
            "Lcom/microsoft/office/reactreka/ReactContextHolder;";
};
}


#endif //ORNHOST_REACTCONTEXTHOLDER_H
