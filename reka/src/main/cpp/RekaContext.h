#pragma once

//
// Base class for RekaContext.
// Platform specific contexts extend this base cross-platform class
//

#include <reka/RekaApi.h>
#include <object/UnknownObject.h>

namespace Mso { namespace JSHost {

namespace Names {

constexpr const char* CreateService = "createService";
constexpr const char* Init = "init";
constexpr const char* OnFailure = "onFailure";
constexpr const char* OnSuccess = "onSuccess";
constexpr const char* RekaBridge = "$RekaBridge";

} // namespace Names

struct RekaInstance;

struct RekaContext : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IRekaContext>
{
public:
	void ExecutePromiseIdOnSuccess(int32_t promiseId, const char* serializedValue) noexcept;
	void ExecutePromiseIdOnFailure(int32_t promiseId, const char* serializedError) noexcept;

public: // IRekaContext interface
	void CallJavaScript(const char* serviceName, const char* methodName, const char* serializedValue,
		Mso::TCntPtr<IRekaResult>&& result) const noexcept override;

	Mso::TCntPtr<IRekaCallContext> PrepareCall(const char* serviceName, const char* methodName) const noexcept override;

	Mso::Async::IDispatchQueue& GetDispatchQueue() const noexcept override;

	const PropertyBag& Properties() const noexcept override;

private:
	friend MakePolicy;
	RekaContext(Mso::WeakPtr<RekaInstance>&& rekaInstanceWeak, const PropertyBag& properties) noexcept;
	~RekaContext() noexcept override;

private:
	Mso::WeakPtr<RekaInstance> m_rekaInstanceWeak;
	PropertyBag m_properties;
};

void SetRekaServiceProvider(const IRekaContext& context, IRekaServiceProvider& serviceProvider) noexcept;

// For backward compatibility
inline void SetDataServiceProvider(const IRekaContext& context, IRekaServiceProvider& serviceProvider) noexcept
{
	SetRekaServiceProvider(context, serviceProvider);
}

}} // namespace Mso::JSHost
