#pragma once

#include "InstanceFactory.h"
#include "MsoUtils.h"
#include "activeObject/activeObject.h"
#include "ReactNativeHost/React.h"
#include <mutex>
#include <unordered_map>
#include <object/refCountedObject.h>

#include "cppMacros.h"

namespace Mso {
namespace React {

//! Private interface for ReactHostRegistry to shutdown and close ReactHost during shutdown
MSO_STRUCT_GUID(IClosableReactHost, "702b469b-3109-46f7-9e63-0969d96e6d4b")
struct DECLSPEC_NOVTABLE IClosableReactHost : IUnknown
{
	//! The ReactHostRegistry liblet calls this to force a shutdown of the RN instance
	virtual void Close() noexcept = 0;
//	virtual void GetQueueControllersForLibletUninit(
//		std::vector<Mso::CntPtr<Mso::Async::IThreadDispatchQueueController>>& queueControllers) noexcept = 0;
	virtual ReactOptions Options() const noexcept = 0;
};

class ReactViewHost;
class ReactHost;

//! The async action is a Functor that returns void Future.
using AsyncAction = Mso::Functor<Mso::Future<void>()>;

//! The queue that executes actions in the sequential order.
//! Each action returns Mso::Future<void> to indicate its completion.
//! The next action does not start until the previous action is completed.
struct AsyncActionQueue final
	: Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, AsyncActionQueue>
{
	//! Creates a new AsyncActionQueue that is based on the provided sequential queue.
	AsyncActionQueue(Mso::DispatchQueue queue) noexcept;
	AsyncActionQueue() = delete;
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(AsyncActionQueue);

	//! Posts a new action to the AsyncActionQueue and returns Future indicating when it is completed.
	Mso::Future<void> PostAction(AsyncAction&& action) noexcept;

	//! Posts a new action to the AsyncActionQueue and returns Future indicating when the last one is completed.
	//! For the empty list it returns succeeded Future immediately.
	Mso::Future<void> PostActions(std::initializer_list<AsyncAction> actions) noexcept;

	//! Returns the queue associated with the AsyncActionQueue.
	Mso::DispatchQueue Queue() noexcept;

private:
	struct Entry
	{
		AsyncAction Action;
		Mso::Promise<void> Result;
	};

private:
	//! Invokes action in the m_queue and observes its result.
	void InvokeAction(Entry&& entry) noexcept;

	//! Completes the action, and then starts the next one.
	void CompleteAction(Entry&& entry, Mso::Maybe<void>&& result) noexcept;

private:
	// const Mso::CntPtr<Mso::IDispatchQueue> m_queue;
	const Mso::DispatchQueue m_queue;
	const Mso::ActiveField<std::vector<Entry>> m_actions{m_queue};
	const Mso::ActiveField<bool> m_isInvoking{m_queue};
	const Mso::InvokeElsePostExecutor m_executor{m_queue};
};

//! ReactHost manages lifetime of ReactNative instance.
//! It is associated with a native queue that is used modify its state.
//! - It can reload or unload react instance using Reload and Unload.
//! - ReactInstance() returns current react native instance.
//! - NativeQueue() returns the associate native queue.
//! - RekaContext() returns Reka context proxy that can be used before Reka is initialized.
//! - Options() - returns options used for creating ReactInstance.
//!
//! About the RNH closing sequence:
//! - Every time a new RNH is created it is added to the global registry.
//! - During registration we associate Mso::Future with each RNH
//! - The Mso::Future deletes the RNH from the registry.
//! - When the Liblet::Uninit starts, it turns the registry into the uninited state.
//! - In the uninited state new RNH cannot be registered. They are created as already closed.
//! - All operations in closed RNH are canceled.
//! - All registered RNHs are closed. No new work can be done.
//! - We wait in Liblet::Uninit when all pending Futures to indicate closed RNH state are completed.
class ReactHost final
	: public Mso::ActiveObject<IReactHost, IClosableReactHost>
{
	using Super = ActiveObjectType;

public: // IReactHost
	ReactOptions Options() const noexcept override;
	Mso::CntPtr<IReactInstance> Instance() const noexcept override;
	Mso::JSHost::IRekaContext& RekaContext() noexcept override;
	Mso::DispatchQueue NativeQueue() const noexcept override;
	Mso::Future<void> ReloadInstance() noexcept override;
	Mso::Future<void> ReloadInstanceWithOptions(ReactOptions&& options) noexcept override;
	Mso::Future<void> UnloadInstance() noexcept override;
	Mso::CntPtr<IReactViewHost> MakeViewHost(ReactViewOptions&& options) noexcept override;
	Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept override;

	// IClosableReactHost
	void Close() noexcept override;
//	void GetQueueControllersForLibletUninit(
//		std::vector<Mso::CntPtr<Mso::Async::IThreadDispatchQueueController>>& queueControllers) noexcept override;

public:
	void AttachViewHost(ReactViewHost& viewHost) noexcept;
	void DetachViewHost(ReactViewHost& viewHost) noexcept;

	Mso::CntPtr<AsyncActionQueue> ActionQueue() const noexcept;

	Mso::Future<void> LoadInQueue(ReactOptions&& options) noexcept;
	Mso::Future<void> UnloadInQueue(size_t unloadActionId) noexcept;

	bool IsClosed() const noexcept;

	size_t PendingUnloadActionId() const noexcept;
	bool IsInstanceLoaded() const noexcept;

	template <class TCallback>
	Mso::Future<void> PostInQueue(TCallback&& callback) noexcept;

	bool HasCurrentThreadAccess(Mso::IDispatchQueue& queue) noexcept;

private:
	friend MakePolicy;
	ReactHost(ReactOptions&& options, Mso::Promise<void>&& onInstanceLoaded) noexcept;
	~ReactHost() noexcept override;
	void Initialize() noexcept override;
	void Finalize() noexcept override;

	static Mso::DispatchQueue GetNativeQueue(const ReactOptions& options) noexcept;

	void ForEachViewHost(const Mso::FunctorRef<void(ReactViewHost&)>& action) noexcept;

	AsyncAction MakeLoadInstanceAction(ReactOptions&& options) noexcept;
	AsyncAction MakeUnloadInstanceAction() noexcept;

	void EnsureUIQueue(const ReactOptions& options) noexcept;

private:
	mutable std::mutex m_mutex;
	const Mso::InvokeElsePostExecutor m_executor{Queue()};
	const Mso::ActiveField<Mso::Promise<void>> m_onInstanceLoaded{Queue()};
	const Mso::ActiveReadableField<Mso::CntPtr<AsyncActionQueue>> m_actionQueue{Mso::Make<AsyncActionQueue>(Queue()), Queue(), m_mutex};
	const Mso::CntPtr<Mso::JSHost::IRekaContextProxy> m_rekaContextProxy;
	const Mso::ActiveField<Mso::ErrorCode> m_lastError{Queue()};
	const Mso::ActiveReadableField<ReactOptions> m_options{Queue(), m_mutex};
	const Mso::ActiveField<std::unordered_map<uintptr_t, Mso::WeakPtr<ReactViewHost>>> m_viewHosts{Queue()};
	const Mso::ActiveReadableField<Mso::CntPtr<IReactInstanceInternal>> m_reactInstance{Queue(), m_mutex};
	const Mso::ActiveReadableField<Mso::Promise<void>> m_notifyWhenClosed{nullptr, Queue(), m_mutex};
	const Mso::ActiveField<size_t> m_pendingUnloadActionId{0, Queue()};
	const Mso::ActiveField<size_t> m_nextUnloadActionId{0, Queue()};
	const Mso::ActiveField<bool> m_isInstanceLoaded{false, Queue()};
	const Mso::ActiveReadableField<Mso::CntPtr<Mso::IDispatchQueue>> m_uiQueue{nullptr, Queue(), m_mutex};
};

//! Implements a cross-platform host for a React view
class ReactViewHost final
	: public ActiveObject<IReactViewHost>
{
	using Super = ActiveObjectType;

public: // IReactViewHost
	ReactViewOptions Options() const noexcept override;
	IReactHost& ReactHost() const noexcept override;
	Mso::Future<void> ReloadViewInstance() noexcept override;
	Mso::Future<void> ReloadViewInstanceWithOptions(ReactViewOptions&& options) noexcept override;
	Mso::Future<void> UnloadViewInstance() noexcept override;
	Mso::Future<void> AttachViewInstance(IReactViewInstance& viewInstance) noexcept override;
	Mso::Future<void> DetachViewInstance() noexcept override;

public:
	Mso::Future<void> LoadViewInstanceInQueue() noexcept;
	Mso::Future<void> LoadViewInstanceInQueue(ReactViewOptions&& options) noexcept;
	Mso::Future<void> UnloadViewInstanceInQueue(size_t unloadActionId) noexcept;

private:
	friend MakePolicy;
	ReactViewHost(Mso::React::ReactHost& reactHost, ReactViewOptions&& options) noexcept;
	~ReactViewHost() noexcept override;

	void SetOptions(ReactViewOptions&& options) noexcept;

	AsyncAction MakeLoadViewInstanceAction() noexcept;
	AsyncAction MakeLoadViewInstanceAction(ReactViewOptions&& options) noexcept;
	AsyncAction MakeUnloadViewInstanceAction() noexcept;

	template <class TCallback>
	Mso::Future<void> PostInQueue(TCallback&& callback) noexcept;

private:
	mutable std::mutex m_mutex;
	const Mso::InvokeElsePostExecutor m_executor{Queue()};
	const Mso::ActiveField<Mso::CntPtr<AsyncActionQueue>> m_actionQueue{Queue()};
	const Mso::CntPtr<Mso::React::ReactHost> m_reactHost;
	const Mso::ActiveReadableField<ReactViewOptions> m_options{Queue(), m_mutex};
	const Mso::ActiveField<Mso::CntPtr<IReactViewInstance>> m_viewInstance{Queue()};
	const Mso::ActiveField<size_t> m_pendingUnloadActionId{0, Queue()};
	const Mso::ActiveField<size_t> m_nextUnloadActionId{0, Queue()};
	const Mso::ActiveField<bool> m_isViewInstanceLoaded{false, Queue()};
};

//! ReactHostRegistry helps with closing of all ReactHosts on Liblet::Uninit.
class ReactHostRegistry final
	: public Mso::RefCountedObjectNoVTable<ReactHostRegistry>
{
public:
	ReactHostRegistry() = default;
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ReactHostRegistry);

	~ReactHostRegistry() noexcept;

	// Adds ReactHost to the global registry and returns promise to be set when closed.
	// It returns an empty Mso::Promise in case if RNH cannot be registered.
	static Mso::Promise<void> Register(IClosableReactHost& reactHost) noexcept;

	static void OnLibletInit() noexcept;
	static void OnLibletUninit() noexcept;

private:
	struct Entry
	{
		Mso::WeakPtr<IClosableReactHost> WeakReactHost;
		Mso::Future<void> WhenReactHostClosed;
	};

private:
	// Removes the ReactHost from the global registry
	void Unregister(uintptr_t reactHostKey) noexcept;
//	static void WaitForReactHostClosed(
//		const Mso::Future<void>& onClosed,
//		const std::vector<Mso::CntPtr<Mso::Async::IThreadDispatchQueueController>>& queueControllers) noexcept;

private:
	static std::recursive_mutex s_mutex; // protects access to s_registry and m_entries.
	static Mso::CntPtr<ReactHostRegistry> s_registry;

private:
	// m_entries is an instance field to ensure that we have correct behavior
	// in case when tests run multiple Liblet Init/Uninit.
	// The instance field ensures that all Unregister calls use the correct set on entries.
	std::unordered_map<uintptr_t, Entry> m_entries;
};

//=============================================================================================
// ReactHost inline implementation
//=============================================================================================

template <class TCallback>
Mso::Future<void> ReactViewHost::PostInQueue(TCallback&& callback) noexcept
{
	using Callback = std::decay_t<TCallback>;
	return m_reactHost->PostInQueue(
		[weakThis = Mso::WeakPtr{this}, callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept {
			if (auto strongThis = weakThis.GetStrongPtr())
			{
				return callback();
			}

			return Mso::MakeCanceledFuture();
		});
}

template <class TCallback>
Mso::Future<void> ReactHost::PostInQueue(TCallback&& callback) noexcept
{
	using Callback = std::decay_t<TCallback>;
	return Mso::PostFuture(m_executor,
		[weakThis = Mso::MakeWeakPtr(this),
			callback = Callback{std::forward<TCallback>(callback)}]() mutable noexcept {
			if (auto strongThis = weakThis.GetStrongPtr())
			{
				return callback();
			}

			return Mso::MakeCanceledFuture();
		});
}

}
} // namespace Mso::React
