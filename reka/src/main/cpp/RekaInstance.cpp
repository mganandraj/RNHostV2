#include "precomp.h"
#include "RekaContext.h"
#include "RekaInstance.h"
#include "RekaServiceProviderRegistration.h"
#include <dispatchqueue/dispatchqueue.h>

namespace Mso { namespace JSHost {

//=============================================================================================
// MovableRekaBridgeInvokeArgs implementation
//=============================================================================================

_Noexcept_mayterminate_ MovableRekaBridgeInvokeArgs::MovableRekaBridgeInvokeArgs(const RekaBridgeInvokeArgs& args) noexcept
	: ServiceName{args.ServiceName}
	, MethodName{args.MethodName}
	, SerializedValue{args.SerializedValue}
	, PromiseId{args.PromiseId}
{
}

RekaBridgeInvokeArgs MovableRekaBridgeInvokeArgs::AsRekaBridgeInvokeArgs() const noexcept
{
	return {
		ServiceName.c_str(),
		MethodName.c_str(),
		SerializedValue.c_str(),
		PromiseId};
}

//=============================================================================================
// RekaInstance implementation
//=============================================================================================

RekaInstance::RekaInstance(RekaBridgeOptions&& options) noexcept
	: m_dispatchQueue{std::move(options.NativeQueue)}
	, m_jsExecutor{std::move(options.JsExecutor)}
	, m_onInitialized{std::move(options.OnInitialized)}
	, m_onDestroyed{std::move(options.OnDestroyed)}
{
	VerifyElseCrashTag(m_jsExecutor, 0x0281d34c /* tag_c63nm */);

	if (!m_dispatchQueue)
	{
		m_dispatchQueue = Mso::DispatchQueue::MakeSerialQueue();
	}

	m_serviceContext = Mso::Make<RekaContext>(this, m_properties);

	if (options.RekaServiceProviderFactory)
	{
		m_serviceProvider = options.RekaServiceProviderFactory(*m_serviceContext);
	}

	if (!m_serviceProvider)
	{
		m_serviceProvider = MakeRegisteredRekaServiceProvider();
	}

	SetRekaServiceProvider(*m_serviceContext, *m_serviceProvider);
}

void RekaInstance::Destroy() noexcept
{
	if (m_onDestroyed)
	{
		m_dispatchQueue.InvokeElsePost(	[rekaInstance = Mso::TCntPtr<RekaInstance>(this) /*Copy*/]() noexcept {
				// Note, RekaInstance is kept alive
				// during onDestroyed callback.
				// Thus users can access any stored objects from
				// Context.
				rekaInstance->m_onDestroyed(*rekaInstance->m_serviceContext);
			});
	}
}

void RekaInstance::EnumerateConstants(const EnumerateConstantsCallback& callback) noexcept
{
	if (auto constantsProvider = query_cast<IRekaConstantsProvider*>(m_serviceProvider.Get()))
	{
		constantsProvider->EnumerateConstants(callback, *m_serviceContext);
	}
}

_Noexcept_mayterminate_ RekaInstance::~RekaInstance() noexcept
{
	m_dispatchQueue.InvokeElsePost(	[jsResults = std::move(m_jsResults)]() noexcept {
			// Call OnFailure() for leftover m_jsResults
			for (const auto& jsResultEntry : jsResults)
			{
				jsResultEntry.second->OnFailure("[\"RekaInstance was destroyed\"]");
			}
		});
}

Mso::DispatchQueue RekaInstance::GetDispatchQueue() noexcept
{
	return m_dispatchQueue;
}

int32_t RekaInstance::GetJsPromiseId(Mso::TCntPtr<IRekaResult>&& result) noexcept
{
	int32_t promiseId = 0;

	if (result)
	{
		std::lock_guard<std::mutex> locker(m_lock);
		promiseId = ++m_nextJsPromiseId;
		m_jsResults.insert({promiseId, std::move(result)});
	}

	return promiseId;
}

const PropertyBag& RekaInstance::Properties() const noexcept
{
	return m_properties;
}

const Mso::TCntPtr<RekaContext>& RekaInstance::GetContext() const noexcept
{
	return m_serviceContext;
}

void RekaInstance::Invoke(const RekaBridgeInvokeArgs& args) noexcept
{
	m_dispatchQueue.Post([weakPtr = Mso::WeakPtr<RekaInstance>{this},
			args = MovableRekaBridgeInvokeArgs{args}]() noexcept {

			auto spThis = weakPtr.GetStrongPtr();
			if (spThis)
			{
				if (args.ServiceName == Names::RekaBridge)
				{
					// Invoke Reka Internals
					spThis->HandleRekaBridge(args);
				}
				else
				{
					// Invoke RekaService
					auto rekaService = spThis->GetRekaService(args.ServiceName.c_str());
					auto result = spThis->MakeNativeRekaResult(args.PromiseId);
					rekaService->Invoke(args.MethodName.c_str(), args.SerializedValue.c_str(), std::move(result));
				}
			}
		});
}

void RekaInstance::HandleRekaBridge(const MovableRekaBridgeInvokeArgs& args) noexcept
{
	if (args.MethodName == Names::OnSuccess)
	{
		TakeJsRekaResult(args.PromiseId)->OnSuccess(args.SerializedValue.c_str());
	}
	else if (args.MethodName == Names::OnFailure)
	{
		TakeJsRekaResult(args.PromiseId)->OnFailure(args.SerializedValue.c_str());
	}
	else if (args.MethodName == Names::Init)
	{
		if (m_onInitialized)
		{
			m_onInitialized(*m_serviceContext);
		}
	}
	else if (args.MethodName == Names::CreateService)
	{
		// This will create the service
		auto rekaService = GetRekaService(args.SerializedValue.c_str());
	}
}

Mso::TCntPtr<IRekaService> RekaInstance::GetRekaService(const char* serviceName) noexcept
{
	Mso::TCntPtr<IRekaService> dataService = m_serviceProvider->GetRekaService(serviceName, *m_serviceContext);
	VerifyElseCrashSzTag(dataService, "Could not get service", 0x0281d34d /* tag_c63nn */);
	return dataService;
}

Mso::TCntPtr<IRekaResult> RekaInstance::MakeNativeRekaResult(int32_t promiseId) noexcept
{
	Mso::TCntPtr<IRekaResult> result;
	if (promiseId != 0)
	{
		result = Mso::Make<NativeRekaResult, IRekaResult>(Mso::TCntPtr<RekaContext>(m_serviceContext), promiseId);
	}

	return result;
}

Mso::TCntPtr<IRekaResult> RekaInstance::TakeJsRekaResult(int32_t promiseId) noexcept
{
	Mso::TCntPtr<IRekaResult> result;

	std::lock_guard<std::mutex> locker(m_lock);
	auto it = m_jsResults.find(promiseId);
	if (it != m_jsResults.end())
	{
		result = std::move(it->second);
		m_jsResults.erase(it);
	}

	return result;
}

void RekaInstance::InvokeJsExecutor(const RekaBridgeInvokeArgs& args) const noexcept
{
	m_jsExecutor->Invoke(args);
}

//=============================================================================================
// RekaInstanceOwner implementation
//=============================================================================================

void RekaInstanceOwner::Invoke(const RekaBridgeInvokeArgs& args) noexcept
{
	Mso::TCntPtr<RekaInstance> rekaInstance = GetRekaInstance();

	if (rekaInstance)
	{
		rekaInstance->Invoke(args);
	}
}

Mso::TCntPtr<Mso::JSHost::RekaContext> RekaInstanceOwner::GetContext() noexcept
{
	Mso::TCntPtr<RekaInstance> rekaInstance = GetRekaInstance();

	if (rekaInstance)
	{
		return rekaInstance->GetContext();
	}

	return nullptr;
}

// Returns RekaInstance object.
// If Destroy() Api has been called, returns nullptr.
Mso::TCntPtr<RekaInstance> RekaInstanceOwner::GetRekaInstance() noexcept
{
	Mso::TCntPtr<RekaInstance> rekaInstance;

	{
		std::lock_guard<std::mutex> locker{m_lock};
		rekaInstance = m_rekaInstance;
	}

	return rekaInstance;
}

void RekaInstanceOwner::Destroy() noexcept
{
	Mso::TCntPtr<RekaInstance> rekaInstance;

	{
		std::lock_guard<std::mutex> locker{m_lock};
		rekaInstance = std::move(m_rekaInstance);
		// Note: m_rekaInstance becomes nullptr
	}

	DestroyInternal(std::move(rekaInstance));
}

void RekaInstanceOwner::EnumerateConstants(const EnumerateConstantsCallback& callback) noexcept
{
	if (auto rekaInstance = GetRekaInstance())
	{
		rekaInstance->EnumerateConstants(callback);
	}
}

RekaInstanceOwner::RekaInstanceOwner(RekaBridgeOptions&& options) noexcept
{
	m_rekaInstance = Mso::Make<RekaInstance>(std::move(options));
}

RekaInstanceOwner::~RekaInstanceOwner() noexcept
{
	// No lock is needed inside destructor
	DestroyInternal(std::move(m_rekaInstance));
}

/*static*/ void RekaInstanceOwner::DestroyInternal(Mso::TCntPtr<RekaInstance>&& rekaInstance) noexcept
{
	// If rekaInstance is null, then it was already destroyed.
	if (rekaInstance)
	{
		// Make sure that Reka instance is destroyed in the queue associated with it.
		Mso::DispatchQueue queue{rekaInstance->GetDispatchQueue()};
		queue.InvokeElsePost( [rekaInstance = std::move(rekaInstance)]() noexcept {
			rekaInstance->Destroy();
		});
	}
}

//=============================================================================================
// NativeRekaResult implementation
//=============================================================================================

NativeRekaResult::NativeRekaResult(Mso::TCntPtr<RekaContext>&& context, int32_t promiseId) noexcept
	: m_context(std::move(context))
	, m_promiseId(promiseId)
{
}

NativeRekaResult::~NativeRekaResult() noexcept
{
}

void NativeRekaResult::OnSuccess(const char* serializedValue) noexcept
{
	m_context->ExecutePromiseIdOnSuccess(m_promiseId, serializedValue);
}

void NativeRekaResult::OnFailure(const char* serializedError) noexcept
{
	m_context->ExecutePromiseIdOnFailure(m_promiseId, serializedError);
}

LIBLET_PUBLICAPI /*static*/ Mso::TCntPtr<IRekaInstance> MakeRekaInstance(RekaBridgeOptions&& options) noexcept
{
	return Mso::Make<RekaInstanceOwner, IRekaInstance>(std::move(options));
}

LIBLET_PUBLICAPI /*static*/ Mso::TCntPtr<IRekaInstance> MakeRekaInstance(RekaBridgeOptions&& options, Mso::TCntPtr<Mso::JSHost::IRekaContext>& context) noexcept
{
	auto rekaInstance = Mso::Make<RekaInstanceOwner>(std::move(options));
	context = rekaInstance->GetContext();
	return Mso::TCntPtr<IRekaInstance>(std::move(rekaInstance));
}

}} // namespace Mso::JSHost
