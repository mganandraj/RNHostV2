#include "precomp.h"
#include "PropertyBag_internal.h"
#include "RekaContext.h"
#include "RekaInstance.h"
#include "RekaCallContext.h"
#include <dispatchqueue/dispatchqueue.h>

namespace Mso { namespace JSHost {

//=============================================================================================
// RekaContext implementation
//=============================================================================================

RekaContext::RekaContext(Mso::WeakPtr<RekaInstance>&& rekaInstanceWeak, const PropertyBag& properties) noexcept
	: m_rekaInstanceWeak{std::move(rekaInstanceWeak)}
	, m_properties{Mso::Make<WeakPropertyBagProxy, IPropertyBag>(properties)}
{
}

RekaContext::~RekaContext() noexcept
{
}

void RekaContext::ExecutePromiseIdOnSuccess(int32_t promiseId, const char* serializedValue) noexcept
{
	auto rekaInstance = m_rekaInstanceWeak.GetStrongPtr();
	if (rekaInstance)
	{
		rekaInstance->InvokeJsExecutor({Names::RekaBridge, Names::OnSuccess, serializedValue, promiseId});
	}
}

void RekaContext::ExecutePromiseIdOnFailure(int32_t promiseId, const char* serializedError) noexcept
{
	auto rekaInstance = m_rekaInstanceWeak.GetStrongPtr();
	if (rekaInstance)
	{
		rekaInstance->InvokeJsExecutor({Names::RekaBridge, Names::OnFailure, serializedError, promiseId});
	}
}

void RekaContext::CallJavaScript(
	const char* serviceName,
	const char* methodName,
	const char* serializedValue,
	Mso::TCntPtr<IRekaResult>&& result) const noexcept
{
	if (auto rekaInstance = m_rekaInstanceWeak.GetStrongPtr())
	{
		int32_t promiseId = rekaInstance->GetJsPromiseId(std::move(result));
		rekaInstance->InvokeJsExecutor({serviceName, methodName, serializedValue, promiseId});
	}
	else if (result)
	{
		result->OnFailure("[\"RekaInstance was destroyed\"]");
	}
}

Mso::TCntPtr<IRekaCallContext> RekaContext::PrepareCall(const char* serviceName, const char* methodName) const noexcept
{
	return Mso::Make<RekaJsonCallContext, IRekaCallContext>(
		serviceName,
		methodName,
		[spThis = Mso::TCntPtr<RekaContext> { const_cast<RekaContext*>(this) }](
			const char* serviceNameParam,
			const char* methodNameParam,
			std::string&& jsonArgs,
			Mso::TCntPtr<IRekaResult>&& result) noexcept
	{
		if (auto rekaInstance = spThis->m_rekaInstanceWeak.GetStrongPtr())
		{
			int32_t promiseId = rekaInstance->GetJsPromiseId(std::move(result));
			rekaInstance->InvokeJsExecutor(RekaBridgeInvokeArgs {
				serviceNameParam,
				methodNameParam,
				jsonArgs.c_str(),
				promiseId });
		}
		else if (result)
		{
			result->OnFailure("[\"RekaInstance was destroyed\"]");
		}
	});
}

Mso::Async::IDispatchQueue& RekaContext::GetDispatchQueue() const noexcept
{
	if (auto rekaInstance = m_rekaInstanceWeak.GetStrongPtr())
	{
		return rekaInstance->GetDispatchQueue();
	}

	VerifyElseCrashSzTag(false, "RekaInstance was destroyed", 0x0281d348 /* tag_c63ni */);
}

const PropertyBag& RekaContext::Properties() const noexcept
{
	return m_properties;
}

const int RekaServiceProviderIsolationKey{};
const IsolatedNamedProperty<IRekaServiceProvider>
	RekaServiceProviderProperty{&RekaServiceProviderIsolationKey, "RekaServiceProvider"};

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> GetRekaServiceProvider(const IRekaContext& context) noexcept
{
	return context.Properties().Get(RekaServiceProviderProperty);
}

void SetRekaServiceProvider(const IRekaContext& context, IRekaServiceProvider& serviceProvider) noexcept
{
	context.Properties().Set(RekaServiceProviderProperty, &serviceProvider);
}

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaService> GetRekaService(const char* serviceName, IRekaContext& context) noexcept
{
	auto serviceProvider = GetRekaServiceProvider(context);
	return serviceProvider ? serviceProvider->GetRekaService(serviceName, context) : nullptr;
}

}} // namespace Mso::JSHost
