#include "precomp.h"

#include <errorCode/hresultErrorProvider.h>
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

void ReactInstanceAndroid::onInitialized(facebook::jni::alias_ref<JReactContextHolder> contextHolder) noexcept {
    m_jReactContextHolder = make_global(contextHolder);
    if(Options().OnInstanceCreated)
        Options().OnInstanceCreated(*this);
}

void ReactInstanceAndroid::onBundleLoaded(std::string&& /*bundleName*/) noexcept {
    if(Options().OnInstanceLoaded)
        Options().OnInstanceLoaded(*this, Mso::HResultErrorProvider().MakeErrorCode(S_OK));
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


void ReactInstanceAndroid::NotifyRekaInitialized() noexcept
{
    // We want to make sure that notify about Reka initialization only once and only after the ReactInstance
    // is loaded and the Reka is initialized. Thus, this method is called when the ReactInstance
    // is successfully loaded and when the Reka is initialized because we do not know what comes first.
    if (m_isLoaded && !m_isRekaInitialized)
    {
        if (auto rekaContext = m_rekaContext.Load().Get())
        {
            m_isRekaInitialized = true;

            m_rekaContextProxy->SetContext(*rekaContext);
            if (auto onRekaInitialized = m_options.OnInitialized.Get())
            {
                onRekaInitialized->Invoke(*rekaContext);
            }
        }
    }
}

void ReactInstanceAndroid::NotifyRekaDestroyed() noexcept
{
    if (m_isRekaInitialized)
    {
        m_isRekaInitialized = false;

        if (auto rekaContext = m_rekaContext.Load().Get())
        {
            if (auto onRekaDestroyed = m_options.OnDestroyed.Get())
            {
                onRekaDestroyed->Invoke(*rekaContext);
            }
        }

        m_rekaContextProxy->SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    }
}

Mso::JSHost::RekaBridgeOptions ReactInstanceAndroid::createRekaBridgeOptions() noexcept
{
    Mso::JSHost::RekaBridgeOptions rekaOptions {};
    const auto& reactOptions = Options();
    if (Options().RekaProviderFactory)
    {
        // Custom Provider Factory
        rekaOptions.RekaServiceProviderFactory = reactOptions.RekaProviderFactory;
    }
    else
    {
        // Default Provider Factory
        rekaOptions.RekaServiceProviderFactory =
                [rekaServiceProviderName = reactOptions.DataServiceProviderName](Mso::JSHost::IRekaContext& context) mutable noexcept
                {
                    return Mso::JSHost::GetOrMakeRekaServiceProvider(rekaServiceProviderName.c_str(), context);
                };
    }

    rekaOptions.OnInitialized = [weakThis = Mso::WeakPtr(this)](Mso::JSHost::IRekaContext& context) noexcept
    {
        if (auto strongThis = weakThis.GetStrongPtr())
        {
            strongThis->m_rekaContext.Store(&context);
            strongThis->NotifyRekaInitialized();
        }
    };

    rekaOptions.OnDestroyed = [weakThis = Mso::WeakPtr(this)](Mso::JSHost::IRekaContext& /*context*/) noexcept
    {
        if (auto strongThis = weakThis.GetStrongPtr())
        {
            strongThis->NotifyRekaDestroyed();
            strongThis->m_rekaContext.Exchange(nullptr);
        }
    };

    // Note:: We are configuring JsExecutor on native module initialization.
//    rekaOptions.JsExecutor = Mso::Make<reactreka::ReactNativeRekaBridge, Mso::JSHost::IRekaBridge>(
//            reactreka::ReactContextHolder:: m_jReactContextHolder /*reinterpret_cast ??*/);

    rekaOptions.NativeQueue = Queue();
    return rekaOptions;
}

}; // namespace Mso::React
