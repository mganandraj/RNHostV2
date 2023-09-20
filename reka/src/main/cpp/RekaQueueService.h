#pragma once

//
// Specialized implementations of IRekaService interface.
//

#include <object/UnknownObject.h>
#include <reka/RekaApi.h>

namespace Mso { namespace JSHost {

// To ensure that RekaQueueService is deleted in the associated queue
struct RekaQueueServiceDeleter
{
	template <class TObject>
	static void Delete(TObject* obj) noexcept;
};

struct RekaQueueService : Mso::UnknownObject<Mso::WeakRefCountPolicy<RekaQueueServiceDeleter>, IRekaService>
{
	using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;

public: // IRekaService interface
	void Invoke(const char* methodName, const char* serializedValue, Mso::TCntPtr<IRekaResult>&& result) noexcept override;
	void InvokeWithReader(const char* methodName, IRekaReader& argumentsReader, Mso::TCntPtr<IRekaResult>&& result) noexcept override;

private:
	friend MakePolicy;				// To access constructor and InitializeThis()
	friend RefCountPolicy;			// To access destructor
	friend RekaQueueServiceDeleter; // To access DestroyThis()

	RekaQueueService() noexcept;
	~RekaQueueService() noexcept override;
	void InitializeThis(
		Mso::TCntPtr<Mso::Async::IDispatchQueue>&& queue,
		Mso::Functor<void(Mso::TCntPtr<IRekaService>&)>&& rekaServiceMaker) noexcept;
	void DestroyThis() noexcept;

private:
	Mso::TCntPtr<IRekaService> m_service;
	Mso::TCntPtr<Mso::Async::IDispatchQueue> m_queue;
};


template <class TObject>
void RekaQueueServiceDeleter::Delete(TObject* obj) noexcept
{
	static_cast<RekaQueueService*>(obj)->DestroyThis();
}

}} // namespace Mso::JSHost
