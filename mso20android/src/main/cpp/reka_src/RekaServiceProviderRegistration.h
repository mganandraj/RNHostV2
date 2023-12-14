#pragma once

#include <reka/RekaApi.h>
#include <atomic>
#include <map>
#include <mutex>
#include <string>

namespace Mso { namespace JSHost {

// A global registry of data service providers identified by name.
// It is accessible through a singleton Instance() method.
struct RekaServiceProviderRegistry
{
	static RekaServiceProviderRegistry& Instance() noexcept;

	Mso::TCntPtr<IRekaServiceProvider> GetOrMakeProvider(
		const char* providerName, IRekaContext& context) noexcept;
	Mso::TCntPtr<IRekaServiceProvider> GetProvider(const char* providerName) noexcept;
	RekaServiceProviderFactory GetProviderFactory(const char* providerName) noexcept;
	void AddProvider(const char* providerName, Mso::TCntPtr<IRekaServiceProvider>&& provider) noexcept;
	void AddProviderFactory(const char* providerName, RekaServiceProviderFactory&& providerFactory) noexcept;
	void RemoveProvider(const char* providerName) noexcept;
	void RemoveProviderFactory(const char* providerName) noexcept;

private:
	static std::atomic<RekaServiceProviderRegistry*> s_instance;

	std::mutex m_providersLock;
	std::map<std::string, Mso::TCntPtr<IRekaServiceProvider>> m_providers;
	std::map<std::string, RekaServiceProviderFactory> m_providerFactories;
};

}} // namespace Mso::JSHost
