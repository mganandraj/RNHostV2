#include "precomp.h"
#include "RekaContextProxy.h"
#include "RekaCallContext.h"
#include <dispatchqueue/dispatchqueue.h>

namespace Mso { namespace JSHost {

//=============================================================================================
// RekaContextProxy implementation
//=============================================================================================

RekaContextProxy::RekaContextProxy(Mso::DispatchQueue queue) noexcept
	: Super { std::move(queue) }
{
}

RekaContextProxy::~RekaContextProxy() noexcept
{
}

void RekaContextProxy::CallRemote(RemoteCallData&& callData) const noexcept
{
	InvokeInQueueStrong([this, callData = std::move(callData)]() mutable noexcept
	{
		if (const auto& errorCode = m_errorCode.Load())
		{
			callData.Result->OnFailure(ArgsToJson(errorCode.ToString()).c_str());
		}
		else if (const auto& rekaContext = m_rekaContext.Load())
		{
			rekaContext->CallJavaScript(
				callData.ServiceName.c_str(),
				callData.MethodName.c_str(),
				callData.JsonArgs.c_str(),
				std::move(callData.Result));
		}
		else
		{
			m_callQueue.Load().push_back(std::move(callData));
		}
	});
}

void RekaContextProxy::CallJavaScript(
	const char* serviceName,
	const char* methodName,
	const char* serializedValue,
	Mso::TCntPtr<IRekaResult>&& result) const noexcept
{
	CallRemote(RemoteCallData { serviceName, methodName, serializedValue, std::move(result) });
}

Mso::TCntPtr<IRekaCallContext> RekaContextProxy::PrepareCall(const char* serviceName, const char* methodName) const noexcept
{
	return Mso::Make<RekaJsonCallContext, IRekaCallContext>(serviceName, methodName,
		[spThis = Mso::TCntPtr<const RekaContextProxy> { this }](
			const char* serviceNameParam,
			const char* methodNameParam,
			std::string&& jsonArgs,
			Mso::TCntPtr<IRekaResult>&& result) noexcept
	{
		spThis->CallRemote(RemoteCallData { serviceNameParam, methodNameParam, std::move(jsonArgs), std::move(result) });
	});
}

void RekaContextProxy::SetContext(IRekaContext& rekaContext) noexcept
{
	InvokeInQueueStrong([this, rekaContext = Mso::TCntPtr<IRekaContext> { &rekaContext }]() mutable noexcept
	{
		VerifyElseCrashSzTag(!m_errorCode.Load(), "ErrorCode must not be set", 0x0285e288 /* tag_c74ki */);

		auto& callQueue = m_callQueue.Load();
		for (auto& callData : callQueue)
		{
			rekaContext->CallJavaScript(
				callData.ServiceName.c_str(),
				callData.MethodName.c_str(),
				callData.JsonArgs.c_str(),
				std::move(callData.Result));
		}

		callQueue.clear();
		m_rekaContext.Store(std::move(rekaContext));
	});
}

void RekaContextProxy::SetError(Mso::ErrorCode&& errorCode) noexcept
{
	InvokeInQueueStrong([this, errorCode = std::move(errorCode)]() mutable noexcept
	{
		DrainQueueWithError("Hit Error: " + std::string(errorCode.ToString()));
		m_rekaContext.Store(nullptr);
		m_errorCode.Store(std::move(errorCode));
	});
}

void RekaContextProxy::ResetState() noexcept
{
	InvokeInQueueStrong([this]() noexcept
	{
		m_rekaContext.Store(nullptr);
		m_errorCode.Store({});
		DrainQueueWithError("RekaContext is resetting");
	});
}

void RekaContextProxy::DrainQueueWithError(const std::string& errorMessage) noexcept
{
	std::string serializedError = ArgsToJson(errorMessage.c_str());
	auto& callQueue = m_callQueue.Load();
	for (auto& callData : callQueue)
	{
		callData.Result->OnFailure(serializedError.c_str());
	}

	callQueue.clear();
}

Mso::DispatchQueue RekaContextProxy::GetDispatchQueue() const noexcept
{
	return Queue();
}

const PropertyBag& RekaContextProxy::Properties() const noexcept
{
	VerifyElseCrashSzTag(false, "Properties is not implemented", 0x0281d34b /* tag_c63nl */);
}

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaContextProxy> MakeRekaContextProxy(Mso::DispatchQueue queue) noexcept
{
	return Mso::Make<RekaContextProxy, IRekaContextProxy>(std::move(queue));
}

}} // namespace Mso::JSHost
