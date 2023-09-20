#pragma once

#include <compileradapters/functiondecorations.h>
#include <platformadapters/windowsfirst.h>

#include <ErrorCode/ErrorCode.h>
#include <ErrorCode/ErrorProvider.h>

namespace Mso { namespace JSHost {

struct RekaError
{
	RekaError(const char* serializedError) noexcept
		: m_serializedError{Mso::MakeErrorString(serializedError)} // Copy string
	{
	}

	const char* GetSerializedError() const noexcept
	{
		return m_serializedError->Data();
	}

private:
	Mso::TCntPtr<Mso::IErrorString> m_serializedError;
};

MSO_CLASS_GUID(RekaErrorProviderGuid, "ae29feb0-00b5-45a4-a895-3803275c84bc")
LIBLET_PUBLICAPI const Mso::ErrorProvider<RekaError, RekaErrorProviderGuid>& RekaErrorProvider() noexcept;

}} // namespace Mso::JSHost

namespace Mso {

// Template specialization for ErrorProvider must be defined inside of Mso namespace
template <>
Mso::TCntPtr<Mso::IErrorString> ErrorProvider<Mso::JSHost::RekaError, Mso::JSHost::RekaErrorProviderGuid>::ToString(const Mso::ErrorCode& errorCode) const noexcept;

} // namespace Mso