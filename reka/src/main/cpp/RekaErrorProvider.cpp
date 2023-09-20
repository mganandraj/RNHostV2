#include "precomp.h"
#include <Reka/RekaErrorProvider.h>

namespace Mso { namespace JSHost {

const Mso::ErrorProvider<RekaError, RekaErrorProviderGuid> s_rekaErrorProvider{};

const Mso::ErrorProvider<RekaError, RekaErrorProviderGuid>& RekaErrorProvider() noexcept
{
	return s_rekaErrorProvider;
}

}} // namespace Mso::JSHost

namespace Mso {
// Template specialization for ErrorProvider must be defined inside of Mso namespace
template <>
Mso::TCntPtr<Mso::IErrorString> ErrorProvider<Mso::JSHost::RekaError, Mso::JSHost::RekaErrorProviderGuid>::ToString(const Mso::ErrorCode& errorCode) const noexcept
{
	return Mso::MakeErrorString(GetErrorInfo(errorCode).GetSerializedError());
}

} // namespace Mso