#include "precomp.h"
#include "JReactViewInstance.h"
#include "ReactInstanceAndroid.h"
#include "JReactViewOptions.h"
#include "JMsoFuture.h"

using namespace facebook::jni;
using namespace Mso::React;


#define LOG_TAG "JReactViewInstance"

#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, __VA_ARGS__)


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
    auto future = JMsoFuture::create();
    auto msoFuture = future->cthis()->getMsoFuture();

    auto reloadMethod = javaClassLocal()->getMethod<void(JReactInstance::jhybridobject, JReactViewOptions::jhybridobject, JMsoFuture::jhybridobject)>("Reload");
    reloadMethod(m_jThis.lockLocal(), static_cast<ReactInstanceAndroid*>(reactInstance.Get())->m_jReactInstance.get(), JReactViewOptions::create(std::move(viewOptions)).release(), future.release());
    return msoFuture;
}

Mso::Future<void> JOfficeReactRootView::Unload() noexcept {
    auto future = JMsoFuture::create();
    auto msoFuture = future->cthis()->getMsoFuture();

    auto unloadMethod = javaClassLocal()->getMethod<void(JMsoFuture::jhybridobject)>("Unload");
    unloadMethod(m_jThis.lockLocal(), future.release());
    return msoFuture;
}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                     Mso::React::ReactViewOptions&& viewOptions) noexcept {

    Mso::Future<void> result;
    ThreadScope::WithClassLoader([this, &result, reactInstance, viewOptions2=std::move(viewOptions)]() {
        auto local = m_jThis.lockLocal();
        result = local->cthis()->Reload(std::move(reactInstance), std::move(viewOptions2));
    });

    return result;
}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Unload() noexcept {
    Mso::Future<void> result;
    ThreadScope::WithClassLoader([this, &result]() {
        auto local = m_jThis.lockLocal();
        result = local->cthis()->Unload();

    });

    return result;
}