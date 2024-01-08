#pragma once

#include <jsi/jsi.h>
#ifdef USE_OPENSOUCE_MSO
#include <functional/functor.h>
#else
#include <core/functor.h>
#endif
#include <ErrorCode/ErrorCode.h>
#include <object/RefCounted.h>
#include <Reka/RekaApi.h>

namespace Mso::JSHost::Headless
{
struct JSRuntime : public Mso::IRefCounted
{
    virtual void Post(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) noexcept = 0;
    virtual ~JSRuntime() noexcept = 0;
};

enum class LogLevel : int32_t
{
    Trace = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4,
};

using OnErrorCallback = Mso::Functor<void(const Mso::ErrorCode&)>;
using OnLoggingCallback = Mso::Functor<void(LogLevel logLevel, const char* message)>;

// using OnCreatedCallback = Mso::Functor<void(JSRuntime&, facebook::jsi::Runtime&)>;
using OnCreatedCallback = Mso::Functor<void(JSRuntime&)>;
using OnLoadedCallback = Mso::Functor<void(JSRuntime&, facebook::jsi::Runtime&)>;
using OnDestroyedCallback = Mso::Functor<void(JSRuntime&)>;

using OnRekaInitializedCallback = Mso::Functor<void(JSRuntime&, IRekaContext&)>;
using OnRekaDestroyedCallback = Mso::Functor<void(JSRuntime&, IRekaContext&)>;

struct JSRuntimeDevOptions
{
	//! When dev mode is enabled, extra developer features are available
	//! such as the live reload etc.
	bool IsDevModeEnabled{false};

	//! Enables debugging in the JavaScript engine (if supported).
	bool UseDirectDebugger{false};

	//! For direct debugging, whether to break on the next line of JavaScript that is executed.
	bool DebuggerBreakOnNextLine{false};

  	//! For direct debugging, specifies a name to associate with the JavaScript runtime instance.
	std::string DebuggerRuntimeName;

    //! Bundle name without any extension (e.g. "index.android"). Default: "index.{PLATFORM}"
    std::string SourceBundleName;
};

struct JSRuntimeOptions
{
    //! Identity which uniquely identifies the instance or JS bundle. This value is used by other components to uniquely identify JS bundles and modules to be loaded.
    std::string Identity;

    //! (Optional) Name of the registered IDataServiceProvider to use when creating data services
    //! for this SDX.  Providers are registered using RegisterDataServiceProvider().
    //! Or, specify the reserved name DefaultDataServiceProviderName to delegate to
    //! the DataServiceRegistration facility.
    std::string RekaProviderName;

    // Whether OfficeJS support is needed.
    bool EnableOfficeJS{true};

    //! The callback is called when Reka instance is initialized and provided IRekaContext can be used.
    //! It may be never called if Reka instance is never initialized by JavaScript code.
    //! There is no guarantee on the relative ordering w.r.t. other callbacks except OnRekaDestroyed
    OnRekaInitializedCallback OnRekaInitialized;

    //! Reka Instance Destruction
    //! The callback is called when Reka instance is destroyed.
    //! It is the last chance to use IRekaContext.
    //! This callback is called only if OnRekaInitialized callback called before.
    //! There is no guarantee on the relative ordering w.r.t. other callbacks except OnRekaDestroyed
    OnRekaDestroyedCallback OnRekaDestroyed;
        
    //! The callback is called when runtime is created, and JS bundles are started to load.
    //! It MAY NOT be called from the JS executor thread. 
    OnCreatedCallback OnCreated;

    //! The callback is called when Runtime finished loading JS bundles.
    //! The callback is always called regardless if JSBundles were loaded or not.
    //! It is called from the JS executor thread. Callback can access JS runtime synchronously.
    OnLoadedCallback OnLoaded{nullptr};

    //! The callback is called when Runtime is destroyed and must not be used anymore.
    //! It MAY NOT be called from the JS executor thread. 
    OnDestroyedCallback OnDestroyed;

    //! Expected ordering of the callbacks:
    //! OnCreated -> OnRekaInitialized -> OnLoaded -> <User posted tasks> -> OnRekaDestroyed -> OnDestroyed

    JSRuntimeDevOptions DeveloperSettings = {};

    //! JS bundle based on a file name to JSBundles.
    //! If set, this will override the default bundle path discovery.
    std::string JSBundlePath;
};

LIBLET_PUBLICAPI Mso::TCntPtr<JSRuntime> CreateJSRuntime(JSRuntimeOptions&&) noexcept;

} // namespace Mso::JSHost::Headless
