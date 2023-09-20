#pragma once

//
// Support for Reka service registration.
//
// Reka service can be registered by defining a global variable of RekaServiceRegistration
// or RekaQueueServiceRegistration types.
//
// The RekaServiceRegistration is used for Reka services that are instantiated and invoked from
// the Reka's default dispatch queue. These Reka services may provide a list of constants by
// implementing IRekaObjectConstants interface. The Reka services with constants will be
// initialized as soon as possible during the initialization sequence.
//
// The RekaQueueServiceRegistration is used to register Reka services that must be
// created, invoked, and destroyed from the provided queue. The queue is provided
// as a property identifier to access an IRekaContext property.
// The property identifier can have one of the five forms:
// - Property name
// - NamedProperty instance
// - IsolatedNamedProperty instance
// - IsolationKey and Property name
// - IsolationKey NamedProperty instance
// It is expected that the queue itself will be provided as a property in IRekaContext
// at the time of Reka instance initialization.
// Reka services registered with RekaQueueServiceRegistration cannot implement the
// IRekaObjectConstants interface because their creation is delayed.
//

#include <compileradapters/functiondecorations.h>
#include <platformadapters/windowsfirst.h>

#include <type_traits>
#include "RekaApi.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

struct RekaServiceRegistrationEntry
{
	LIBLET_PUBLICAPI RekaServiceRegistrationEntry(const char* serviceName, bool hasConstants) noexcept;
	virtual Mso::TCntPtr<IRekaService> GetRekaService(IRekaContext& serviceContext) const noexcept = 0;
	const char* ServiceName() const noexcept { return m_serviceName; }
	bool HasConstants() const noexcept { return m_hasConstants; }
	const RekaServiceRegistrationEntry* Next() const noexcept { return m_next; }

private:
	const char* m_serviceName;
	bool m_hasConstants{false};
	const RekaServiceRegistrationEntry* m_next{nullptr};
};

template <class TServiceImpl>
struct IsRekaServiceWithConstant
{
	static const bool value = std::is_base_of<IRekaObjectConstants, TServiceImpl>::value || std::is_base_of<IRekaConstants, TServiceImpl>::value;
};

template <class TServiceImpl, class TService = typename TServiceImpl::RekaServiceType>
struct RekaServiceRegistration : RekaServiceRegistrationEntry
{
	RekaServiceRegistration() noexcept
		: RekaServiceRegistrationEntry{TService::Name, IsRekaServiceWithConstant<TServiceImpl>::value}
	{
	}

	Mso::TCntPtr<IRekaService> GetRekaService(IRekaContext& serviceContext) const noexcept override
	{
		return serviceContext.Properties().GetOrCreate<IRekaService>(&serviceContext, TService::Name, [&serviceContext]() noexcept {
			return Mso::Make<TServiceImpl, IRekaService>(serviceContext);
		});
	}
};

template <class TServiceImpl, class TService = typename TServiceImpl::RekaServiceType>
struct RekaQueueServiceRegistration : RekaServiceRegistrationEntry
{
	static_assert(!IsRekaServiceWithConstant<TServiceImpl>::value, "Constants can only be accessed from the default native queue.");

	RekaQueueServiceRegistration(const char* queuePropertyName) noexcept
		: RekaServiceRegistrationEntry{TService::Name, false}
		, m_queuePropertyName{queuePropertyName}
	{
	}

	RekaQueueServiceRegistration(const NamedProperty<IDispatchQueue> queueProperty) noexcept
		: RekaServiceRegistrationEntry{TService::Name, false}
		, m_queuePropertyName{queueProperty.Name}
	{
	}

	RekaQueueServiceRegistration(const IsolatedNamedProperty<Mso::DispatchQueue> queueProperty) noexcept
		: RekaServiceRegistrationEntry{TService::Name, false}
		, m_isolationKey{queueProperty.IsolationKey}
		, m_queuePropertyName{queueProperty.Name}
	{
	}

	RekaQueueServiceRegistration(const void* isolationKey, const char* queuePropertyName) noexcept
		: RekaServiceRegistrationEntry{TService::Name, false}
		, m_isolationKey{isolationKey}
		, m_queuePropertyName{queuePropertyName}
	{
	}

	RekaQueueServiceRegistration(const void* isolationKey, const NamedProperty<Mso::DispatchQueue> queueProperty) noexcept
		: RekaServiceRegistrationEntry{TService::Name, false}
		, m_isolationKey{isolationKey}
		, m_queuePropertyName{queueProperty.Name}
	{
	}

	Mso::TCntPtr<IRekaService> GetRekaService(IRekaContext& serviceContext) const noexcept override
	{
		return serviceContext.Properties().GetOrCreate<IRekaService>(&serviceContext, TService::Name, [this, &serviceContext]() noexcept {
			auto queue = serviceContext.Properties().Get<Mso::DispatchQueue>(m_isolationKey, m_queuePropertyName);
			return MakeRekaQueueService(std::move(queue),
				[serviceContext = Mso::TCntPtr<IRekaContext>{&serviceContext}](/*out*/Mso::TCntPtr<IRekaService> & rekaService) noexcept {
					rekaService = Mso::Make<TServiceImpl, IRekaService>(*serviceContext);
				});
		});
	}

private:
	const void* m_isolationKey{nullptr};
	const char* m_queuePropertyName{nullptr};
};

// For backward compatibility
template <class TServiceImpl, class TService = typename TServiceImpl::DataServiceType>
using DataServiceRegistration = RekaServiceRegistration<TServiceImpl, TService>;

}} // namespace Mso::JSHost

#pragma pack(pop)
