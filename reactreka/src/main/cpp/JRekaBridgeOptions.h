#pragma once
#ifndef ORNHOST_JREKABRIDGEOPTIONS_H
#define ORNHOST_JREKABRIDGEOPTIONS_H

#include <fbjni/fbjni.h>
#include <reka/rekaApi.h>

namespace reactreka {

struct JRekaBridgeOptions : facebook::jni::HybridClass<JRekaBridgeOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/reka/RekaBridgeOptions;";

    Mso::JSHost::RekaBridgeOptions rekaOptions_;

    JRekaBridgeOptions(Mso::JSHost::RekaBridgeOptions &&options) :
            rekaOptions_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject>
    create(Mso::JSHost::RekaBridgeOptions &&rekaOptions);
};

}

#endif //ORNHOST_JREKABRIDGEOPTIONS_H
