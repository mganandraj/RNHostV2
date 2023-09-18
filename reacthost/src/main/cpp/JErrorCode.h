//
// Created by anandrag on 9/18/2023.
//

#ifndef ORNHOST_JERRORCODE_H
#define ORNHOST_JERRORCODE_H

#include <fbjni/fbjni.h>
#include <errorCode/errorCode.h>

struct JErrorCode : facebook::jni::HybridClass<JErrorCode>  {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ErrorCode;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::ErrorCode&&);

    std::string toString();

    JErrorCode(Mso::ErrorCode &&errorCode) : errorCode_(std::move(errorCode)) {}
    Mso::ErrorCode errorCode_;
};


#endif //ORNHOST_JERRORCODE_H
