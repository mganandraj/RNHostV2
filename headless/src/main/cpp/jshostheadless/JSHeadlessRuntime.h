#pragma once

#include <jsi/jsi.h>
#include <functional/functor.h>
#include <errorCode/errorCode.h>
#include <object/refCounted.h>
#include <reka/RekaApi.h>

namespace Mso::JSHost::Headless
{
    struct JSRuntime : public Mso::IRefCounted
    {
        virtual void Post(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) = 0;
        virtual ~JSRuntime() = 0;
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
    
    using OnCreatedCallback = Mso::Functor<void(JSRuntime&)>;
    using OnLoadedCallback = Mso::Functor<void(JSRuntime&, const Mso::ErrorCode&)>;
    using OnDestroyedCallback = Mso::Functor<void(JSRuntime&)>;

    using OnRekaInitializedCallback = Mso::Functor<void(JSRuntime&, IRekaContext&)>;
    using OnRekaDestroyedCallback = Mso::Functor<void(JSRuntime&, IRekaContext&)>;

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
        //! If more JS bundles are to be loaded after Reka initialization, then this callback could be called
        //! before OnReactInstanceLoaded.
        //! It is called from the native queue.
        OnRekaInitializedCallback OnRekaInitialized;

        //! Reka Instance Destruction
        //! The callback is called when Reka instance is destroyed.
        //! It is the last chance to use IRekaContext.
        //! This callback is called only if OnInitialized callback called before.
        //! It is called before the OnReactInstanceDestroyed callback.
        //! It is called from the native queue.
        OnRekaDestroyedCallback OnRekaDestroyed;
            
        //! The callback is called when IReactInstance is created, and JS bundles are started to load.
        //! It is called from the native queue.
        OnCreatedCallback OnCreated;

        //! The callback is called when Runtime finished loading JS bundles.
        //! If there were errors, then the provided Mso::ErrorCode is not empty.
        //! The callback is always called regardless if JSBundles were loaded or not.
        //! This callback is called from the native queue.
        //!
        //! Error types include:
        //! * Javascript Syntax Error
        //! * Global Javascript Exception Thrown
        //!    Including Javascript Exceptions during initialization
        OnLoadedCallback OnLoaded{nullptr};

        //! The callback is called when Runtime is destroyed and must not be used anymore.
        //! It is called from the native queue.
        OnDestroyedCallback OnDestroyed;

    };

    Mso::CntPtr<JSRuntime> CreateJSRuntime(JSRuntimeOptions&&);

} // namespace Mso::JSHost::Headless