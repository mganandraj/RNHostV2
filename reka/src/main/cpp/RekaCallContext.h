#pragma once

#include <reka/RekaApi.h>
#include <reka/JsonWriter.h>

namespace Mso { namespace JSHost {

using InvokeRekaBridgeCallback = Mso::Functor<void(
	const char* /*serviceName*/,
	const char* /*methodName*/,
	std::string&& /*jsonArgs*/,
	Mso::TCntPtr<IRekaResult>&& /*result*/)>;

struct RekaJsonCallContext : Mso::UnknownObject<IRekaCallContext>
{
public:
	RekaJsonCallContext(
		const char* serviceName,
		const char* methodName,
		InvokeRekaBridgeCallback&& invokeRekaBridge) noexcept;

	IRekaWriter& GetWriter() noexcept override;
	void AsyncCall(Mso::TCntPtr<IRekaResult>&& result = nullptr) noexcept override;

private:
	std::string m_serviceName;
	std::string m_methodName;
	InvokeRekaBridgeCallback m_invokeRekaBridge;
	JsonWriter m_writer;
};

}} // namespace Mso::JSHost