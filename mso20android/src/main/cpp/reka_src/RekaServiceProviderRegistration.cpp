#include "precomp.h"
#include "RekaServiceProviderRegistration.h"
#include <reka/RekaServiceRegistration.h>
// #include <core/memoryapi.h>

namespace Mso { namespace JSHost {

std::atomic<RekaServiceProviderRegistry*> RekaServiceProviderRegistry::s_instance{nullptr};

RekaServiceProviderRegistry& RekaServiceProviderRegistry::Instance() noexcept
{
	auto instance = s_instance.load(std::memory_order_relaxed);
	if (!instance)
	{
		auto newInstance = std::make_unique<RekaServiceProviderRegistry>();
		if (s_instance.compare_exchange_strong(instance, newInstance.get(), std::memory_order_release, std::memory_order_relaxed))
		{
			// The instance now belongs to RekaServiceProviderRegistry class variable s_instance
			instance = newInstance.release();

			//Temporary fix to unblock memory leak tests while implementing ILiblet uninit memory release solution.
			// MsoSetLazyLeakDetection(instance);
		}
	}

	return *instance;
}

Mso::TCntPtr<IRekaServiceProvider> RekaServiceProviderRegistry::GetOrMakeProvider(
	const char* providerName, IRekaContext& context) noexcept
{
	return context.Properties().GetOrCreate<IRekaServiceProvider>(this, providerName, [providerName, &context, this]() noexcept {
		auto provider = GetProvider(providerName);
		if (provider)
		{
			return provider;
		}

		auto providerFactory = GetProviderFactory(providerName);
		if (providerFactory)
		{
			return providerFactory(context);
		}

		return Mso::TCntPtr<IRekaServiceProvider>();
	});
}

Mso::TCntPtr<IRekaServiceProvider> RekaServiceProviderRegistry::GetProvider(const char* providerName) noexcept
{
	std::string providerNameString{providerName};
	if (providerNameString == DefaultRekaServiceProviderName())
	{
		return MakeRegisteredRekaServiceProvider();
	}

	std::lock_guard<std::mutex> lock{m_providersLock};

	auto it = m_providers.find(providerName);
	if (it != std::end(m_providers))
	{
		return it->second;
	}

	return nullptr;
}

RekaServiceProviderFactory RekaServiceProviderRegistry::GetProviderFactory(const char* providerName) noexcept
{
	std::lock_guard<std::mutex> lock{m_providersLock};

	auto it = m_providerFactories.find(providerName);
	if (it != std::end(m_providerFactories))
	{
		return it->second;
	}

	return nullptr;
}

void RekaServiceProviderRegistry::AddProvider(const char* providerName, Mso::TCntPtr<IRekaServiceProvider>&& provider) noexcept
{
	std::string providerNameString{providerName};
	VerifyElseCrashSzTag(providerNameString != DefaultRekaServiceProviderName(),
		"You must not use a reserved name to register a RekaService provider.", 0x0281d351 /* tag_c63nr */);

	std::lock_guard<std::mutex> lock{m_providersLock};

	// All RekaService provider names must be unique
	auto it = m_providers.find(providerNameString);
	VerifyElseCrashSzTag(it == std::end(m_providers), "You must use a globally unique name for each RekaService provider.", 0x0281d352 /* tag_c63ns */);

	m_providers.emplace(std::move(providerNameString), std::move(provider));
}

void RekaServiceProviderRegistry::AddProviderFactory(const char* providerName, RekaServiceProviderFactory&& providerFactory) noexcept
{
	std::string providerNameString{providerName};
	VerifyElseCrashSzTag(providerNameString != DefaultRekaServiceProviderName(), "You must not use a reserved name to register a RekaService provider.", 0x0281d353 /* tag_c63nt */);

	std::lock_guard<std::mutex> lock{m_providersLock};

	// All RekaService provider names must be unique
	auto it = m_providerFactories.find(providerNameString);
	VerifyElseCrashSzTag(it == std::end(m_providerFactories), "You must use a globally unique name for each RekaService provider.", 0x0281d354 /* tag_c63nu */);

	m_providerFactories.emplace(std::move(providerNameString), std::move(providerFactory));
}

void RekaServiceProviderRegistry::RemoveProvider(const char* providerName) noexcept
{
	using std::swap;
	std::string providerNameString{providerName};
	VerifyElseCrashSzTag(providerNameString != DefaultRekaServiceProviderName(), "You must not use a reserved name to unregister a RekaService provider.", 0x0281d355 /* tag_c63nv */);

	// Make sure that objects are deleted outside of lock
	Mso::TCntPtr<IRekaServiceProvider> providerToDelete;

	{
		std::lock_guard<std::mutex> lock{m_providersLock};

		auto it = m_providers.find(providerNameString);
		if (it != std::end(m_providers))
		{
			swap(providerToDelete, it->second);
			m_providers.erase(it);
		}
	}
}

void RekaServiceProviderRegistry::RemoveProviderFactory(const char* providerName) noexcept
{
	using std::swap;
	std::string providerNameString{providerName};
	VerifyElseCrashSzTag(providerNameString != DefaultRekaServiceProviderName(), "You must not use a reserved name to unregister a RekaService provider factory.", 0x0281d356 /* tag_c63nw */);

	// Make sure that objects are deleted outside of lock
	RekaServiceProviderFactory providerFactoryToDelete;

	{
		std::lock_guard<std::mutex> lock{m_providersLock};

		auto it = m_providerFactories.find(providerNameString);
		if (it != std::end(m_providerFactories))
		{
			swap(providerFactoryToDelete, it->second);
			m_providerFactories.erase(it);
		}
	}
}

LIBLET_PUBLICAPI const char* DefaultRekaServiceProviderName() noexcept
{
	return "DefaultProvider";
}

LIBLET_PUBLICAPI void RegisterRekaServiceProvider(const char* providerName, Mso::TCntPtr<IRekaServiceProvider>&& provider) noexcept
{
	RekaServiceProviderRegistry::Instance().AddProvider(providerName, std::move(provider));
}

LIBLET_PUBLICAPI void RegisterRekaServiceProviderFactory(const char* providerName, RekaServiceProviderFactory&& providerFactory) noexcept
{
	RekaServiceProviderRegistry::Instance().AddProviderFactory(providerName, std::move(providerFactory));
}

LIBLET_PUBLICAPI void UnregisterRekaServiceProvider(const char* providerName) noexcept
{
	RekaServiceProviderRegistry::Instance().RemoveProvider(providerName);
}

LIBLET_PUBLICAPI void UnregisterRekaServiceProviderFactory(const char* providerName) noexcept
{
	RekaServiceProviderRegistry::Instance().RemoveProviderFactory(providerName);
}

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> GetOrMakeRekaServiceProvider(
	const char* providerName, IRekaContext& context) noexcept
{
	const char* providerNameOrDefault = (providerName != nullptr && strlen(providerName) > 0)
											? providerName
											: Mso::JSHost::DefaultRekaServiceProviderName();
	return RekaServiceProviderRegistry::Instance().GetOrMakeProvider(providerNameOrDefault, context);
}

}} // namespace Mso::JSHost
