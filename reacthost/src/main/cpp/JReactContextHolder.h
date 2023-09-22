//
// Created by anandrag on 9/21/2023.
//

#ifndef ORNHOST_JREACTCONTEXTHOLDER_H
#define ORNHOST_JREACTCONTEXTHOLDER_H

#include <fbjni/fbjni.h>

struct JReactContextHolder : public facebook::jni::JavaClass<JReactContextHolder> {
    constexpr static auto kJavaDescriptor =
            "Lcom/microsoft/office/reacthost/ReactContextHolder;";
};

#endif //ORNHOST_JREACTCONTEXTHOLDER_H
