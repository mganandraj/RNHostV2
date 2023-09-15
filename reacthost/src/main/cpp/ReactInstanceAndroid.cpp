#include "ReactInstanceAndroid.h"
#include "JReactInstance.h"

using namespace facebook::jni;

namespace Mso::React {

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

void ReactInstanceAndroid::onInitialized() noexcept {
    if(Options().OnInstanceCreated)
        Options().OnInstanceCreated(*this);
}

void ReactInstanceAndroid::Initialize() noexcept
{
    ThreadScope ts;
    // Note: This is required as the AcitiveObject thread is a raw pthread thread which don't have a Java stack in it yet.
    // https://developer.android.com/training/articles/perf-jni#faq_FindClass
    ThreadScope::WithClassLoader([this](){
        ReactOptions optionsCopy = m_options;
        m_jOptions = make_global(JReactOptions::create(std::move(optionsCopy)));
        m_jReactInstance = make_global(JReactInstance::create(m_jOptions, this));
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
