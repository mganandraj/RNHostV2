#include "ReactInstanceAndroid.h"

using namespace facebook::jni;

namespace Mso::React {

/*static */facebook::jni::local_ref<JReactInstance::jhybriddata> JReactInstance::initHybrid(facebook::jni::alias_ref<jhybridobject> jThis) {
    return makeCxxInstance();
}

/*static */void JReactInstance::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", JReactInstance::initHybrid),
    });
}

/*static*/ facebook::jni::local_ref<JReactInstance::jhybridobject> JReactInstance::create(facebook::jni::alias_ref<JReactOptions::jhybridobject> options) {
        return JReactInstance::newObjectJavaArgs(options);
}

ReactInstanceAndroid::ReactInstanceAndroid(IReactHost& reactHost, ReactOptions&& options) noexcept
    : Super{reactHost.NativeQueue()}
    , m_weakReactHost(&reactHost)
    , m_options(std::move(options))
{
}

ReactInstanceAndroid::~ReactInstanceAndroid() noexcept
{
    Destroy();
}

void ReactInstanceAndroid::Initialize() noexcept
{
    ThreadScope ts;
    // Note: This is required as the AcitiveObject thread is a raw pthread thread which don't have a Java stack in it yet.
    // https://developer.android.com/training/articles/perf-jni#faq_FindClass
    ThreadScope::WithClassLoader([this](){
        m_jOptions = make_global(JReactOptions::create(std::move(m_options)));
        m_jReactInstance = make_global(JReactInstance::create(m_jOptions));
    });
}

//! This method must be called from the native queue.
Mso::Future<void> ReactInstanceAndroid::Destroy() noexcept
{
    // Destroy the hybrid object
    return Mso::MakeSucceededFuture();
}

const std::string& ReactInstanceAndroid::JsBundleName() const noexcept
{
    return Options().Identity;
}

std::string ReactInstanceAndroid::SDXBasePath() const noexcept
{
    return Options().SDXBasePath;
}

const ReactOptions& ReactInstanceAndroid::Options() const noexcept
{
    return m_jOptions->cthis()->Options();
}

}; // namespace Mso::React
