#include "precomp.h"
#include "RekaQueueService.h"

#include <string>

namespace Mso {

template <class T>
WeakPtr<T> MakeWeakPtr(T* ptr) noexcept
{
	return Mso::WeakPtr<T>(ptr);
}

} // namespace Mso

namespace Mso { namespace JSHost {

struct RekaServiceArgs
{
	std::string MethodName;
	std::string ArgsJson;
	Mso::TCntPtr<IRekaResult> Result;
};

//=============================================================================================
// RekaServiceQueueWrapper implementation
//=============================================================================================

RekaQueueService::RekaQueueService() noexcept
{
	// To keep it in the .cpp file.
}

RekaQueueService::~RekaQueueService() noexcept
{
	// To keep it in the .cpp file.
}

void RekaQueueService::InitializeThis(
	Mso::DispatchQueue queue,
	Mso::Functor<void(Mso::TCntPtr<IRekaService>&)>&& rekaServiceMaker) noexcept
{
	m_queue = std::move(queue);
	m_queue.Post([
		thisPtr = Mso::TCntPtr<RekaQueueService>{this},
		rekaServiceMaker = std::move(rekaServiceMaker)
	]() noexcept {
		rekaServiceMaker(thisPtr->m_service);
	});
}

void RekaQueueService::DestroyThis() noexcept
{
	// Make sure that wrapped service is deleted in the queue
	m_queue.Post([this]() noexcept {
		RefCountPolicy::Delete(this);
	});
}

void RekaQueueService::Invoke(const char* methodName, const char* serializedValue,
	Mso::TCntPtr<IRekaResult>&& result) noexcept
{
	m_queue.Post([
		weakThis = Mso::MakeWeakPtr(this),
		args = RekaServiceArgs{methodName, serializedValue, std::move(result)}
	]() mutable noexcept {
		if (auto strongThis = weakThis.GetStrongPtr())
		{
			strongThis->m_service->Invoke(args.MethodName.c_str(), args.ArgsJson.c_str(), std::move(args.Result));
		}
	});
}

void RekaQueueService::InvokeWithReader(const char* /*methodName*/, IRekaReader& /*argumentsReader*/, Mso::TCntPtr<IRekaResult>&& /*result*/) noexcept
{
	// This function will not be called now
	// The above function will be replaced by this one once all users upgrade their new reka-compiler
	VerifyElseCrashTag(false, 0x0281d34e /* tag_c63no */);
}

LIBLET_PUBLICAPI Mso::TCntPtr<IRekaService> MakeRekaQueueService(
	Mso::DispatchQueue queue,
	Mso::Functor<void(Mso::TCntPtr<IRekaService>&)>&& rekaServiceMaker) noexcept
{
	VerifyElseCrashSzTag(queue, "DispatchQueue to run service on is null", 0x0281d34f /* tag_c63np */);
	VerifyElseCrashSzTag(rekaServiceMaker, "Reka service maker is missing", 0x0281d350 /* tag_c63nq */);
	return Mso::Make<RekaQueueService, IRekaService>(std::move(queue), std::move(rekaServiceMaker));
}

}} // namespace Mso::JSHost
