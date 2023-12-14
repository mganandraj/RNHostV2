#include "precomp.h"
#include "RekaCallContext.h"
#include "RekaInstance.h"

namespace Mso { namespace JSHost {

RekaJsonCallContext::RekaJsonCallContext(
	const char* serviceName,
	const char* methodName,
	InvokeRekaBridgeCallback&& invokeRekaBridge) noexcept
	: m_serviceName { serviceName }
	, m_methodName { methodName }
	, m_invokeRekaBridge { std::move(invokeRekaBridge) }
{
}

IRekaWriter& RekaJsonCallContext::GetWriter() noexcept
{
	return m_writer.GetRekaWriter();
}

void RekaJsonCallContext::AsyncCall(Mso::TCntPtr<IRekaResult>&& result) noexcept
{
	m_invokeRekaBridge(
		m_serviceName.c_str(),
		m_methodName.c_str(),
		m_writer.ToString(),
		std::move(result));
}

}} // namespace Mso::JSHost
