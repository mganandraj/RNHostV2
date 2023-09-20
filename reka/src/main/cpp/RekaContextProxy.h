#pragma once

#include <mutex>
#include <vector>
#include <string>

#include <activeObject/ActiveObject.h>
#include <reka/RekaApi.h>

namespace Mso { namespace JSHost {

struct RemoteCallData
{
	std::string ServiceName;
	std::string MethodName;
	std::string JsonArgs;
	Mso::TCntPtr<IRekaResult> Result;
};

struct RekaContextProxy : Mso::ActiveObject<IRekaContextProxy>
{
public: // IRekaContext interface
	void CallJavaScript(const char* serviceName, const char* methodName, const char* serializedValue,
		Mso::TCntPtr<IRekaResult>&& result) const noexcept override;

	Mso::TCntPtr<IRekaCallContext> PrepareCall(const char* serviceName, const char* methodName) const noexcept override;

	Mso::DispatchQueue GetDispatchQueue() const noexcept override;

	const PropertyBag& Properties() const noexcept override;

public: // IRekaContextProxy interface
	void ResetState() noexcept override;
	void SetContext(IRekaContext& rekaContext) noexcept override;
	void SetError(Mso::ErrorCode&& errorCode) noexcept override;

private:
	using Super = ActiveObjectType;
	friend MakePolicy;
	RekaContextProxy(Mso::DispatchQueue queue) noexcept;
	~RekaContextProxy() noexcept override;
	void CallRemote(RemoteCallData&& callData) const noexcept;
	void DrainQueueWithError(const std::string& serializedError) noexcept;

private:
	Mso::ActiveField<std::vector<RemoteCallData>> m_callQueue { Queue() };
	Mso::ActiveField<Mso::TCntPtr<IRekaContext>> m_rekaContext { Queue() };
	Mso::ActiveField<Mso::ErrorCode> m_errorCode { Queue() };
};

}} // namespace Mso::JSHost