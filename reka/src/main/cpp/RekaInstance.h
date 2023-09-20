#pragma once
#include <object/UnknownObject.h>
#include <reka/RekaApi.h>
#include <mutex>
#include <map>
#include <string>

namespace Mso { namespace JSHost {

struct RekaContext;

struct MovableRekaBridgeInvokeArgs
{
	MovableRekaBridgeInvokeArgs(const RekaBridgeInvokeArgs& args) noexcept;
	MovableRekaBridgeInvokeArgs(const MovableRekaBridgeInvokeArgs&) = default;
	MovableRekaBridgeInvokeArgs(MovableRekaBridgeInvokeArgs&&) = default;

	RekaBridgeInvokeArgs AsRekaBridgeInvokeArgs() const noexcept;

	std::string ServiceName;
	std::string MethodName;
	std::string SerializedValue;
	int32_t PromiseId;
};

struct RekaInstance : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef,
								Mso::QueryCastChain<IRekaInstance, IRekaBridge>>
{
public:
	Mso::DispatchQueue GetDispatchQueue() noexcept;
	int32_t GetJsPromiseId(Mso::TCntPtr<IRekaResult>&& result) noexcept;

	const PropertyBag& Properties() const noexcept;

	const Mso::TCntPtr<RekaContext>& GetContext() const noexcept;

	void InvokeJsExecutor(const RekaBridgeInvokeArgs& args) const noexcept;

public: // IRekaBridge interface
	void Invoke(const RekaBridgeInvokeArgs& args) noexcept override;

public: // IRekaInstance interface
	void Destroy() noexcept override;
	void EnumerateConstants(const EnumerateConstantsCallback& callback) noexcept override;

private:
	friend MakePolicy;
	RekaInstance(RekaBridgeOptions&& options) noexcept;
	~RekaInstance() noexcept override;

	void HandleRekaBridge(const MovableRekaBridgeInvokeArgs& args) noexcept;
	Mso::TCntPtr<IRekaService> GetRekaService(const char* serviceName) noexcept;
	Mso::TCntPtr<IRekaResult> MakeNativeRekaResult(int32_t promiseId) noexcept;
	Mso::TCntPtr<IRekaResult> TakeJsRekaResult(int32_t promiseId) noexcept;

private:
	mutable std::mutex m_lock;
	Mso::DispatchQueue m_dispatchQueue;
	std::map<int32_t /*promiseId*/, Mso::TCntPtr<IRekaResult>> m_jsResults;
	int32_t m_nextJsPromiseId{0};
	Mso::TCntPtr<RekaContext> m_serviceContext;
	Mso::TCntPtr<IRekaServiceProvider> m_serviceProvider;
	Mso::TCntPtr<IRekaBridge> m_jsExecutor;
	OnInitializedCallback m_onInitialized;
	OnDestroyedCallback m_onDestroyed;
	PropertyBag m_properties;
};

// Owns a single strong reference to RekaInstance instance
struct RekaInstanceOwner : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::QueryCastChain<IRekaInstance, IRekaBridge>>
{
public: // IRekaBridge interface
	void Invoke(const RekaBridgeInvokeArgs& args) noexcept override;

public: // IRekaInstance interface
	void Destroy() noexcept override;
	void EnumerateConstants(const EnumerateConstantsCallback& callback) noexcept override;

public:
	Mso::TCntPtr<Mso::JSHost::RekaContext> GetContext() noexcept;

private:
	friend MakePolicy;
	RekaInstanceOwner(RekaBridgeOptions&& options) noexcept;
	~RekaInstanceOwner() noexcept override;

	Mso::TCntPtr<RekaInstance> GetRekaInstance() noexcept;
	static void DestroyInternal(Mso::TCntPtr<RekaInstance>&& rekaInstance) noexcept;

private:
	mutable std::mutex m_lock;

	// The only strong reference which keeps RekaInstance object alive
	// Will be set to nullptr during Destroy()
	// Since it can be set to nullptr from any thread,
	// it must not be used directly, only through
	// the thread-safe helper method GetRekaInstance()
	Mso::TCntPtr<RekaInstance> m_rekaInstance;
};

struct NativeRekaResult : Mso::UnknownObject<IRekaResult>
{
public: // IRekaResult interface
	void OnSuccess(const char* serializedValue) noexcept override;
	void OnFailure(const char* serializedError) noexcept override;

private:
	friend MakePolicy;
	NativeRekaResult(Mso::TCntPtr<RekaContext>&& context, int32_t promiseId) noexcept;
	~NativeRekaResult() noexcept override;

private:
	Mso::TCntPtr<RekaContext> m_context;
	int32_t m_promiseId;
};

}} // namespace Mso::JSHost
