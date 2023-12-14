#include "precomp.h"
#include <object/UnknownObject.h>
#include <reka/RekaServiceRegistration.h>
#include <map>
#include <reka/JsonWriter.h>

namespace Mso { namespace JSHost {

static const RekaServiceRegistrationEntry* g_serviceRegistration{nullptr};

namespace TestOnly {

// To be used only by the internal unit tests
void ResetRegistration() noexcept
{
	g_serviceRegistration = nullptr;
}

} // namespace TestOnly

LIBLET_PUBLICAPI RekaServiceRegistrationEntry::RekaServiceRegistrationEntry(const char* serviceName, bool hasConstants) noexcept
	: m_serviceName{serviceName}
	, m_hasConstants{hasConstants}
	, m_next{g_serviceRegistration}
{
	g_serviceRegistration = this;
}

// An IRekaServiceProvider that provides registered data services
struct RegisteredRekaServiceProvider : Mso::UnknownObject<IRekaConstantsProvider, IRekaServiceProvider>
{
private:
	friend MakePolicy;

	struct ConstantWriter : Mso::UnknownObject<IRekaConstantWriter>
	{
		ConstantWriter(const EnumerateConstantsCallback& callback, const char* serviceName, const char* propertyName)
			: m_callback(callback)
			, m_serviceName(serviceName)
			, m_propertyName(propertyName)
		{
		}
		
		IRekaWriter& GetWriter() noexcept override
		{
			return m_writer.GetRekaWriter();
		}

		void Submit() noexcept override
		{
			VerifyElseCrashTag(!m_submitted, 0x02863511 /* tag_c79ur */);
			m_submitted = true;
			m_callback(m_serviceName.c_str(), m_propertyName.c_str(), m_writer.ToString().c_str());
		}

	private:
		const EnumerateConstantsCallback& m_callback;
		std::string m_serviceName;
		std::string m_propertyName;
		JsonWriter m_writer;
		bool m_submitted = false;
	};

	RegisteredRekaServiceProvider() noexcept
	{
		for (auto reg = g_serviceRegistration; reg != nullptr; reg = reg->Next())
		{
			m_serviceRegistrations.emplace(ServiceName{reg->ServiceName()}, reg);
		}
	}

	Mso::TCntPtr<IRekaService> GetRekaService(const char* serviceName, IRekaContext& serviceContext) noexcept override
	{
		auto it = m_serviceRegistrations.find(ServiceName{serviceName});
		if (it == m_serviceRegistrations.end())
		{
			return nullptr;
		}

		return it->second->GetRekaService(serviceContext);
	}

	void EnumerateConstants(const EnumerateConstantsCallback& callback, IRekaContext& serviceContext) noexcept override
	{
		for (auto reg = g_serviceRegistration; reg != nullptr; reg = reg->Next())
		{
			if (reg->HasConstants())
			{
				if (auto dataService = GetRekaService(reg->ServiceName(), serviceContext))
				{
					if (auto constants = query_cast<IRekaConstants*>(dataService.Get()))
					{
						constants->EnumerateConstants([&callback, reg](const char* propertyName) noexcept {
							return Mso::Make<ConstantWriter, IRekaConstantWriter>(callback, reg->ServiceName(), propertyName);
						});
					}
					else if (auto objectConstants = query_cast<IRekaObjectConstants*>(dataService.Get()))
					{
						objectConstants->EnumerateConstants([&callback, reg](const char* propertyName, const char* serializedJson) noexcept {
							callback(reg->ServiceName(), propertyName, serializedJson);
						});
					}
				}
			}
		}
	}

	struct ServiceName
	{
		const char* Name;
	};

	struct ServiceNameCompare
	{
		bool operator()(const ServiceName& left, const ServiceName& right) const noexcept
		{
			return strcmp(left.Name, right.Name) < 0;
		}
	};

private:
	std::map<ServiceName, const RekaServiceRegistrationEntry*, ServiceNameCompare> m_serviceRegistrations;
};

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaServiceProvider> MakeRegisteredRekaServiceProvider() noexcept
{
	return Mso::Make<RegisteredRekaServiceProvider, IRekaServiceProvider>();
}

}} // namespace Mso::JSHost
