#pragma once

//
// Public API for the JSHost DataService
//

#include <compileradapters/functiondecorations.h>
#include <platformadapters/windowsfirst.h>
#include <guid/msoguid.h>
#include <compileradapters/declspecDefinitions.h>

#include "PropertyBag.h"
#include <functional/Functor.h>
#include <dispatchqueue/dispatchqueue.h>
#include <ErrorCode/ErrorCode.h>

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

struct RekaBridgeInvokeArgs
{
	const char* ServiceName;
	const char* MethodName;
	const char* SerializedValue;
	int32_t PromiseId;
};

struct IRekaContext;
struct IRekaReader;
struct IRekaWriter;

using EnumerateConstantsCallback = Mso::FunctorRef<void(const char* serviceName, const char* propertyName, const char* serializedJson)>;
using EnumerateObjectConstantsCallback = Mso::FunctorRef<void(const char* propertyName, const char* serializedJson)>;

//! By implementing IRekaConstantsProvider in your data service provider with QueryCastChain
//! The native module knows all services which provide constants
MSO_STRUCT_GUID(IRekaConstantsProvider, "09177608-477f-4b02-8170-1ad6445772e2")
struct DECLSPEC_NOVTABLE IRekaConstantsProvider : IUnknown
{
	virtual void EnumerateConstants(const EnumerateConstantsCallback& callback, IRekaContext& serviceContext) noexcept = 0;
};

//! By publicly implementing IRekaObjectConstants in your service class
//! DataServiceRegistration<T> knows this service provides constants.
MSO_STRUCT_GUID(IRekaObjectConstants, "be48b38a-af97-43cf-8975-818c041a4e13")
struct DECLSPEC_NOVTABLE IRekaObjectConstants : IUnknown
{
	virtual void EnumerateConstants(const EnumerateObjectConstantsCallback& callback) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaConstantWriter, "906b59d3-4510-46f8-a6c8-da4b6e48f824")
struct DECLSPEC_NOVTABLE IRekaConstantWriter : IUnknown
{
	virtual IRekaWriter& GetWriter() noexcept = 0;
	virtual void Submit() noexcept = 0;
};

using EnumerateRekaConstantsCallback = Mso::FunctorRef<Mso::TCntPtr<IRekaConstantWriter>(const char* propertyName)>;

//! An upgraded version of IRekaObjectConstants
//! IRekaObjectConstants will be removed along with IRekaService::Invoke
MSO_STRUCT_GUID(IRekaConstants, "127139d5-b782-4656-a6be-0e1f616d1b6e")
struct DECLSPEC_NOVTABLE IRekaConstants : IUnknown
{
	virtual void EnumerateConstants(const EnumerateRekaConstantsCallback& callback) noexcept = 0;
};

//! IRekaBridge interface is used to call code from JavaScript to C++ code, and from the
//! C++ code back to JavaScript.
//! There must be two different implementations: one for each call direction.
//! The jshost_dataservice liblet provides an implementation for C++ code calls.
//! It is expected that code outside of jshost_dataservice implements the counterpart to call JavaScript.
//! The jshost_dataservice implementation is created by calling MakeRekaInstance method.
MSO_STRUCT_GUID(IRekaBridge, "d3140c59-60f6-4524-b587-86f1b8c325ca")
struct DECLSPEC_NOVTABLE IRekaBridge : IUnknown
{
	/// Invokes code with parameters provided through the
	virtual void Invoke(const RekaBridgeInvokeArgs& args) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaResult, "235b566c-cb0f-43e7-9e96-889f33984122")
struct DECLSPEC_NOVTABLE IRekaResult : IUnknown
{
	virtual void OnSuccess(const char* serializedValue) noexcept = 0;
	virtual void OnFailure(const char* serializedError) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaService, "28e6010d-1e3f-42de-b8a9-eabe5893d2ee")
struct DECLSPEC_NOVTABLE IRekaService : IUnknown
{
	// This method is being deprecated
	virtual void Invoke(const char* methodName, const char* serializedValue, Mso::TCntPtr<IRekaResult>&& result) noexcept = 0;

	// This is the new method that we are going to use in future
	virtual void InvokeWithReader(const char* /*methodName*/, IRekaReader& /*argumentsReader*/, Mso::TCntPtr<IRekaResult>&& /*result*/) noexcept
	{
		VerifyElseCrashSzTag(false, "Must be overridden in derived class", 0x0281d35d /* tag_c63n3 */);
	}
};

// TODO: move it to QueryCast.h
// The goal of this function is to up cast to TObject while not changing ref count.
template <class TObject>
static Mso::TCntPtr<TObject> TryCast(Mso::TCntPtr<IUnknown>&& object) noexcept
{
	Mso::TCntPtr<TObject> result;
	TObject* castedObj = query_cast<TObject*>(object.Get());
	if (castedObj)
	{
		result.Attach(castedObj);
		object.Detach();
	}

	return result;
}

MSO_STRUCT_GUID(IRekaCallContext, "a77980d8-a6f9-4668-b7fb-599aada50e66")
struct DECLSPEC_NOVTABLE IRekaCallContext : IUnknown
{
	virtual IRekaWriter& GetWriter() noexcept = 0;
	virtual void AsyncCall(Mso::TCntPtr<IRekaResult>&& result = nullptr) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaContext, "86243e9e-d346-4b69-95ab-39b3d4c33aa7")
struct DECLSPEC_NOVTABLE IRekaContext : IUnknown
{
	// Initiates a remote call.
	virtual _SA_deprecated_(Mso::JSHost::IRekaContext::PrepareCall())
		void CallJavaScript(const char* serviceName, const char* methodName, const char* serializedValue,
		Mso::TCntPtr<IRekaResult>&& result = nullptr) const noexcept = 0;

	//! Creates IRekaCallContext object.
	//! IRekaCallContext can be used to call Reka service.
	virtual Mso::TCntPtr<IRekaCallContext> PrepareCall(const char* serviceName, const char* methodName) const noexcept = 0;

	//! Gets default dispatch queue associated with the Reka instance.
	virtual Mso::DispatchQueue GetDispatchQueue() const noexcept = 0;

	//! Returns properties associated with the Reka context.
	//! The returned PropertyBag is a weak pointer the actual PropertyBag implementation.
	//! It is safe to be referenced from object being stored in the PropertyBag.
	//! Property access is thread safe too.
	virtual const PropertyBag& Properties() const noexcept = 0;
};

//! The IRekaContextProxy can be used to provide a single IRekaContext
//! instance that works while IRekaInstance is recreated multiple times.
//! Until SetContext or SetError are called, the IRekaContextProxy queues up all incoming calls.
MSO_STRUCT_GUID(IRekaContextProxy, "6520d018-25cf-49fa-a3ff-2ec3fa4e2be4")
struct DECLSPEC_NOVTABLE IRekaContextProxy : Mso::QueryCastList<IRekaContext>
{
	//! Resets state to the initial when we queue up all incoming calls.
	virtual void ResetState() noexcept = 0;

	//! Sets the Reka context. We use it for all pending or incoming calls until we get an error state.
	virtual void SetContext(IRekaContext& context) noexcept = 0;

	//! Sets the state to error state and cancel all pending or incoming calls.
	virtual void SetError(Mso::ErrorCode&& errorcode) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaInstance, "6678614b-c899-49c1-b0f8-02893a4cd3e2")
struct DECLSPEC_NOVTABLE IRekaInstance : Mso::QueryCastList<IRekaBridge>
{
	//! Releases RekaInstance
	//! Allows all calls which are in the middle of execution to complete
	//! Prevents new invocations
	virtual void Destroy() noexcept = 0;

	//! Retrieves all constants
	virtual void EnumerateConstants(const EnumerateConstantsCallback& callback) noexcept = 0;
};

MSO_STRUCT_GUID(IRekaServiceProvider, "a84c08d9-10da-4ddd-92af-b27fafa747c8")
struct DECLSPEC_NOVTABLE IRekaServiceProvider : IUnknown
{
	virtual Mso::TCntPtr<IRekaService> GetRekaService(const char* serviceName, IRekaContext& serviceContext) noexcept = 0;
};

using RekaServiceProviderFactory = Mso::Functor<Mso::TCntPtr<IRekaServiceProvider>(IRekaContext&)>;
using OnInitializedCallback = Mso::Functor<void(IRekaContext&)>;
using OnDestroyedCallback = Mso::Functor<void(IRekaContext&)>;

//! Options to create native RekaBridge
//! JsExecutor - to invoke JavaScript code
//! RekaServiceProviderFactory - custom factory for a IRekaServiceProvider
//! NativeQueue - default queue to invoke data services
//! OnInitialized - a callback to call when the Reka JavaScript code is initialized.
//! OnDestroyedCallback - a callback to call when Reka is destroyed
struct RekaBridgeOptions
{
	/*required*/ Mso::TCntPtr<IRekaBridge> JsExecutor;
	/*optional*/ RekaServiceProviderFactory RekaServiceProviderFactory;
	/*optional*/ Mso::DispatchQueue NativeQueue;
	/*optional*/ OnInitializedCallback OnInitialized;
	/*optional*/ OnDestroyedCallback OnDestroyed;
};

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaInstance> MakeRekaInstance(RekaBridgeOptions&& options) noexcept;

//! Creates Reka Instance and also gets Reka Context object.
LIBLET_PUBLICAPI Mso::TCntPtr<IRekaInstance> MakeRekaInstance(RekaBridgeOptions&& options, /*out*/ Mso::TCntPtr<Mso::JSHost::IRekaContext>& context) noexcept;

LIBLET_PUBLICAPI const char* DefaultRekaServiceProviderName() noexcept;

LIBLET_PUBLICAPI void RegisterRekaServiceProvider(const char* providerName, Mso::TCntPtr<IRekaServiceProvider>&& provider) noexcept;
LIBLET_PUBLICAPI void RegisterRekaServiceProviderFactory(const char* providerName, RekaServiceProviderFactory&& providerFactory) noexcept;

LIBLET_PUBLICAPI void UnregisterRekaServiceProvider(const char* providerName) noexcept;
LIBLET_PUBLICAPI void UnregisterRekaServiceProviderFactory(const char* providerName) noexcept;

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> GetOrMakeRekaServiceProvider(
	const char* providerName, IRekaContext& context) noexcept;

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> GetRekaServiceProvider(const IRekaContext& context) noexcept;
LIBLET_PUBLICAPI Mso::TCntPtr<IRekaService> GetRekaService(const char* serviceName, IRekaContext& context) noexcept;

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaService> MakeRekaQueueService(
	Mso::DispatchQueue queue,
	Mso::Functor<void(Mso::TCntPtr<IRekaService>&)>&& rekaServiceInit) noexcept;

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> MakeRegisteredRekaServiceProvider() noexcept;

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaContextProxy> MakeRekaContextProxy(Mso::DispatchQueue queue) noexcept;

// Keep old names for now. After new names are used everywhere, these old names should be removed.
using IDataServiceContext = IRekaContext;
using IDataService = IRekaService;
using IDataServiceResult = IRekaResult;
using IDataServiceProvider = IRekaServiceProvider;
using DataServiceProviderFactory = RekaServiceProviderFactory;

inline Mso::TCntPtr<IRekaInstance> MakeDataServiceManager(RekaBridgeOptions&& options) noexcept
{
	return MakeRekaInstance(std::move(options));
}

inline Mso::TCntPtr<IRekaInstance> MakeDataServiceManager(RekaBridgeOptions&& options, /*out*/ Mso::TCntPtr<Mso::JSHost::IDataServiceContext>& context) noexcept
{
	return MakeRekaInstance(std::move(options), /*out*/ context);
}

inline const char* DefaultDataServiceProviderName() noexcept
{
	return DefaultRekaServiceProviderName();
}

inline void RegisterDataServiceProvider(const char* providerName, Mso::TCntPtr<IDataServiceProvider>&& provider) noexcept
{
	RegisterRekaServiceProvider(providerName, std::move(provider));
}

inline void RegisterDataServiceProviderFactory(const char* providerName, DataServiceProviderFactory&& providerFactory) noexcept
{
	RegisterRekaServiceProviderFactory(providerName, std::move(providerFactory));
}

inline void UnregisterDataServiceProvider(const char* providerName) noexcept
{
	UnregisterRekaServiceProvider(providerName);
}

inline void UnregisterDataServiceProviderFactory(const char* providerName) noexcept
{
	UnregisterRekaServiceProviderFactory(providerName);
}

inline Mso::TCntPtr<IDataServiceProvider> GetOrMakeDataServiceProvider(
	const char* providerName, IDataServiceContext& context) noexcept
{
	return GetOrMakeRekaServiceProvider(providerName, context);
}

inline Mso::TCntPtr<IDataServiceProvider> GetDataServiceProvider(const IDataServiceContext& context) noexcept
{
	return GetRekaServiceProvider(context);
}

inline Mso::TCntPtr<IDataService> GetDataService(const char* serviceName, IDataServiceContext& context) noexcept
{
	return GetRekaService(serviceName, context);
}

inline Mso::TCntPtr<IDataServiceProvider> MakeRegisteredDataServiceProvider() noexcept
{
	return MakeRegisteredRekaServiceProvider();
}
}} // namespace Mso::JSHost

#pragma pack(pop)