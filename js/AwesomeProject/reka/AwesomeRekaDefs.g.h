#pragma once
#ifndef REACTHOSTAPP_AWESOMEREKADEFS_H
#define REACTHOSTAPP_AWESOMEREKADEFS_H
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by the Reka compiler.
//     Changes to this file may cause incorrect behavior and will be lost if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

#include <reka/Reka.h>

namespace reacthostapp {

//================================================================================
// Services
//================================================================================

struct DECLSPEC_NOVTABLE NativeService : IUnknown
{
	constexpr static const char* Name = "reacthostapp::NativeService";
	virtual void tryCall(std::string&& input1, int32_t&& input2) noexcept = 0;
};
} // namespace reacthostapp

namespace Mso { namespace JSHost {

//================================================================================
// Service implementation
//================================================================================

template <>
struct DataService<::reacthostapp::NativeService> : RekaObject<::reacthostapp::NativeService>
{
	using DataServiceBase = DataService;

	DataService(IRekaContext& serviceContext) noexcept
		: RekaObjectBase(serviceContext)
	{
	}

	void InvokeWithReader(const char* methodName, Mso::JSHost::IRekaReader& argumentsReader, Mso::TCntPtr<IRekaResult>&& result) noexcept override;
};

}} // namespace Mso::JSHost

#endif