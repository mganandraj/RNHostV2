// #include "precomp.h"

#include "JSHeadlessRuntimeHelpers.h"
#include "JSHeadlessRuntimeImpl.h"

#include <jshostassets/assetconfiguration.h>
#include <JSHostOfficePal/PlatformJSBundles.h>
#include <ReactNativeHost/JSBundle.h>
#include <Reka/RekaServiceRegistration.h>
#include <ReactNativeHost/React.h>

#include <osfclient/osfreacthelper.h>
#include <osfclient/hostintegrationmanager.h>
#include <jshostrichapids/IRichApiProxyDataService.h>

#include <ExecutionContext/ExecutionContext.h>
#include <synchronization/simpleEvent.h>

namespace Mso::JSHost::Headless {

static constexpr const Mso::JSHost::NamedProperty<IOsfControlContainer> ControlContainerProperty {"ControlContainer"};
static constexpr const Mso::JSHost::NamedProperty<uint64_t> DocCookieProperty{ "DocCookie" };

JSRuntime::~JSRuntime() {}

void JSHeadlessRuntime::OnJSLoaded(facebook::jsi::Runtime &runtime)
{
    AssertTag(m_runtime == nullptr, 0x1e4d52c2 /* tag_4tvlc */);
    m_runtime = &runtime;

    m_initialized = true;
    if(m_pendingFuncs.size() > 0){
        for (auto &func : m_pendingFuncs) {
            Post(std::move(func)); // TODO:: We can run inline as we are in JS thread here.
        }
        m_pendingFuncs.clear();
    }
}

void JSHeadlessRuntime::CreateReactInstance()
{
#ifdef USE_OLD_RNHOST
    EnsureSDXRegisteration();
    m_spReactInstance = Mso::React::GetOrCreateReactInstance( m_options.Identity.c_str(), nullptr ).Detach();
    VerifyElseCrashTag(m_spReactInstance != nullptr, 0x1e4d52c1 /* tag_4tvlb */);
#else
    m_spReactInstance = Mso::React::MakeReactHost(CreateReactOptions());
#endif
}

Mso::React::ReactOptions JSHeadlessRuntime::CreateReactOptions() {
    Mso::React::ReactOptions reactOptions{};
    reactOptions.JSBundles = Mso::React::GetJSBundles( { Mso::React::JSBundles::SDXFoundation, Mso::React::JSBundles::SDXUI } );

    reactOptions.Identity = m_options.Identity;
    reactOptions.DataServiceProviderName = m_options.RekaProviderName;

#if DEBUG
    reactOptions.DeveloperSettings.SourceBundleName = "index";
    reactOptions.DeveloperSettings.IsDevModeEnabled = true;
    reactOptions.DeveloperSettings.UseDirectDebugger = true;
    reactOptions.DeveloperSettings.UseFastRefresh = true;
#endif

    if(m_options.EnableOfficeJS) 
    {
        // For now, we assume that the caller ensures that the call is made in App thread.
        VerifyElseCrashTag(Mso::ApplicationModel::VerifyAppThread(Mso::ApplicationModel::GetCurrentExecutionContext().Get()), 0x1e4d52c0 /* tag_4tvla */);
        Mso::JSHost::EnsureRichApiDataServiceRegistration();
        Mso::TCntPtr<Mso::React::IJSBundle> spHostSpecificOfficeJsBundle;
        VerifySucceededElseCrashTag(Osf::React::CreateHostSpecificOfficeJsBundle(spHostSpecificOfficeJsBundle), 0x1e4d52a3 /* tag_4tvk9 */);
        reactOptions.JSBundles.push_back(spHostSpecificOfficeJsBundle);
    }

#ifndef USE_OLD_RNHOST
    // MakeReactHost API requires the feature bundle to be explicitly added to the bundle list.
    // Note that we are adding the disk file path overlooking the Metro bundler running on the developer machine.   
    const std::string bundlePath = GetBundlePath(m_options.Identity.c_str());
    reactOptions.AddFileJSBundle(m_options.Identity.c_str(), bundlePath);
#endif

    Mso::JSHost::RichApiExecutionContextHolderPtr richApiExecutionContextHolder = std::make_shared< Mso::JSHost::RichApiExecutionContextHolder >(Mso::ApplicationModel::GetCurrentExecutionContext());
    reactOptions.OnInitialized = [spThis = Mso::TCntPtr(this), richApiExecutionContextHolder](Mso::JSHost::IRekaContext& rekaContext)
    {
        rekaContext.Properties().Set( Mso::JSHost::RichApiExecContextProperty, richApiExecutionContextHolder );
        
        DWORD_PTR docCookie{};
        VerifySucceededElseCrashTag(OsfHostGetCurrentDocCookie(&docCookie), 0x1e4d52a2 /* tag_4tvk8 */);
        rekaContext.Properties().Set(DocCookieProperty, docCookie);
    
        Mso::TCntPtr<IOsfControlContainer> spControlContainer;
        VerifySucceededElseCrashTag(Osf::React::CreateExternalSdxControlContainer(docCookie, 0, /* out */ spControlContainer), 0x1e4d52a1 /* tag_4tvk7 */);
        rekaContext.Properties().Set(ControlContainerProperty, spControlContainer);
    
        Osf::React::RichApiReactConnectionOptions richApiConnectionOptions = {};
        richApiConnectionOptions.DocCookie = docCookie;
        richApiConnectionOptions.SubDocCookie = 0;
        richApiConnectionOptions.Readonly = false;
        richApiConnectionOptions.OsfControlContainer = spControlContainer;
        Osf::React::SetupRichApiReactConnection(rekaContext, std::move(richApiConnectionOptions));

        if(spThis->Options().OnRekaInitialized){
            spThis->Options().OnRekaInitialized(*spThis, rekaContext);
        }
    };

    reactOptions.OnDestroyed = [spThis = Mso::TCntPtr(this)](Mso::JSHost::IRekaContext& rekaContext)
    {
        Mso::TCntPtr<IOsfControlContainer> spControlContainer = rekaContext.Properties().Get(ControlContainerProperty);
        if (spControlContainer != nullptr)
        {
            Osf::React::DeleteExternalSdxControlContainer(spControlContainer.Get());
        }

        if(spThis->Options().OnRekaDestroyed) {
            spThis->Options().OnRekaDestroyed(*spThis, rekaContext);
        }
    };
 
    // The following callbacks are not called with legacy RNHost.
    reactOptions.OnInstanceLoaded = [spThis = Mso::TCntPtr(this)](Mso::React::IReactInstance&, const Mso::ErrorCode& errorCode)
    {
        auto executor = spThis->GetRuntimeExecutor();
        executor([spThis](facebook::jsi::Runtime &runtime)
            {
                spThis->OnJSLoaded(runtime);
            });

        if(spThis->Options().OnLoaded){
            spThis->Options().OnLoaded(*spThis, errorCode);
        }
    };

    reactOptions.OnInstanceCreated = [spThis = Mso::TCntPtr(this)](Mso::React::IReactInstance&)
    {
        if(spThis->Options().OnCreated){
            spThis->Options().OnCreated(*spThis);
        }
    };

    reactOptions.OnInstanceDestroyed = [spThis = Mso::TCntPtr(this)](Mso::React::IReactInstance&)
    {
        if(spThis->Options().OnDestroyed){
            spThis->Options().OnDestroyed(*spThis);
        }
    };

    return reactOptions;
}

void JSHeadlessRuntime::EnsureSDXRegisteration()
{
    static std::once_flag once;
    std::call_once(once, [this](){
        Mso::Synchronization::SimpleEvent event;
    
        // OSF initialization requires app thread.
        Mso::Async::InvokeElsePost( Mso::ApplicationModel::GetCurrentExecutionContext()->UseAppContext(), [&event, this]() noexcept {
            Mso::React::ReactOptions descriptor(CreateReactOptions());
            Mso::React::RegisterSDX( std::move( descriptor ) );
            event.Set();
        });
        event.Wait();
    });
}

void JSHeadlessRuntime::Initialize(Mso::TCntPtr<Mso::ApplicationModel::IExecutionContext>)
{
    CreateReactInstance();
#ifdef USE_OLD_RNHOST
    auto executor = GetRuntimeExecutor();
    executor([&](facebook::jsi::Runtime &runtime)
        {
            m_spJsiNativeImpl = std::make_shared<JsiNativeImpl>(runtime, *this);
            m_spJsiNativeImpl->Register();
        });
#endif
}

facebook::react::RuntimeExecutor JSHeadlessRuntime::GetRuntimeExecutor() 
{
#ifdef USE_OLD_RNHOST
    return Mso::React::GetRuntimeExecutor(*m_spReactInstance);
#else
    return Mso::React::GetRuntimeExecutor2(*(m_spReactInstance->Instance()));
#endif
}

facebook::jsi::Runtime& JSHeadlessRuntime::GetJsiRuntime() const noexcept
{
    VerifyElseCrashSzTag(m_runtime, "Trying to access the JSI Runtime before the JSHeadlessRuntime finished loading", 0x1e4d52a0 /* tag_4tvk6 */);
    return *m_runtime;
}

JSHeadlessRuntime::JSHeadlessRuntime(JSRuntimeOptions&& options) 
    : m_options(std::move(options))
{  
    Initialize(); 
}

void JSHeadlessRuntime::Post(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) 
{
    if(m_initialized) {
        GetRuntimeExecutor()(std::move(callback));
    }
    else {
        m_pendingFuncs.push_back(std::move(callback));
    }
}

Mso::TCntPtr<JSRuntime> CreateJSRuntime(JSRuntimeOptions&& options)
{
    return Mso::Make<JSHeadlessRuntime>(std::move(options));
}

} // namespace Mso::JSHost
