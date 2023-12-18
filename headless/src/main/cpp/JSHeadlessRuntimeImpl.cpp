#include "precomp.h"

// #include "JSHeadlessRuntimeHelpers.h"
#include "JSHeadlessRuntimeImpl.h"

// #include <jshostassets/assetconfiguration.h>
#include <JSHostOfficePal/PlatformJSBundles.h>
#include <ReactNativeHost/JSBundle.h>
#include <Reka/RekaServiceRegistration.h>
#include <ReactNativeHost/React.h>

// #include <osfclient/osfreacthelper.h>
// #include <osfclient/hostintegrationmanager.h>
// #include <jshostrichapids/IRichApiProxyDataService.h>

// #include <ExecutionContext/ExecutionContext.h>
// #include <osfclient/IOsfControlContainerExecutionContext.h>

namespace Mso::JSHost::Headless {

// static constexpr const Mso::JSHost::NamedProperty<IOsfControlContainer> ControlContainerProperty {"ControlContainer"};
static constexpr const Mso::JSHost::NamedProperty<uint64_t> DocCookieProperty{ "DocCookie" };

JSRuntime::~JSRuntime() noexcept {}
JSHeadlessRuntime::~JSHeadlessRuntime() noexcept {}

void JSHeadlessRuntime::PrimeRuntime() noexcept
{
    const std::lock_guard<std::mutex> lock(g_pendingFuncs_mutex);
    m_jsRuntimeInitialized = true;
    if(m_pendingFuncs.size() > 0)
    {
        for (auto &func : m_pendingFuncs)
        {
            PostInternal(std::move(func)); // TODO:: We can run inline as we are in JS thread here.
        }
        m_pendingFuncs.clear();
    }
}

Mso::React::ReactOptions JSHeadlessRuntime::CreateReactOptions() noexcept 
{
    Mso::React::ReactOptions reactOptions{};
    reactOptions.JSBundles = Mso::React::GetJSBundles( { Mso::React::JSBundles::SDXFoundation, Mso::React::JSBundles::SDXUI } );

    reactOptions.Identity = m_options.Identity;
    reactOptions.DataServiceProviderName = m_options.RekaProviderName;

    reactOptions.DeveloperSettings.IsDevModeEnabled = m_options.DeveloperSettings.IsDevModeEnabled;
    reactOptions.DeveloperSettings.SourceBundleName = m_options.DeveloperSettings.SourceBundleName;
    reactOptions.DeveloperSettings.UseDirectDebugger = m_options.DeveloperSettings.UseDirectDebugger;
    reactOptions.DeveloperSettings.DebuggerBreakOnNextLine = m_options.DeveloperSettings.DebuggerBreakOnNextLine;
    reactOptions.DeveloperSettings.DebuggerRuntimeName = m_options.DeveloperSettings.DebuggerRuntimeName;
    reactOptions.DeveloperSettings.UseFastRefresh = m_options.DeveloperSettings.IsDevModeEnabled;

//    if(m_options.EnableOfficeJS)
//    {
//        // For now, we assume that the caller ensures that the call is made in App thread.
//        VerifyElseCrashTag(Mso::ApplicationModel::VerifyAppThread(Mso::ApplicationModel::GetCurrentExecutionContext().Get()), 0x1e4d52c0 /* tag_4tvla */);
//        Mso::JSHost::EnsureRichApiDataServiceRegistration();
//        Mso::TCntPtr<Mso::React::IJSBundle> spHostSpecificOfficeJsBundle;
//        VerifySucceededElseCrashTag(Osf::React::CreateHostSpecificOfficeJsBundle(spHostSpecificOfficeJsBundle), 0x1e4d52a3 /* tag_4tvk9 */);
//        reactOptions.JSBundles.push_back(spHostSpecificOfficeJsBundle);
//    }

#ifdef MS_TARGET_APPLE    
    const std::string bundlePath = GetBundlePath(m_options.Identity.c_str());
    reactOptions.AddFileJSBundle(m_options.Identity.c_str(), bundlePath);
#elif MS_TARGET_ANDROID
    if(!m_options.JSBundlePath.empty())
    {
        reactOptions.AddFileJSBundle(m_options.Identity.c_str(), m_options.JSBundlePath);
    }
    else
    {
        // On Android this gets mapped to the packaged asset bundle.
        reactOptions.AddFileJSBundle(m_options.Identity.c_str(), m_options.Identity.c_str());
    }
#endif

    // Mso::JSHost::RichApiExecutionContextHolderPtr richApiExecutionContextHolder = std::make_shared< Mso::JSHost::RichApiExecutionContextHolder >(Mso::ApplicationModel::GetCurrentExecutionContext());
    reactOptions.OnInitialized = [spWeakThis = Mso::WeakPtr(this)/*, richApiExecutionContextHolder*/](Mso::JSHost::IRekaContext& rekaContext)
    {
        auto spStrongThis = spWeakThis.GetStrongPtr();
        if(!spStrongThis)
            return;

        // rekaContext.Properties().Set( Mso::JSHost::RichApiExecContextProperty, richApiExecutionContextHolder );

//        DWORD_PTR docCookie{};
//        VerifySucceededElseCrashTag(OsfHostGetCurrentDocCookie(&docCookie), 0x1e4d52a2 /* tag_4tvk8 */);
//        rekaContext.Properties().Set(DocCookieProperty, docCookie);
//
//        Mso::TCntPtr<IOsfControlContainer> spControlContainer;
//        VerifySucceededElseCrashTag(Osf::React::CreateExternalSdxControlContainer(docCookie, 0, /* out */ spControlContainer), 0x1e4d52a1 /* tag_4tvk7 */);
//        rekaContext.Properties().Set(ControlContainerProperty, spControlContainer);
//
//        Mso::TCntPtr<IOsfControlContainerExecutionContext> spControlContainerExecutionContext = qi_cast<IOsfControlContainerExecutionContext>(spControlContainer);
//        if (spControlContainerExecutionContext != nullptr)
//        {
//            spControlContainerExecutionContext->SetExecutionContext(richApiExecutionContextHolder->GetExecutionContext().Get());
//        }
//
//        Osf::React::RichApiReactConnectionOptions richApiConnectionOptions = {};
//        richApiConnectionOptions.DocCookie = docCookie;
//        richApiConnectionOptions.SubDocCookie = 0;
//        richApiConnectionOptions.Readonly = false;
//        richApiConnectionOptions.OsfControlContainer = spControlContainer;
//        Osf::React::SetupRichApiReactConnection(rekaContext, std::move(richApiConnectionOptions));

        if(spStrongThis->Options().OnRekaInitialized){
            spStrongThis->Options().OnRekaInitialized(*spStrongThis, rekaContext);
        }
    };

    reactOptions.OnDestroyed = [spWeakThis = Mso::WeakPtr(this)](Mso::JSHost::IRekaContext& rekaContext)
    {
//        Mso::TCntPtr<IOsfControlContainer> spControlContainer = rekaContext.Properties().Get(ControlContainerProperty);
//        if (spControlContainer != nullptr)
//        {
//            Osf::React::DeleteExternalSdxControlContainer(spControlContainer.Get());
//        }

        auto spStrongThis = spWeakThis.GetStrongPtr();
        if(!spStrongThis)
            return;

        spStrongThis->MarkAsDestroyed();
        if(spStrongThis->Options().OnRekaDestroyed) {
            spStrongThis->Options().OnRekaDestroyed(*spStrongThis, rekaContext);
        }
    };
 
    reactOptions.OnInstanceLoaded = [spWeakThis = Mso::WeakPtr(this)](Mso::React::IReactInstance&, const Mso::ErrorCode& /*errorCode*/)
    {
        auto spStrongThis = spWeakThis.GetStrongPtr();
        if(!spStrongThis)
            return;

        if(spStrongThis->Options().OnLoaded){
            spStrongThis->PostInternal([spStrongThis](facebook::jsi::Runtime &runtime)
            {
                spStrongThis->Options().OnLoaded(*spStrongThis, runtime);
            });
        }

        // Ready to execute user posted tasks.
        spStrongThis->PrimeRuntime();
    };

    reactOptions.OnInstanceCreated = [spWeakThis = Mso::WeakPtr(this)](Mso::React::IReactInstance&)
    {
        auto spStrongThis = spWeakThis.GetStrongPtr();
        if(!spStrongThis)
            return;

        if(spStrongThis->Options().OnCreated){
            spStrongThis->Options().OnCreated(*spStrongThis);
        }
    };

    reactOptions.OnInstanceDestroyed = [spWeakThis = Mso::WeakPtr(this)](Mso::React::IReactInstance&)
    {
        auto spStrongThis = spWeakThis.GetStrongPtr();
        if(!spStrongThis)
            return;

        if(spStrongThis->Options().OnDestroyed){
            spStrongThis->Options().OnDestroyed(*spStrongThis);
        }
    };

    return reactOptions;
}

void JSHeadlessRuntime::PostInternal(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) noexcept {
    AssertTag(!m_jsRuntimeDestroyed, 0x1e45208f /* tag_4rscp */);
    auto runtimeExecutor = Mso::React::GetRuntimeExecutor2(*(m_spReactHost->Instance()));
    VerifyElseCrashTag(runtimeExecutor, 0x1e446096 /* tag_4rgcw */);
    runtimeExecutor(std::move(callback));
}

JSHeadlessRuntime::JSHeadlessRuntime(JSRuntimeOptions&& options) noexcept
    : m_options(std::move(options)),
        m_spReactHost(Mso::React::MakeReactHost(CreateReactOptions())) {}

void JSHeadlessRuntime::Post(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) noexcept
{
    const std::lock_guard<std::mutex> lock(g_pendingFuncs_mutex);
    if(m_jsRuntimeInitialized) 
    {
        AssertTag(m_pendingFuncs.empty(), 0x1e45208e /* tag_4rsco */);
        PostInternal(std::move(callback));
    } 
    else 
    {
        m_pendingFuncs.push_back(std::move(callback));
    }
}

Mso::TCntPtr<JSRuntime> CreateJSRuntime(JSRuntimeOptions&& options) noexcept
{
    return Mso::Make<JSHeadlessRuntime>(std::move(options));
}

} // namespace Mso::JSHost
