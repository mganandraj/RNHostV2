#include "precomp.h"
#include "JReactViewInstance.h"
#include "ReactInstanceAndroid.h"
#include "JReactViewOptions.h"

using namespace facebook::jni;
using namespace Mso::React;

void JOfficeReactRootView::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JOfficeReactRootView::initHybrid),
    });
}

/*static*/ local_ref<JOfficeReactRootView::jhybriddata> JOfficeReactRootView::initHybrid(alias_ref<jhybridobject> jThis){
    return makeCxxInstance(std::move(jThis));
}

JOfficeReactRootView::JOfficeReactRootView(alias_ref<jhybridobject> jThis) {
    m_jThis = make_weak(std::move(jThis));
    m_ReactViewInstance = Mso::Make<OfficeReactViewInstance>(std::move(jThis));
}

Mso::Future<void> JOfficeReactRootView::Reload(const Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance, const Mso::React::ReactViewOptions&& viewOptions) noexcept {
    auto clazz = javaClassLocal();
    auto reloadMethod = clazz->getMethod<void(JReactInstance::jhybridobject, JReactViewOptions::jhybridobject)>("Reload");
    auto jViewOptions = JReactViewOptions::create(std::move(viewOptions)).release();
    ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(reactInstance.Get());
    reloadMethod(m_jThis.lockLocal(), instanceImpl->m_jReactInstance.get(), jViewOptions);
    return Mso::MakeSucceededFuture();
}

Mso::Future<void> JOfficeReactRootView::Unload() noexcept {

}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                     Mso::React::ReactViewOptions&& viewOptions) noexcept {
    // ThreadScope ts;
    ThreadScope::WithClassLoader([this, reactInstance, viewOptions2=std::move(viewOptions)]() {
        auto local = m_jThis.lockLocal();
        local->cthis()->Reload(std::move(reactInstance), std::move(viewOptions2));

    });
    return Mso::MakeSucceededFuture();
}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Unload() noexcept {

}