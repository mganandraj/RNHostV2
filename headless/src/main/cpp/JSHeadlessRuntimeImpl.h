#pragma once

#include <jshostheadless/JSHeadlessRuntime.h>

#include <ReactNativeHost/React.h>
#include <jsi/jsi.h>

#ifdef USE_OPENSOUCE_MSO
#include <object/refCountedObject.h>
#endif

namespace Mso::JSHost::Headless {

struct JSHeadlessRuntime : public Mso::RefCountedObject<
    Mso::RefCountStrategy::WeakRef, 
	Mso::QueryCastDerived<JSHeadlessRuntime>,
    JSRuntime>
{
private:
    Mso::React::ReactOptions CreateReactOptions() noexcept;
    void PrimeRuntime() noexcept;
    JSRuntimeOptions& Options() noexcept { return m_options; };
    void MarkAsDestroyed() noexcept { m_jsRuntimeDestroyed = true; }
    void PostInternal(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) noexcept;

private:
    JSRuntimeOptions m_options;
    Mso::TCntPtr<Mso::React::IReactHost> m_spReactHost {nullptr};
    std::mutex g_pendingFuncs_mutex;
    std::vector<std::function<void(facebook::jsi::Runtime& runtime)>> m_pendingFuncs;
    std::atomic<bool> m_jsRuntimeInitialized;
    std::atomic<bool> m_jsRuntimeDestroyed;

public:
    JSHeadlessRuntime(JSRuntimeOptions&& options) noexcept;
    virtual ~JSHeadlessRuntime() noexcept override;
    virtual void Post(std::function<void(facebook::jsi::Runtime& runtime)>&&callback) noexcept override;
};

} // namespace Mso::JSHost
