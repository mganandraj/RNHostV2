#include "precomp.h"

#include "ReactInstanceAndroidDeferred.h"
#include <fbjni/fbjni.h>
#include <dlfcn.h>
#include "OfficeAssetApi.h"

using namespace facebook;
using namespace facebook::jni;

namespace {
void* getOfficeHostHandle()
{
	void* hostHandle = loadCommonLibrary(L"officereacthost", RTLD_NOW);
	return hostHandle;
}
} // namespace

namespace Mso::React {
namespace Deferred {
Mso::TCntPtr<IReactInstanceInternal> MakeReactInstance(
    IReactHost &reactHost,
    ReactOptions &&options,
    Mso::JSHost::IRekaContextProxy &rekaContextProxy,
    Mso::Promise<void> &&whenLoaded) noexcept {
	auto method = (Mso::TCntPtr<IReactInstanceInternal>(*)(IReactHost & reactHost, ReactOptions &&, Mso::JSHost::IRekaContextProxy &, Mso::Promise<void> &&))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details17MakeReactInstanceERNS0_10IReactHostEONS0_12ReactOptionsERNS_6JSHost17IRekaContextProxyEONS_7PromiseIvEE"));
	auto instance = method(reactHost, std::move(options), rekaContextProxy, std::move(whenLoaded));
	VerifyElseCrashTag(instance, 0x1e44609c /* tag_4rgc2 */);
	return instance;
}
} // namespace Deferred

Mso::React::JReactHost_::JavaPart::javaobject ToJReactHost(Mso::TCntPtr<Mso::React::IReactHost> reactHost) {
#ifdef USE_OPENSOUCE_MSO
    auto method = (Mso::React::JReactHost_::JavaPart::javaobject(*)(Mso::TCntPtr<Mso::React::IReactHost>))(
            dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details12ToJReactHostENS_6CntPtrINS0_10IReactHostEEE"));
#else
    auto method = (Mso::React::JReactHost_::JavaPart::javaobject(*)(Mso::TCntPtr<Mso::React::IReactHost>))(
		dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details12ToJReactHostENS_7TCntPtrINS0_10IReactHostEEE"));
#endif
	auto jHost = method(reactHost);
	VerifyElseCrashTag(jHost, 0x1e44609b /* tag_4rgc1 */);
	return jHost;
}

JReactOptions_::javaobject ToJReactOptions(ReactOptions &&options) {
	auto method = (JReactOptions_::javaobject(*)(ReactOptions && options))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details15ToJReactOptionsEONS0_12ReactOptionsE"));
	auto jOptions = method(std::move(options));
	VerifyElseCrashTag(jOptions, 0x1e44609a /* tag_4rgc0 */);
	return jOptions;
}

ReactOptions ToReactOptions(JReactOptions_::JavaPart::javaobject jOptions) {
	auto method = (Mso::React::ReactOptions(*)(Mso::React::JReactOptions_::JavaPart::javaobject))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details14ToReactOptionsEPN8facebook3jni6detail8JTypeForINS5_11HybridClassINS0_14JReactOptions_ENS6_15BaseHybridClassEE8JavaPartENS5_7JObjectEvE11_javaobjectE"));
	auto options = method(std::move(reinterpret_cast<Mso::React::JReactOptions_::JavaPart::javaobject>(jOptions)));
	return options;
}

facebook::react::RuntimeExecutor GetRuntimeExecutor2(Mso::React::IReactInstance &instance) noexcept {
	auto method = (facebook::react::RuntimeExecutor(*)(Mso::React::IReactInstance &))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details18GetRuntimeExecutorERNS0_14IReactInstanceE"));
	auto runtimeExecutor = method(instance);
	VerifyElseCrashTag(runtimeExecutor, 0x1e446099 /* tag_4rgcz */);
	return runtimeExecutor;
}

facebook::jsi::Runtime *GetJsiRuntime(IReactInstance &instance) noexcept {
	auto method = (facebook::jsi::Runtime * (*)(Mso::React::IReactInstance &))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details13GetJsiRuntimeERNS0_14IReactInstanceE"));
	auto runtime = method(instance);
	VerifyElseCrashTag(runtime, 0x1e446098 /* tag_4rgcy */);
	return runtime;
}

std::shared_ptr<facebook::react::CallInvoker> GetJSThreadCallInvoker(IReactInstance &instance) noexcept {
	auto method = (std::shared_ptr<facebook::react::CallInvoker>(*)(Mso::React::IReactInstance &))(
	    dlsym(getOfficeHostHandle(), "_ZN3Mso5React7Android8Deferred7Details22GetJSThreadCallInvokerERNS0_14IReactInstanceE"));
	auto invoker = method(instance);
	VerifyElseCrashTag(invoker, 0x1e446097 /* tag_4rgcx */);
	return invoker;
}
} // namespace Mso::React