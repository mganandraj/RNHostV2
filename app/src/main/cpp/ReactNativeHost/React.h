#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <DispatchQueue/DispatchQueue.h>
#include <ErrorCode/ErrorCode.h>
#include <future/future.h>
#include <object/RefCounted.h>
// #include <ReactNativeHost/IMemoryTracker.h>
#include "ReactNativeHost/JSBundle.h"
//#include <ReactNativeHost/JSExceptionInfo.h>
//#include <ReactNativeHost/ModuleProvider.h>
//#include <Reka/RekaApi.h>
//#include <ShipAssertApi/ShipAssertApi.h>

#pragma pack(push, _CRT_PACKING)

namespace facebook {

namespace jsi {
class Runtime;
}

namespace react {
using RuntimeExecutor = std::function<void(std::function<void(jsi::Runtime &runtime)> &&callback)>;
}

}

namespace Mso::React {

// Forward declarations
struct IReactInstance;
struct IReactRootView;
struct IReactViewHost;
struct ReactOptions;

enum class LogLevel : int32_t
{
	Trace = 0,
	Info = 1,
	Warning = 2,
	Error = 3,
	Fatal = 4,
};

// These values must stay in sync with facebook::react::JSIEngineOverride
enum class JSIEngineOverride : int32_t
{
	Unset = -1,		// No change, use individual instance override
	Default = 0,	// No JSI, will use the legacy ExecutorFactory
	Chakra = 1,		// Use the JSIExecutorFactory with ChakraRuntime
	ChakraCore = 2, // Use the JSIExecutorFactory with ChakraCoreRuntime
	Hermes = 3,		// Use the JSIExecutorFactory with Hermes
	V8 = 4,			// Use the JSIExecutorFactory with V8
	V8Lite = 5,		// Use the JSIExecutorFactory with V8Lite,
	V8NodeApi = 6,	// Use the JSIExecutorFactory with V8 via ABI-safe NAPI

	Last = V8NodeApi
};

enum class HttpModuleClass : bool
{
	Office = false,				//Mso::React::NetworkingHttpModule
	ReactNativeWindows = true	//Microsoft::React::HttpModule
};


using OnErrorCallback = Mso::Functor<void(const Mso::ErrorCode&)>;
using OnLoggingCallback = Mso::Functor<void(LogLevel logLevel, const char* message)>;
using OnReactInstanceCreatedCallback = Mso::Functor<void(IReactInstance&)>;
using OnReactInstanceLoadedCallback = Mso::Functor<void(IReactInstance&, const Mso::ErrorCode&)>;
using OnReactInstanceDestroyedCallback = Mso::Functor<void(IReactInstance&)>;

////! PropertyBag named property for JavaScript dispatch queue.
//inline constexpr const Mso::JSHost::NamedProperty<Mso::Async::IDispatchQueue> JSDispatchQueueProperty{"JSDispatchQueue"};
//
////! PropertyBag named property for Native dispatch queue.
//inline constexpr const Mso::JSHost::NamedProperty<Mso::Async::IDispatchQueue> NativeDispatchQueueProperty{"NativeDispatchQueue"};
//
////! PropertyBag named property for UI dispatch queue.
//inline constexpr const Mso::JSHost::NamedProperty<Mso::Async::IDispatchQueue> UIDispatchQueueProperty{"UIDispatchQueue"};

//! Every individual breaking behavior change on the platform should have its own quirk.  This way if
//! there is a breaking change that is a significant amount of work for an SDX to fix, they can maintain that
//! behavior without having to stop getting other new behaviors.
//! We can also add additional quirks based on what quirk version the SDX is running at.  (Maybe we need to do
//! something different for an SDX three versions behind, vs one version behind)
//! Quirks can be shared by the SDX platform, or host specific.
//! Quirks should be registered by calling RegisterQuirk.  Then they can be set on ReactOptions.
//! Generally a host specific version config should be used to set the quirks on ReactOptions, rather
//! then setting them manually.
using SDXQuirk = uint32_t;

//! An Office wrapper that extends FB's React Instance and makes it a 1:1 relationship with the bundle,
//! such that each Office React Instance is capable of loading a single bundle and hosting js in an
//! isolated JavaScript execution environment.
MSO_STRUCT_GUID(IReactInstance, "085D524A-AF3B-4839-8056-E5D0E6FC64BC")
struct IReactInstance : public IUnknown
{
	//! Returns the name of the bundle file this React Instance has loaded.
	virtual const std::string& JsBundleName() const noexcept = 0;

	//! Returns the SDX base path from where this React Instance has loaded.
	virtual std::string SDXBasePath() const noexcept = 0;

	//! Returns a flag that indicates some properties of the existing instance has been changed (like using web debugger),
	//! and this instance should not be cached and served anymore.
	virtual bool NeedsReload() const noexcept = 0;

	//! Marks this instance as needing reload, due to a fundamental property change (like useWebDebugger).
	//! After the instance is marked, this instance will not be served through GetOrCreateReactInstance API anymore.
	virtual void SetAsNeedsReload() noexcept = 0;

	//! Returns if a quirk is set on this instance or not.
	virtual bool GetQuirk(SDXQuirk quirk) const noexcept = 0;

	//! Returns ReactOptions associated with the IReactInstance
	//! The ReactOptions are meant to immutable and give to IReactInstance at its creation.
	virtual const ReactOptions& Options() const noexcept = 0;
};

//! An isolated UI control in the UI tree that is not owned (parented) by another React View.
MSO_STRUCT_GUID(IReactRootView, "FF0FF21D-C760-48C2-98B9-E62DDAFE2BBF")
struct IReactRootView : public IUnknown
{
	//! Returns the ReactInstance this rootView is part of.
	virtual Mso::CntPtr<IReactInstance> GetReactInstance() const noexcept = 0;

	//! Returns the name of the component. This name refers to the name of the component in the bundle
	//! that is loaded by the React Instance.
	virtual const std::string& JsComponentName() const noexcept = 0;

	//! Start the JS component and starts rendering it.
	virtual void Start() noexcept = 0;

	//! Stop the JS component.
	virtual void Stop() noexcept = 0;
};

//! Settings per each IReactViewHost associated with an IReactHost instance.
struct ReactViewOptions
{
	//! Name of a component registered in JavaScript via AppRegistry.registerComponent('ModuleName', () => ModuleName);
	std::string ComponentName;

	//! Set of initial component properties. It is a JSON string.
	std::string InitialProps;
};

struct ReactDevOptions
{
	//! When dev mode is enabled, extra developer features are available
	//! such as the dev menu, red box, etc.
	bool IsDevModeEnabled{false};

	//! Enables debugging in the JavaScript engine (if supported).
	bool UseDirectDebugger{false};

	//! For direct debugging, whether to break on the next line of JavaScript that is executed.
	bool DebuggerBreakOnNextLine{false};

	//! For direct debugging, allow specifying the port to use for the debugger.
	uint16_t DebuggerPort{0};

	//! For direct debugging, specifies a name to associate with the JavaScript runtime instance.
	std::string DebuggerRuntimeName;

	//! Enable live reload to load the source bundle from the React Native packager.
	//! When the file is saved, the packager will trigger reloading.
	bool UseLiveReload{false};

	//! Fast refresh is a hot module reload technology introduced in react-native 0.61.
	//! This should be considered a replacement for UseLiveReload
	bool UseFastRefresh{false};

	//! URL used for debugging
	std::string DebugHost;

	//! When using Fast Refresh, the source is obtained from the packager.
	//! The source url for the bundle is "http://{HOST}:{PORT}/{NAME}{EXTENSION}?platform=..."
	//! which defaults to:
	//! {HOST} = "localhost",
	//! {PORT} = "8081",
	//! {NAME} = "index.{PLATFORM}" where {PLATFORM} = "ios", "android", "win32", etc.
	//! {EXTENSION} = ".bundle"
	//! Specify a value for a component, or leave empty to use the default.
	std::string SourceBundleHost;	   // Host domain (without port) for the bundler server. Default: "localhost".
	std::string SourceBundlePort;	   // Host port for the bundler server. Default: "8081".
	std::string SourceBundleName;	   // Bundle name without any extension (e.g. "index.win32"). Default: "index.{PLATFORM}"
	std::string SourceBundleExtension; // Bundle name extension. Default: ".bundle".

	//! Module name used for loading the debug bundle.
	//! e.g. The modules name registered in the jsbundle via AppRegistry.registerComponent('ModuleName', () => ModuleName);
	std::string SourceBundleModuleName;

	//! Allows overriding the JSI engine at the instance level
	JSIEngineOverride JSIEngineOverride{JSIEngineOverride::Default};

	/// Workaround OSF sandbox not being able to read feature gates from the registry.
	HttpModuleClass HttpModuleClass{HttpModuleClass::Office};
};

//! A simple struct that describes the basic properties/needs of an SDX. Whenever a new SDX is
//! getting hosted in React, properties here will be used to construct the SDX.
struct ReactOptions
{
	//! Identity of the SDX. Must uniquely describe the SDX across the installed product.
	std::string Identity;

	//! (Deprecated) List of registered module providers whose native modules
	//! should be loaded by the instance. These modules must have been
	//! registered in the same DLL as the ReactInstance. It is preferred that
	//! new code use a Reka service or pass CxxModuleProviders directly if
	//! native modules must be used.
	//!
	//! In addition to this list, there are following hardcoded modules.
	//!
	//! Cxx Modules Provided by ReactNativeHost:
	//! RekaBridge: Implemented by Mso::React::RekaNativeModule
	//!
	//! Cxx Modules Provided by ReactNative:
	//! WebSocketModule: Implemented by facebook::react::WebSocketModule
	//! Timing: Implemented by facebook::react::TimingModule
	//! SourceCode: Implemented by facebook::react::SourceCodeModule
	//! ExceptionsManager: Implemented by facebook::react::ExceptionsManagerModule
	std::vector<std::string> CxxModuleNames;

	//! Module providers whose native modules should be loaded by the instance.
	// std::vector<Mso::TCntPtr<Mso::JSHost::ICxxModuleProvider>> CxxModuleProviders;

	//! List of names of Java ReactPackages whose native modules will be loaded
	//! by the instance.
	std::vector<std::string> JavaModuleNames;

	//! List of names of Apple Module Providers whose native modules will be loaded
	std::vector<std::string> AppleModuleProviders{"AppleNativeModuleProvider"};

	//! (Optional) Name of the registered IDataServiceProvider to use when creating data services
	//! for this SDX.  Providers are registered using RegisterDataServiceProvider().
	//! Or, specify the reserved name DefaultDataServiceProviderName to delegate to
	//! the DataServiceRegistration facility.
	std::string DataServiceProviderName;

	//! Base path of the SDX. The absolute path of the SDX can be constructed from this and the Identity.
	std::string SDXBasePath;

	//! Javascript Bundles
	//! This List includes both Platform and User Javascript Bundles
	//! Bundles are loaded into Javascript engine in the same order
	//! as they are specified in this list.
	std::vector<Mso::CntPtr<IJSBundle>> JSBundles;

	//! The callback is called when Reka instance is initialized and provided IRekaContext can be used.
	//! It may be never called if Reka instance is never initialized by JavaScript code.
	//! If more JS bundles are to be loaded after Reka initialization, then this callback could be called
	//! before OnReactInstanceLoaded.
	//! It is called from the native queue.
	// Mso::JSHost::OnInitializedCallback OnInitialized;

	//! Reka Instance Destruction
	//! The callback is called when Reka instance is destroyed.
	//! It is the last chance to use IRekaContext.
	//! This callback is called only if OnInitialized callback called before.
	//! It is called before the OnReactInstanceDestroyed callback.
	//! It is called from the native queue.
	// Mso::JSHost::OnDestroyedCallback OnDestroyed;

	//! ReactNative Infrastructure Error
	//! Error types include:
	//! * Any call to Javascript function after Global Exception has been raised
	//! * Any WebServer error when DeveloperSettings.UseWebDebugger is true
	//! Note: Default callback generates ShipAssert.
	OnErrorCallback OnError{nullptr};

	//! The callback is called when JavaScript code logs a message through the console.log class of APIs.
	//! Note: this is currently only used in Win32 (Chakra Executor)
	OnLoggingCallback OnLogging;

	//! Javascript Function Exception
	//! Callback called when an Exception is thrown inside JavaScript function,
	//! except the ones that happen during initialization.
	//! Javascript Exceptions which happen during initialization go through OnError callback.
	// OnJSExceptionCallback OnJSException{nullptr};

	//! Flag to suggest sdx owner's preference on enabling Bytecode caching in Javascript Engine for corresponding SDX.
	bool EnableBytecode{true};

	//! Flag controlling whether the JavaScript engine uses JIT compilation.
	bool EnableJITCompilation{true};

	//! Enable function nativePerformanceNow.
	//! Method nativePerformanceNow() returns high resolution time info.
	//! It is not safe to expose to Custom Function. Add this flag so we can turn it off for Custom Function.
	bool EnableNativePerformanceNow{true};

	//! Enables telemetry that may collect sensitive information if used for third-party scenarios (e.g. bundle
	//! filenames). This must be manually enabled on SDX's which certify not to load third-party bundles.
	bool EnableSensitiveTelemetry{false};

	//! This lets us have a mechanism to keep existing SDX behavior in places where we want to change the behavior
	std::unordered_set<SDXQuirk> Quirks;

	ReactDevOptions DeveloperSettings = {};

	//! Object providing notifications about memory consumption of the JS engine.
	// Mso::TCntPtr<IMemoryTracker> MemoryTracker;

	//! Custom Provider Factory. Optional.
	// Mso::Functor<Mso::TCntPtr<Mso::JSHost::IRekaServiceProvider>(Mso::JSHost::IRekaContext&)> RekaProviderFactory;

	//! Additional properties associated with the ReactOptions.
	// Mso::JSHost::PropertyBag Properties;

	//! Adds registered JS bundle to JSBundles.
	LIBLET_PUBLICAPI ReactOptions& AddRegisteredJSBundle(std::string_view jsBundleId) noexcept;

	//! Adds JS bundle based on a file name to JSBundles.
	LIBLET_PUBLICAPI ReactOptions& AddFileJSBundle(std::string_view jsBundleId, std::string_view fileName) noexcept;

	//! Adds JS bundle based on a string to JSBundles.
	LIBLET_PUBLICAPI ReactOptions& AddDynamicJSBundle(std::string&& jsBundleId, std::string&& jsBundleContent) noexcept;

	//! Adds JS bundle downloaded from a bundle server (Dev scenarios only - when no offline file exists).
	LIBLET_PUBLICAPI ReactOptions& AddBundleServerJSBundle(std::string&& jsBundleId) noexcept;

	//! The callback is called when IReactInstance is created, and JS bundles are started to load.
	//! It is called from the native queue.
	OnReactInstanceCreatedCallback OnInstanceCreated;

	//! The callback is called when IReactInstance finished loading JS bundles.
	//! If there were errors, then the provided Mso::ErrorCode is not empty.
	//! The callback is always called regardless if JSBundles were loaded or not.
	//! This callback is called from the native queue.
	//!
	//! Error types include:
	//! * Javascript Syntax Error
	//! * Global Javascript Exception Thrown
	//!    Including Javascript Exceptions during initialization
	OnReactInstanceLoadedCallback OnInstanceLoaded{nullptr};

	//! The callback is called when IReactInstance is destroyed and must not be used anymore.
	//! It is called from the native queue.
	OnReactInstanceDestroyedCallback OnInstanceDestroyed;

	//! This opts SDXs into a new implementation of ReactHost that uses the ABI safe interface from react-native-windows
	//! This enables usage of TurboModules for this SDX.  New SDXs are encouraged to use this flag.
	//! This flag has no effect on non-windows platforms
	bool UseWindowsAbi{false};
};

//! Gets a SDXQuirk from a quirk name.
LIBLET_PUBLICAPI SDXQuirk RegisterQuirk(const char* name) noexcept;

//! Dynamically register an SDX.
LIBLET_PUBLICAPI void RegisterSDX(ReactOptions&& options) noexcept;

//! Unregister a previously registered SDX.
LIBLET_PUBLICAPI void UnregisterSDX(const std::string& identity) noexcept;

//! Return previously registered SDX.
LIBLET_PUBLICAPI ReactOptions GetRegisteredSDXDescriptor(const std::string& identity) noexcept;

//! IReactHost manages a RactNative instance.
MSO_STRUCT_GUID(IReactHost, "2a2474ff-264f-449a-9852-0463e6ac6bbf")
struct DECLSPEC_NOVTABLE IReactHost : IUnknown
{
	//! Returns a copy of react options.
	virtual ReactOptions Options() const noexcept = 0;

	//! Returns current ReactInstance. The result can be null if instance is unloaded.
	virtual Mso::CntPtr<IReactInstance> Instance() const noexcept = 0;

	//! Gets Reka context proxy to queue up and run Reka proxies that call JavaScript services or JavaScript event handlers.
	// virtual Mso::JSHost::IRekaContext& RekaContext() noexcept = 0;

	//! A native sequential queue associated with IReactHost.
	//! All internal operations are done using this queue to synchronize internal state.
	// virtual Mso::IDispatchQueue& NativeQueue() const noexcept = 0;
	virtual Mso::DispatchQueue NativeQueue() const noexcept = 0;

	//! Reloads the ReactNative instance and associated ReactViews.
	virtual Mso::Future<void> ReloadInstance() noexcept = 0;

	//! Updates ReactOptions associated with the IReactHost and reloads the ReactNative instance.
	virtual Mso::Future<void> ReloadInstanceWithOptions(ReactOptions&& options) noexcept = 0;

	//! Unloads the ReactNative instance and associated ReactViews.
	virtual Mso::Future<void> UnloadInstance() noexcept = 0;

	//! Creates a new instance of IReactViewHost.
	//! The IReactViewHost is added to the list of view hosts only after a IReactViewInstance is attached to it.
	virtual Mso::CntPtr<IReactViewHost> MakeViewHost(ReactViewOptions&& options) noexcept = 0;

	//! Returns a snapshot of IReactViewHost instances associated with the IReactHost.
	virtual Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept = 0;
};

//! Each platform has a ReactViewInstance where it hosts the React UI tree.
//! This ReactViewInstance implements the IReactViewInstance interface.
//! When the ReactViewInstance is created, it is given a IReactViewHost instance.
//! The IReactViewInstance should attach itself to the IReactViewHost instance at creation time, and then
//! detach from it when destroyed.
//! After the IReactViewInstance is attached to the IReactViewHost, the IReactViewHost is attached to its
//! IReactHost. The IReactViewHost is detached from IReactHost after when the IReactViewInstance is detached.
//! The IReactViewHost has a set of Options associated with it that are used by IReactViewInstance to create
//! the React UI tree.
//! The IReactViewHost is responsible for calling IReactViewInstance Reload() and Unload() methods to
//! reload and unload the UI tree.
//! The IReactViewHost also provides an access to IReactHost associated with it.

//! This interface is to be implemented by a platform specific ReactViewInstance that hosts React UI tree.
MSO_STRUCT_GUID(IReactViewInstance, "29e04f14-9fc9-4dd7-a543-e59db0d57bd2")
struct IReactViewInstance : IUnknown
{
	//! Reloads React UI tree. We pass reactInstance and viewOptions that essentially represent
	//! a snapshot of current options.
	virtual Mso::Future<void> Reload(Mso::CntPtr<IReactInstance>&& reactInstance, ReactViewOptions&& viewOptions) noexcept = 0;

	//! Unloads React UI tree.
	virtual Mso::Future<void> Unload() noexcept = 0;
};

//! A host object for ReactView.
//! Each IReactViewHost has association with its parent IReactHost.
//! IReactHost has a weak reference to IReactViewHost when the IReactViewHost is bound to IReactViewInstance.
//! Typically platform-specific implementation of IReactViewInstance is created using
//! IReactViewHost instance options, and then attaches itself to the IReactViewHost.
//! When IReactViewInstance is destroyed, it detaches itself from the IReactViewHost.
MSO_STRUCT_GUID(IReactViewHost, "cc9f0c95-ad43-43af-a834-0ef9adc411c4")
struct IReactViewHost : IUnknown
{
	//! Returns a copy of current react view options.
	virtual ReactViewOptions Options() const noexcept = 0;

	//! Returns IReactHost associated with this IReactViewHost.
	virtual IReactHost& ReactHost() const noexcept = 0;

	//! Reloads the IReactViewInstance if it is attached.
	virtual Mso::Future<void> ReloadViewInstance() noexcept = 0;

	//! Reloads IReactViewInstance if it is attached with a new set of options.
	virtual Mso::Future<void> ReloadViewInstanceWithOptions(ReactViewOptions&& options) noexcept = 0;

	//! Unloads the attached IReactViewInstance.
	virtual Mso::Future<void> UnloadViewInstance() noexcept = 0;

	//! Attaches IReactViewInstance to the IReactViewHost.
	virtual Mso::Future<void> AttachViewInstance(IReactViewInstance& viewInstance) noexcept = 0;

	//! Detaches IReactViewInstance from the IReactViewHost.
	virtual Mso::Future<void> DetachViewInstance() noexcept = 0;
};

//! Creates a new instance of IReactHost.
LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost(ReactOptions&& options) noexcept;

//! Creates a new instance of IReactHost. Uses the provided onInstanceLoaded promise
//! to notify about the initial instance loading. Use Mso::Future returned from
//! the ReloadInstance call to observe instance loading after the reload.
LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost(
	ReactOptions&& options,
	Mso::Promise<void>&& onInstanceLoaded) noexcept;

//! This API will load the provided bundle to an IReactInstance and will return a TCntPtr of it.
//! Note that sdxIdentity is either relative or full path to js bundle until a proper SDXIdentity is implemented.
//! The optional onInitialized callback is called when Reka bridge is initialized in JavaScript,
//! and developers can access or create Reka objects or object proxies using the provided IRekaContext.
//! The onInitialized callback is called every time when bundle is loaded into react instance.
//! For debug scenarios onInitialized callback is called every time when bundle is changed and reloaded.
//LIBLET_PUBLICAPI_EX("android", "apple")
//Mso::CntPtr<IReactInstance> GetOrCreateReactInstance(const std::string& sdxIdentity,
//	Mso::JSHost::OnInitializedCallback&& onInitialized = nullptr) noexcept;
//noexcept
//! Get the current period of time where RN Instance will additionally be kept alive after its last RRV is dismissed.
LIBLET_PUBLICAPI std::uint16_t TestOnly_GetInstanceDebouncePeriodInMsecs() noexcept;

//! Set the period of time where RN Instance will additionally be kept alive after its last RRV is dismissed
LIBLET_PUBLICAPI void TestOnly_SetInstanceDebouncePeriodInMsecs(std::uint16_t msecs) noexcept;

//------------------------------------------------------------------------------------------------
// Aliases and functions backward compatibility
//------------------------------------------------------------------------------------------------

using SDXIdentity = std::string;
using SDXDeveloperSettings = ReactDevOptions;
using SDXDescriptor = ReactOptions;
using IReactNativeHost = IReactHost;

inline Mso::CntPtr<IReactNativeHost> MakeReactNativeHost(ReactOptions&& options) noexcept
{
	return MakeReactHost(std::move(options));
}

//! Gets the instance's Executor - not exported on Windows
//! GetRuntimeExecutor works for old reactnativehost (GetOrCreateReactInstance)  
//! GetRuntimeExecutor2 works with newer reactnativehost (MakeReactHost)
LIBLET_PUBLICAPI_APPLE facebook::react::RuntimeExecutor GetRuntimeExecutor(IReactInstance& instance) noexcept;
LIBLET_PUBLICAPI_APPLE facebook::react::RuntimeExecutor GetRuntimeExecutor2(IReactInstance& instance) noexcept;

} // namespace Mso::React

#pragma pack(pop)
