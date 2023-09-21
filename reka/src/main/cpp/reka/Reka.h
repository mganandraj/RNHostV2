#pragma once

//
// Base classes to implement and use data services.
//

#include "DataTypes.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "RekaApi.h"
#include "RekaErrorProvider.h"
#include <future/future.h>
#include <object/UnknownObject.h>
#include <type_traits>
//#include <variant>
// #include <Variant/match.h>

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

template <class TService>
struct DataService;

template <class TService, class... TExtraInterfaces>
struct RekaObject : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IRekaService, TExtraInterfaces..., Mso::QueryCastHidden<TService>>
{
	using RekaObjectBase = RekaObject;
	using RekaServiceType = TService;

// For backward compatibility
	using DataServiceType = RekaServiceType;

	IRekaContext& ServiceContext() noexcept
	{
		return *m_serviceContext;
	}

	void UnknownMethod(const char* /*methodName*/) noexcept
	{
	}

	template <class TArg>
	void RaiseEvent(Event<TArg, TService>& event, TArg&& arg) noexcept
	{
		event.Raise(*m_serviceContext, std::forward<TArg>(arg));
	}
	
	void Invoke(const char* methodName, const char* serializedValue, Mso::TCntPtr<IRekaResult>&& result) noexcept override
	{
		// These two overridings are for backward compatibility.
		// Step 1: All generated code overrides this function, so the below code doesn't take effect.
		// Step 2: Some users begin to upgrade their reka-compiler and generate the IRekaReader& version of Invoke function, this function is used to do redirections.
		// Step 3: After all users upgrade their reka-compiler version, both Invoke function here will be removed, and RekaContext will call the IRekaReader& version of Invoke.
		JsonReader reader{serializedValue};
		InvokeWithReader(methodName, reader.GetRekaReader(), std::move(result));
	}

	void InvokeWithReader(const char* /*methodName*/, IRekaReader& /*argumentsReader*/, Mso::TCntPtr<IRekaResult>&& /*result*/) noexcept override
	{
		VerifyElseCrashTag(false, 0x0281d35b /* tag_c63n1 */);
	}

protected:
	RekaObject(IRekaContext& serviceContext) noexcept
		: m_serviceContext(&serviceContext)
	{
	}

private:
	Mso::TCntPtr<IRekaContext> m_serviceContext;
};

// DataService<TService> will be deleted after everyone have updated their packages generated by the lastest tsnm-compiler released before SEP 2018.
// There is no one using IRekaObjectConstants for now. So we can make the default DataService<TService> incompatible with services that have constants.
// Because when they add constants, the compiler will generate new code for them.

template <class TService>
struct DataService : RekaObject<TService>
{
	using DataServiceBase = DataService;

	DataService(IRekaContext& serviceContext) noexcept
		: RekaObject<TService>(serviceContext)
	{
	}

	virtual void Invoke(const char* /*methodName*/, const char* /*serializedValue*/,
		Mso::TCntPtr<IRekaResult>&& /*result*/) noexcept override
	{
		VerifyElseCrashSzTag(false, "DataService<TService>::Invoke method must be specialized for the TService type.", 0x0281d35c /* tag_c63n2 */);
	}
};

inline bool IsSameMethodName(const char* methodName1, const char* methodName2) noexcept
{
	return strcmp(methodName1, methodName2) == 0;
}

template <class TService>
Mso::TCntPtr<TService> MakeServiceProxy(IRekaContext& context) noexcept;

template <class TResult>
void ToJsResult(const Mso::Future<TResult>& result, Mso::TCntPtr<IRekaResult>&& jsResult) noexcept
{
	result.template Then<Mso::Executors::Inline>([jsr = std::move(jsResult)](const Mso::Maybe<TResult>& value) noexcept {
		if (value.IsValue())
		{
			jsr->OnSuccess(Mso::JSHost::ArgsToJson(value.GetValue()).c_str());
		}
		else
		{
			jsr->OnFailure(Mso::JSHost::ArgsToJson(value.GetError().ToString()).c_str());
		}
	});
}

template <>
inline void ToJsResult(const Mso::Future<void>& result, Mso::TCntPtr<IRekaResult>&& jsResult) noexcept
{
	result.template Then<Mso::Executors::Inline>([jsr = std::move(jsResult)](const Mso::Maybe<void>& value) noexcept {
		if (value.IsValue())
		{
			jsr->OnSuccess("");
		}
		else
		{
			jsr->OnFailure(Mso::JSHost::ArgsToJson(value.GetError().ToString()).c_str());
		}
	});
}

template <class TResult>
struct JsRekaServiceResult : Mso::UnknownObject<IRekaResult>
{
	void OnSuccess(const char* serializedValue) noexcept override
	{
		TResult result;
		Mso::JSHost::ArgsFromJson(serializedValue, result);
		m_resultPromise.SetValue(std::move(result));
	}

	void OnFailure(const char* serializedError) noexcept override
	{
		m_resultPromise.SetError(RekaErrorProvider().MakeErrorCode(RekaError(serializedError)));
	}

	Mso::Future<TResult> GetFuture() noexcept
	{
		return m_resultPromise.AsFuture();
	}

private:
	Mso::Promise<TResult> m_resultPromise;
};

// Calls JavaScript function
// and observes its result.
template <class TResult>
Mso::Future<TResult> ToNativeResult(
	IRekaContext& context,
	const char* serviceName,
	const char* methodName,
	const char* serializedValue) noexcept
{
	auto jsRekaServiceResult = Mso::Make<JsRekaServiceResult<TResult>>();
	Mso::Future<TResult> result = jsRekaServiceResult->GetFuture();
	Mso::TCntPtr<IRekaResult> serviceResult = std::move(jsRekaServiceResult);
	context.CallJavaScript(serviceName, methodName, serializedValue, std::move(serviceResult));
	return result;
}

// Calls JavaScript function
// and observes its result.
template <class TResult>
Mso::Future<TResult> ToNativeResultAsync(const Mso::TCntPtr<IRekaCallContext>& callContext) noexcept
{
	auto jsRekaServiceResult = Mso::Make<JsRekaServiceResult<TResult>>();
	Mso::Future<TResult> result = jsRekaServiceResult->GetFuture();
	callContext->AsyncCall(std::move(jsRekaServiceResult));
	return result;
}

// Emit a constant value
template <class TConst>
void EmitConstant(const EnumerateRekaConstantsCallback& emitConstant, const char* propertyName, TConst&& propertyValue)
{
	auto constantWriter = emitConstant(propertyName);
	ArgsToWriter(constantWriter->GetWriter(), std::forward<TConst&&>(propertyValue));
	constantWriter->Submit();
}

// Invokes Reka Services implemented in JavaScript.
template <class TService>
struct RekaServiceProxy;

template <class TService>
Mso::TCntPtr<TService> MakeRekaServiceProxy(IRekaContext& context) noexcept;

// For backward compatibility

template <class TService>
struct DataServiceProxy;

template <class TService>
inline Mso::TCntPtr<TService> MakeDataServiceProxy(IRekaContext& context) noexcept
{
	 return MakeRekaServiceProxy<TService>(context);
};
}} // namespace Mso::JSHost

#pragma pack(pop)
