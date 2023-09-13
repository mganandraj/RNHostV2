//
// Created by Anandraj Govindan on 9/4/23.
//

#ifndef ornhost_JREACTVIEWOPTIONS_H
#define ornhost_JREACTVIEWOPTIONS_H

#include "fbjni/fbjni.h"
#include <ReactNativeHost/React.h>

struct JReactViewOptions : facebook::jni::HybridClass<JReactViewOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewOptions;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    JReactViewOptions(Mso::React::ReactViewOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(const Mso::React::ReactViewOptions&&);
    Mso::React::ReactViewOptions options_;

    std::string getComponentName();
    void setComponentName(std::string componentName);

    std::string getInitialProps();
    void setInitialProps(std::string initialProps);

    const Mso::React::ReactViewOptions& Options() const noexcept;

};


#endif //ornhost_JREACTVIEWOPTIONS_H
