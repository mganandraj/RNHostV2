//#include "precomp.h"
#include "ReactHost.h"
//#include "ChangeGate.h"
#include <Future/FutureWait.h>
//#include <telemetryactivity/activity.h>
//#include "reactnativehost.tml.h"
//#include "ReactNativeLogTelemetry.h"
#include "BundleServerJSBundle.h"

#include "crash/verifyElseCrash.h"

namespace Mso::React {

//=============================================================================================
// ReactOptions implementation
//=============================================================================================

LIBLET_PUBLICAPI ReactOptions& ReactOptions::AddRegisteredJSBundle(std::string_view jsBundleId) noexcept
{
	this->JSBundles.push_back(MakeRegisteredJSBundle(jsBundleId));
	return *this;
}

//! Adds JS bundle based on a file name to JSBundles.
LIBLET_PUBLICAPI ReactOptions& ReactOptions::AddFileJSBundle(std::string_view jsBundleId, std::string_view fileName) noexcept
{
	this->JSBundles.push_back(MakeFileJSBundle(jsBundleId.data(), fileName.data()));
	return *this;
}

//! Adds JS bundle based on a string to JSBundles.
LIBLET_PUBLICAPI ReactOptions& ReactOptions::AddDynamicJSBundle(std::string&& jsBundleId, std::string&& jsBundleContent) noexcept
{
	this->JSBundles.push_back(MakeDynamicJSBundle(std::move(jsBundleId), std::move(jsBundleContent)));
	return *this;
}

//! Adds JS bundle downloaded from a bundle server (Dev scenarios only - when no offline file exists).
LIBLET_PUBLICAPI ReactOptions& ReactOptions::AddBundleServerJSBundle(std::string&& jsBundleId) noexcept
{
	this->JSBundles.push_back(MakeBundleServerJSBundle(std::move(jsBundleId)));
	return *this;
}

//=============================================================================================
// AsyncActionQueue implementation
//=============================================================================================

AsyncActionQueue::AsyncActionQueue(Mso::DispatchQueue queue) noexcept
	: m_queue{queue}
{
}

Mso::Future<void> AsyncActionQueue::PostAction(AsyncAction&& action) noexcept
{
	Entry entry{std::move(action), Mso::Promise<void>{}};
	Mso::Future<void> result = entry.Result.AsFuture();
	auto& actionQueue = m_actions.Load();
	if (!m_isInvoking.Load() && actionQueue.empty())
	{
		InvokeAction(std::move(entry));
	}
	else
	{
		actionQueue.push_back(std::move(entry));
	}

	return result;
}

Mso::Future<void> AsyncActionQueue::PostActions(std::initializer_list<AsyncAction> actions) noexcept
{
	// Return Future for the last action in the list
	Mso::Future<void> result;
	for (auto& action : actions)
	{
		// We must copy action because the initialize_list is read-only (a bug in the standard?).
		result = PostAction(Mso::Copy(action));
	}

	if (!result)
	{
		result = Mso::MakeSucceededFuture();
	}

	return result;
}

void AsyncActionQueue::InvokeAction(Entry&& entry) noexcept
{
	Mso::Internal::VerifyIsInQueueElseCrash(m_queue);

	m_isInvoking.Store(true);
	auto actionResult = entry.Action();
	actionResult.Then(m_executor,
		[entry = std::move(entry), spThis = Mso::MakeTCntPtr(this)](Mso::Maybe<void>&& result) mutable noexcept {
			spThis->CompleteAction(std::move(entry), std::move(result));
		});
}

void AsyncActionQueue::CompleteAction(Entry&& entry, Mso::Maybe<void>&& result) noexcept
{
	// Complete the action
	entry.Result.SetValue(std::move(result));
	entry = {nullptr, nullptr}; // Release the action before the next one starts

	// Start the next action from the queue.
	auto& actionQueue = m_actions.Load();
	if (!actionQueue.empty())
	{
		auto nextActionEntry = std::move(actionQueue[0]);
		actionQueue.erase(actionQueue.begin());
		InvokeAction(std::move(nextActionEntry));
	}
	else
	{
		m_isInvoking.Store(false);
	}
}

Mso::DispatchQueue AsyncActionQueue::Queue() noexcept
{
	return m_queue;
}

//=============================================================================================
// ReactHost implementation
//=============================================================================================

ReactHost::ReactHost(ReactOptions&& options, Mso::Promise<void>&& onInstanceLoaded) noexcept
	: Super{GetNativeQueue(options)}
	, m_onInstanceLoaded{std::move(onInstanceLoaded), Queue()}
	// , m_rekaContextProxy{Mso::JSHost::MakeRekaContextProxy(&Queue())}
	, m_options{std::move(options), Queue(), m_mutex}
	, m_notifyWhenClosed{ReactHostRegistry::Register(*this), Queue(), m_mutex}
{
}

ReactHost::~ReactHost() noexcept
{
}

// Initialize is always called from the Native queue
void ReactHost::Initialize() noexcept
{
//	Mso::Telemetry::Activity activity(
//		Office::SDX::Runtime::ReactNativeHost::EventName("Initialize"_S),
//		Mso::Telemetry::DataCategories::ProductServiceUsage);
//
//	ShipAssertSzTag(Options().Identity != "index.win32", "Identify field must be more unique to help identify current SDX", 0x1f892603 /* tag_58syd */);
//
//	activity.DataFields().Add("SDXIdentity", Options().Identity, Mso::Telemetry::DataClassifications::SystemMetadata);
//	activity.SetSuccess(true);

// NOTE:: NOTE:: This may not be required on Android-- Double check
	// EnsureUIQueue(m_options.Load());

	auto whenLoaded = m_actionQueue.Load()->PostAction(MakeLoadInstanceAction(Mso::Copy(m_options.Load())));

	// We only use the m_onInstanceLoaded for the initial notification and then set it to null.
	Mso::SetPromiseValue(m_onInstanceLoaded.Exchange(nullptr), std::move(whenLoaded));
}

// Finalize is always called from the Native queue
void ReactHost::Finalize() noexcept
{
	// If we are here, then it means that the current ref count is zero.
	// Since each AsyncAction has a strong ref count to ReactHost, the AsyncActionQueue must be empty.
	// Thus, we only need to call UnloadInQueue to unload ReactInstance if the ReactHost is not closed yet.
	if (Mso::Promise<void> notifyWhenClosed = m_notifyWhenClosed.Exchange(nullptr))
	{
		UnloadInQueue(0).Then<Mso::Executors::Inline>([notifyWhenClosed = std::move(notifyWhenClosed)]() noexcept {
			notifyWhenClosed.TrySetValue();
		});
	}
}

void ReactHost::Close() noexcept
{
//	MsoSendStructuredTraceTag(0x2205d809 /* tag_8b36j */,
//		Mso::Logging::Category::ReactNativeModule,
//		Mso::Logging::Severity::Verbose,
//		Mso::Logging::DataCategories::ProductServiceUsage,
//		L"Closing ReactHost",
//		Mso::Logging::StructuredString(L"BundleName", Options().Identity));

	InvokeInQueue([this]() noexcept {
//		MsoSendStructuredTraceTag(0x2205d808 /* tag_8b36i */,
//			Mso::Logging::Category::ReactNativeModule,
//			Mso::Logging::Severity::Verbose,
//			Mso::Logging::DataCategories::ProductServiceUsage,
//			L"Posting UnloadInstanceAction",
//			Mso::Logging::StructuredString(L"BundleName", Options().Identity));

		// Put the ReactHost to the closed state, unload ReactInstance, and notify the closing Promise.
		auto whenClosed = m_actionQueue.Load()->PostAction(MakeUnloadInstanceAction());

		// After we set the m_notifyWhenClosed to null, the ReactHost is considered to be closed.
		Mso::SetPromiseValue(m_notifyWhenClosed.Exchange(nullptr), std::move(whenClosed));
	});
}

bool ReactHost::IsClosed() const noexcept
{
	return !static_cast<bool>(m_notifyWhenClosed.Load());
}

Mso::CntPtr<AsyncActionQueue> ReactHost::ActionQueue() const noexcept
{
	return m_actionQueue.LoadWithLock();
}

size_t ReactHost::PendingUnloadActionId() const noexcept
{
	return m_pendingUnloadActionId.Load();
}

bool ReactHost::IsInstanceLoaded() const noexcept
{
	return m_isInstanceLoaded.Load();
}

/*static*/ Mso::DispatchQueue ReactHost::GetNativeQueue(const ReactOptions& options) noexcept
{
//	auto nativeDispatchQueue = options.Properties.Get(NativeDispatchQueueProperty);
//	if (nativeDispatchQueue)
//	{
//		// VerifyElseCrashSzTag(Mso::Async::IsSequentialQueue(*nativeDispatchQueue), "NativeQueue must be sequential", 0x0281e3d6 /* tag_c64pw */);
//	}
//	else
//	{
		// Try to use current queue
		auto nativeDispatchQueue = Mso::DispatchQueue::CurrentQueue();
		// if (!Mso::IsSequentialQueue(*nativeDispatchQueue))
		if (!nativeDispatchQueue.IsSerial())
		{
			nativeDispatchQueue = Mso::DispatchQueue::MakeSerialQueue() /*CreateBackgroundSequentialDispatchQueue()*/;
		}
//	}

//	if (JSHost::ChangeGate::ShouldFlushUIQueueOnShutdown())
//	{
//		// Wrap up the nativeDispatchQueue into a forwarder queue to handle its items in liblet Uninit.
//		// We must do it only for the UI queue, but we do not have a reliable way to determine the UI Queue.
//		// For now we use the fact that the UI queue is implemented as a Forwarder queue.
//		if (Mso::Async::IsMainThreadQueue(nativeDispatchQueue.Get()))
//		{
//			nativeDispatchQueue = Mso::Async::MakeSequentialDispatchQueueForwarder(nativeDispatchQueue.Get());
//		}
//		options.Properties.Set(NativeDispatchQueueProperty, nativeDispatchQueue);
//	}

	return nativeDispatchQueue;
}

ReactOptions ReactHost::Options() const noexcept
{
	return m_options.LoadWithLock();
}

Mso::CntPtr<IReactInstance> ReactHost::Instance() const noexcept
{
	return m_reactInstance.LoadWithLock();
}

//Mso::JSHost::IRekaContext& ReactHost::RekaContext() noexcept
//{
//	return *m_rekaContextProxy;
//}

Mso::DispatchQueue ReactHost::NativeQueue() const noexcept
{
	return Queue();
}

Mso::Future<void> ReactHost::ReloadInstance() noexcept
{
	return ReloadInstanceWithOptions(m_options.LoadWithLock());
}

Mso::Future<void> ReactHost::ReloadInstanceWithOptions(ReactOptions&& options) noexcept
{
//	MsoSendStructuredTraceTag(0x2205d807 /* tag_8b36h */,
//		Mso::Logging::Category::ReactNativeModule,
//		Mso::Logging::Severity::Verbose,
//		Mso::Logging::DataCategories::ProductServiceUsage,
//		L"Reloading ReactHost",
//		Mso::Logging::StructuredString(L"OldBundleName", Options().Identity),
//		Mso::Logging::StructuredString(L"NewBundleName", options.Identity));

	return PostInQueue([this, options = std::move(options)]() mutable noexcept {
		EnsureUIQueue(options);
		return m_actionQueue.Load()->PostActions({MakeUnloadInstanceAction(),
			MakeLoadInstanceAction(std::move(options))});
	});
}

Mso::Future<void> ReactHost::UnloadInstance() noexcept
{
//	MsoSendStructuredTraceTag(0x2205d806 /* tag_8b36g */,
//		Mso::Logging::Category::ReactNativeModule,
//		Mso::Logging::Severity::Verbose,
//		Mso::Logging::DataCategories::ProductServiceUsage,
//		L"Unloading ReactHost",
//		Mso::Logging::StructuredString(L"dBundleName", Options().Identity));

	return PostInQueue([this]() noexcept {
		return m_actionQueue.Load()->PostAction(MakeUnloadInstanceAction());
	});
}

AsyncAction ReactHost::MakeLoadInstanceAction(ReactOptions&& options) noexcept
{
	return [spThis = Mso::MakeTCntPtr(this), options = std::move(options)]() mutable noexcept {
		return spThis->LoadInQueue(std::move(options));
	};
}

AsyncAction ReactHost::MakeUnloadInstanceAction() noexcept
{
	size_t unloadActionId = ++m_nextUnloadActionId.Load();
	m_pendingUnloadActionId.Store(Mso::Copy(unloadActionId));
	return [spThis = Mso::MakeTCntPtr(this), unloadActionId]() noexcept {
		return spThis->UnloadInQueue(unloadActionId);
	};
}

Mso::CntPtr<IReactViewHost> ReactHost::MakeViewHost(ReactViewOptions&& options) noexcept
{
//	Mso::Telemetry::Activity activity(
//		Office::SDX::Runtime::ReactNativeHost::EventName("MakeViewHost"_S),
//		Mso::Telemetry::DataCategories::ProductServiceUsage);
//
//	activity.DataFields().Add("SDXIdentity", Options().Identity, Mso::Telemetry::DataClassifications::SystemMetadata);
//	activity.DataFields().Add("ComponentName", options.ComponentName, Mso::Telemetry::DataClassifications::SystemMetadata);
//	activity.SetSuccess(true);

	return Mso::Make<ReactViewHost, IReactViewHost>(*this, std::move(options));
}

Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> ReactHost::GetViewHostList() noexcept
{
	Mso::Promise<std::vector<Mso::CntPtr<IReactViewHost>>> result;

	InvokeInQueue([this, result]() noexcept {
		std::vector<Mso::CntPtr<IReactViewHost>> viewHosts;
		ForEachViewHost([&viewHosts](auto& viewHost) noexcept {
			viewHosts.push_back(&viewHost);
		});
		result.SetValue(std::move(viewHosts));
	});

	return result.AsFuture();
}

Mso::Future<void> ReactHost::LoadInQueue(ReactOptions&& options) noexcept
{
	// If the ReactHost is already closed then we cancel loading ReactInstance.
	if (IsClosed())
	{
		return Mso::MakeCanceledFuture();
	}

	// Make sure that we set new options even if we do not load due to the pending unload.
	m_options.Exchange(Mso::Copy(options));

	// If there is a pending unload action, then we cancel loading ReactInstance.
	if (PendingUnloadActionId())
	{
		return Mso::MakeCanceledFuture();
	}

	Mso::Promise<void> onLoaded;
	m_reactInstance.Exchange(MakeReactInstance(*this, std::move(options)/*, *m_rekaContextProxy*/, Mso::Copy(onLoaded)));

	// After the instance is loaded, we load the view instances.
	// It is safe to capture 'this' because the Load action keeps a strong reference to ReactHost.
	return onLoaded.AsFuture().Then(m_executor, [this](Mso::Maybe<void>&& /*value*/) noexcept {
		m_isInstanceLoaded.Store(true);

		std::vector<Mso::Future<void>> loadCompletionList;
		ForEachViewHost([&loadCompletionList](auto& viewHost) noexcept {
			loadCompletionList.push_back(viewHost.LoadViewInstanceInQueue(viewHost.Options()));
		});

		return Mso::WhenAllCompleted(loadCompletionList);
	});
}

Mso::Future<void> ReactHost::UnloadInQueue(size_t unloadActionId) noexcept
{
	// If the pending unload action Id does not match, then we have newer unload action,
	// and thus we should cancel this one.
	if (unloadActionId != PendingUnloadActionId())
	{
		return Mso::MakeCanceledFuture();
	}

	// Clear the pending unload action Id
	m_pendingUnloadActionId.Store(0);

	std::vector<Mso::Future<void>> unloadCompletionList;
	ForEachViewHost([&unloadCompletionList](auto& viewHost) noexcept {
		unloadCompletionList.push_back(viewHost.UnloadViewInstanceInQueue(0));
	});

	// We unload ReactInstance after all view instances are unloaded.
	// It is safe to capture 'this' because the Unload action keeps a strong reference to ReactHost.
	return Mso::WhenAllCompleted(unloadCompletionList).Then(m_executor, [this](Mso::Maybe<void>&& /*value*/) noexcept {
		// m_rekaContextProxy->SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));

		Mso::Future<void> onUnloaded;
		if (auto reactInstance = m_reactInstance.Exchange(nullptr))
		{
			m_isInstanceLoaded.Store(false);
			onUnloaded = reactInstance->Destroy();
		}

		m_lastError.Store({});

		if (!onUnloaded)
		{
			onUnloaded = Mso::MakeSucceededFuture();
		}

		return onUnloaded;
	});
}

void ReactHost::ForEachViewHost(const Mso::FunctorRef<void(ReactViewHost&)>& action) noexcept
{
	for (const auto& entry : m_viewHosts.Load())
	{
		if (auto viewHost = entry.second.GetStrongPtr())
		{
			action(*viewHost);
		}
	}
}

void ReactHost::AttachViewHost(ReactViewHost& viewHost) noexcept
{
	auto& viewHosts = m_viewHosts.Load();

	uintptr_t viewHostKey = reinterpret_cast<uintptr_t>(&viewHost);
	auto it = viewHosts.find(viewHostKey);
	// VerifyElseCrashSzTag(it == viewHosts.end(), "The view host is already attached", 0x0281e3d8 /* tag_c64py */);
	viewHosts[viewHostKey] = &viewHost;
}

void ReactHost::DetachViewHost(ReactViewHost& viewHost) noexcept
{
	auto& viewHosts = m_viewHosts.Load();

	uintptr_t viewHostKey = reinterpret_cast<uintptr_t>(&viewHost);
	auto it = viewHosts.find(viewHostKey);
	// VerifyElseCrashSzTag(it != viewHosts.end(), "The view host is not attached", 0x0281e3d9 /* tag_c64pz */);
	viewHosts.erase(it);
}

void ReactHost::EnsureUIQueue(const ReactOptions& options) noexcept
{
//	if (!JSHost::ChangeGate::ShouldFlushUIQueueOnShutdown())
//		return;
//
//	auto uiQueue = options.Properties.Get(UIDispatchQueueProperty);
//	if (uiQueue && uiQueue == m_uiQueue.Load())
//	{
//		// The uiQueue is already set up correctly.
//		return;
//	}
//
//	if (uiQueue)
//	{
//		// Wrap up the uiQueue into a forwarder queue to handle its items in liblet Uninit
//		// We must do it only for the UI queue, but we do not have a reliable way to determine the UI queue.
//		// For now we use the fact that the UI queue is implemented as a Forwarder queue.
//		if (Mso::Async::IsMainThreadQueue(uiQueue.Get()))
//		{
//			uiQueue = Mso::Async::MakeSequentialDispatchQueueForwarder(uiQueue.Get());
//		}
//	}
//	else
//	{
//		// Use the native queue as the UI queue.
//		// The native queue is already wrapped up into a forwarder queue.
//		uiQueue = &Queue();
//	}
//
//	options.Properties.Set(UIDispatchQueueProperty, uiQueue);
//	m_uiQueue.Exchange(std::move(uiQueue));
}

//void ReactHost::GetQueueControllersForLibletUninit(
//	std::vector<Mso::CntPtr<Mso::Async::IThreadDispatchQueueController>>& queueControllers) noexcept
//{
//	Mso::CntPtr<Mso::IDispatchQueue> queue = &Queue();
//	if (HasCurrentThreadAccess(*queue))
//	{
//		// The query_cast to a reference VECs if the interface is not found.
//		queueControllers.push_back(&query_cast<Mso::Async::IThreadDispatchQueueController&>(*queue));
//	}
//
//	queue = m_uiQueue.LoadWithLock();
//	if (HasCurrentThreadAccess(*queue))
//	{
//		queueControllers.push_back(&query_cast<Mso::Async::IThreadDispatchQueueController&>(*queue));
//	}
//}

//bool ReactHost::HasCurrentThreadAccess(Mso::IDispatchQueue& queue) noexcept
//{
//	bool result{false};
//	Mso::Async::InvokeElsePost(&queue, [threadId = std::this_thread::get_id(), &result]() noexcept {
//		if (threadId == std::this_thread::get_id())
//		{
//			result = true;
//		}
//	});
//
//	return result;
//}

//=============================================================================================
// ReactViewHost implementation
//=============================================================================================

ReactViewHost::ReactViewHost(
	Mso::React::ReactHost& reactHost,
	ReactViewOptions&& options) noexcept
	: Super{reactHost.NativeQueue()}
	, m_actionQueue{reactHost.ActionQueue(), Queue()}
	, m_reactHost{&reactHost}
	, m_options{std::move(options), Queue(), m_mutex}
{
}

ReactViewHost::~ReactViewHost() noexcept
{
}

IReactHost& ReactViewHost::ReactHost() const noexcept
{
	return *m_reactHost;
}

ReactViewOptions ReactViewHost::Options() const noexcept
{
	return m_options.LoadWithLock();
}

void ReactViewHost::SetOptions(ReactViewOptions&& options) noexcept
{
	m_options.Exchange(std::move(options));
}

Mso::Future<void> ReactViewHost::ReloadViewInstance() noexcept
{
	return PostInQueue([this]() noexcept {
		return m_actionQueue.Load()->PostActions({MakeUnloadViewInstanceAction(),
			MakeLoadViewInstanceAction()});
	});
}

Mso::Future<void> ReactViewHost::ReloadViewInstanceWithOptions(ReactViewOptions&& options) noexcept
{
	return PostInQueue([this, options = std::move(options)]() mutable noexcept {
		return m_actionQueue.Load()->PostActions({MakeUnloadViewInstanceAction(),
			MakeLoadViewInstanceAction(std::move(options))});
	});
}

Mso::Future<void> ReactViewHost::UnloadViewInstance() noexcept
{
#pragma warning(suppress : 5233) // explicit lambda capture 'spThis' is not used
	return PostInQueue([this, spThis = Mso::CntPtr{this}]() noexcept {
		return m_actionQueue.Load()->PostAction(MakeUnloadViewInstanceAction());
	});
}

Mso::Future<void> ReactViewHost::AttachViewInstance(IReactViewInstance& viewInstance) noexcept
{
	return PostInQueue([this, viewInstance = Mso::MakeTCntPtr(&viewInstance)]() noexcept {
		auto previousViewInstance = m_viewInstance.Exchange(Mso::Copy(viewInstance));
		// // VerifyElseCrashSzTag(!previousViewInstance, "ViewInstance must not be previously attached.", 0x028508d6 /* tag_c7q9w */);
		m_reactHost->AttachViewHost(*this);

		// Schedule the viewInstance load in the action queue since there can be other load actions in the queue that need to be consolidated.
		return m_actionQueue.Load()->PostAction(MakeLoadViewInstanceAction());
	});
}

Mso::Future<void> ReactViewHost::DetachViewInstance() noexcept
{
#pragma warning(suppress : 5233) // explicit lambda capture 'spThis' is not used
	return PostInQueue([this, spThis = Mso::CntPtr(this)]() noexcept {
		auto viewInstance = m_viewInstance.Exchange(nullptr);
		// // VerifyElseCrashSzTag(viewInstance, "ViewInstance is not attached.", 0x0281e3db /* tag_c64p1 */);
		m_reactHost->DetachViewHost(*this);

		// We unload the viewInstance here as soon as possible without using the action queue,
		// otherwise we would not have the viewInstance to call the Unload() later.
		m_isViewInstanceLoaded.Store(false);
		return viewInstance->Unload();
	});
}

AsyncAction ReactViewHost::MakeLoadViewInstanceAction() noexcept
{
	return [spThis = Mso::MakeTCntPtr(this)]() mutable noexcept {
		return spThis->LoadViewInstanceInQueue();
	};
}

AsyncAction ReactViewHost::MakeLoadViewInstanceAction(ReactViewOptions&& options) noexcept
{
	return [spThis = Mso::MakeTCntPtr(this), options = std::move(options)]() mutable noexcept {
		return spThis->LoadViewInstanceInQueue(std::move(options));
	};
}

AsyncAction ReactViewHost::MakeUnloadViewInstanceAction() noexcept
{
	size_t unloadActionId = ++m_nextUnloadActionId.Load();
	m_pendingUnloadActionId.Store(std::move(unloadActionId));
	return [spThis = Mso::MakeTCntPtr(this), unloadActionId]() noexcept {
		return spThis->UnloadViewInstanceInQueue(unloadActionId);
	};
}

Mso::Future<void> ReactViewHost::LoadViewInstanceInQueue() noexcept
{
	// It is already loaded: nothing to do.
	if (m_isViewInstanceLoaded.Load())
	{
		return Mso::MakeSucceededFuture();
	}

	// Cancel if the ReactHost is already closed.
	if (m_reactHost->IsClosed())
	{
		return Mso::MakeCanceledFuture();
	}

	// If there is a pending unload action, then we cancel loading ReactViewInstance.
	if (m_reactHost->PendingUnloadActionId() || m_pendingUnloadActionId.Load())
	{
		return Mso::MakeCanceledFuture();
	}

	// We cannot load if instance is not loaded.
	if (!m_reactHost->IsInstanceLoaded())
	{
		return Mso::MakeCanceledFuture();
	}

	// Make sure that we have a ReactInstance
	if (!m_reactHost->Instance())
	{
		return Mso::MakeCanceledFuture();
	}

	if (auto viewInstance = m_viewInstance.Load().Get())
	{
		m_isViewInstanceLoaded.Store(true);
		return viewInstance->Reload(m_reactHost->Instance(), Mso::Copy(m_options.Load()));
	}

	return Mso::MakeCanceledFuture();
}

Mso::Future<void> ReactViewHost::LoadViewInstanceInQueue(ReactViewOptions&& options) noexcept
{
	// Make sure that we set new options even if we do not load due to the pending unload.
	m_options.Exchange(std::move(options));

	// VerifyElseCrashSzTag(!m_isViewInstanceLoaded.Load(), "The viewInstance must be unloaded before the load with new options.", 0x0285411a /* tag_c7ue0 */);

	return LoadViewInstanceInQueue();
}

Mso::Future<void> ReactViewHost::UnloadViewInstanceInQueue(size_t unloadActionId) noexcept
{
	// If the pending unload action Id does not match, then we have newer unload action,
	// and thus we should cancel this one.
	if (unloadActionId != m_pendingUnloadActionId.Load())
	{
		return Mso::MakeCanceledFuture();
	}

	// Clear the pending unload action Id
	m_pendingUnloadActionId.Store(0);

	if (m_reactHost->PendingUnloadActionId())
	{
		return Mso::MakeCanceledFuture();
	}

	if (auto viewInstance = m_viewInstance.Load().Get())
	{
		m_isViewInstanceLoaded.Store(false);
		return viewInstance->Unload();
	}

	return Mso::MakeCanceledFuture();
}

//=============================================================================================
// ReactHostRegistry implementation
//=============================================================================================

/*static*/ std::recursive_mutex ReactHostRegistry::s_mutex;
/*static*/ Mso::CntPtr<ReactHostRegistry> ReactHostRegistry::s_registry;

ReactHostRegistry::~ReactHostRegistry() noexcept
{
}

/*static*/ void ReactHostRegistry::OnLibletInit() noexcept
{
	// Create new global ReactHostRegistry
	std::lock_guard<std::recursive_mutex> lock{s_mutex};
	// VerifyElseCrashSzTag(!s_registry, "No ReactHostRegistry must exist yet.", 0x0281e3dc /* tag_c64p2 */);
	s_registry = Mso::Make<ReactHostRegistry>();
//	Mso::Diagnostics::SendDiagnosticTrace(0x2205d804 /* tag_8b36e */,
//		Mso::Diagnostics::Category::ReactNativeModule,
//		Mso::Diagnostics::Severity::Info,
//		Mso::Diagnostics::ValidDataCategories::ProductServiceUsage,
//		"Initialized ReactHostRegistry"_S);
}

/*static*/ void ReactHostRegistry::OnLibletUninit() noexcept
{
//	// Wait for all pending ReactHost futures indicating closing state and remove the global ReactHostRegistry.
//	std::vector<Mso::Future<void>> futures;
//
//	// For RN instances to complete shutdown we need to pump queues that use the current thread.
//	std::vector<Mso::CntPtr<Mso::Async::IThreadDispatchQueueController>> queueControllers;
//
////	Mso::Diagnostics::SendDiagnosticTrace(0x2205d802 /* tag_8b36c */,
////		Mso::Diagnostics::Category::ReactNativeModule,
////		Mso::Diagnostics::Severity::Info,
////		Mso::Diagnostics::ValidDataCategories::ProductServiceUsage,
////		"Uninitializing ReactHostRegistry"_S);
//
//	{
//		std::lock_guard<std::recursive_mutex> lock{s_mutex};
//		auto registry = std::move(s_registry);
//
//		for (const auto& entry : registry->m_entries)
//		{
//			if (auto reactHost = entry.second.WeakReactHost.GetStrongPtr())
//			{
////				Mso::Diagnostics::SendDiagnosticTrace(0x2205d800 /* tag_8b36a */,
////					Mso::Diagnostics::Category::ReactNativeModule,
////					Mso::Diagnostics::Severity::Info,
////					Mso::Diagnostics::ValidDataCategories::ProductServiceUsage,
////					"Closing ReactNative Host on Uninitializatoin of ReactHostRegistry"_S,
////					// React Identities are fixed names in our codebase, so SystemMetaData is an appropritate classification
////					Mso::Diagnostics::ClassifiedStructuredString("BundleName"_S, reactHost->Options().Identity, Mso::Logging::DataClassifications::SystemMetadata));
//
//				if (JSHost::ChangeGate::ShouldFlushUIQueueOnShutdown())
//				{
//					reactHost->GetQueueControllersForLibletUninit(/*ref*/ queueControllers);
//				}
//				reactHost->Close();
//			}
//
////			Mso::Diagnostics::SendDiagnosticTrace(0x2205d7e2 /* tag_8b358 */,
////				Mso::Diagnostics::Category::ReactNativeModule,
////				Mso::Diagnostics::Severity::Info,
////				Mso::Diagnostics::ValidDataCategories::ProductServiceUsage,
////				"Registring wait on futures for ReactNative Host to close"_S);
//
//			futures.push_back(entry.second.WhenReactHostClosed);
//		}
//
//		registry->m_entries.clear();
//	}
//
//	WaitForReactHostClosed(Mso::WhenAllCompleted(futures), queueControllers);

//	Mso::Diagnostics::SendDiagnosticTrace(0x2205d7e0 /* tag_8b356 */,
//		Mso::Diagnostics::Category::ReactNativeModule,
//		Mso::Diagnostics::Severity::Info,
//		Mso::Diagnostics::ValidDataCategories::ProductServiceUsage,
//		"Uninitialized ReactHostRegistry"_S);
}

/*static*/ Mso::Promise<void> ReactHostRegistry::Register(IClosableReactHost& reactHost) noexcept
{
	std::lock_guard<std::recursive_mutex> lock{s_mutex};
	uintptr_t reactHostKey = reinterpret_cast<uintptr_t>(&reactHost);

//	MsoSendStructuredTraceTag(0x2205d7df /* tag_8b355 */,
//		Mso::Logging::Category::ReactNativeModule,
//		Mso::Logging::Severity::Verbose,
//		Mso::Logging::DataCategories::ProductServiceUsage,
//		L"Registering ReactHost to ReactHostRegistry",
//		Mso::Logging::StructuredString(L"BundleName", reactHost.Options().Identity),
//		Mso::Logging::StructuredPtr(L"ReactHostKey", reinterpret_cast<void*>(reactHostKey)));

	if (!s_registry)
	{
		// Return an empty Promise to indicate failure to register.
		return Mso::Promise<void>{nullptr};
	}

	auto& entries = s_registry->m_entries;
	auto it = entries.find(reactHostKey);
	// VerifyElseCrashSzTag(it == entries.end(), "The ReactHost is already registered", 0x0281e3dd /* tag_c64p3 */);

	Mso::Promise<void> notifyWhenClosed;
	Mso::Future<void> whenReactHostClosed = notifyWhenClosed.AsFuture().Then<Mso::Executors::Inline>(
		[registry{s_registry}, reactHostKey](Mso::Maybe<void>&& result) noexcept {
//			MsoSendStructuredTraceTag(0x2205d7de /* tag_8b354 */,
//				Mso::Logging::Category::ReactNativeModule,
//				Mso::Logging::Severity::Verbose,
//				Mso::Logging::DataCategories::ProductServiceUsage,
//				L"UnRegistring ReactHost Handler",
//				Mso::Logging::StructuredPtr(L"ReactHostKey", reinterpret_cast<void*>(reactHostKey)));

			registry->Unregister(reactHostKey);
			return std::move(result);
		});
	entries.emplace(reactHostKey, Entry{&reactHost, std::move(whenReactHostClosed)});
	return notifyWhenClosed;
}

void ReactHostRegistry::Unregister(uintptr_t reactHostKey) noexcept
{
//	MsoSendStructuredTraceTag(0x2205d7dd /* tag_8b353 */,
//		Mso::Logging::Category::ReactNativeModule,
//		Mso::Logging::Severity::Verbose,
//		Mso::Logging::DataCategories::ProductServiceUsage,
//		L"Unregistering ReactHost from ReactHostRegistry",
//		Mso::Logging::StructuredPtr(L"ReactHostKey", reinterpret_cast<void*>(reactHostKey)));

	std::lock_guard<std::recursive_mutex> lock{s_mutex};

	// Do not unregister ReactHost if we're already in OnLibletUninit to keep m_entries untouched
	if (s_registry)
	{
		auto it = m_entries.find(reactHostKey);
		// VerifyElseCrashSzTag(it != m_entries.end(), "The ReactHost is not registered", 0x0281e3de /* tag_c64p4 */);
		m_entries.erase(it);
	}
}

} // namespace Mso::React
