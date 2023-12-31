#include "precomp.h"

#ifdef USE_OPENSOUCE_MSO

#include <activeObject/activeObject.h>
#include <object/refCountedObject.h>
#include <smartPtr/cntPtr.h>

namespace Mso {
template <typename T>
using TCntPtr = CntPtr<T>;
}
#else
#include <DispatchQueue/activeobject.h>
#endif

#include <ReactNativeHost/JSBundle.h>
#include <ReactNativeHost/React.h>
#include <ReactNativeHost/React_Android.h>
#include <errorCode/hresultErrorProvider.h>

#include <ReactCommon/RuntimeExecutor.h>
#include <react/jni/JRuntimeExecutor.h>
#include <ReactCommon/CallInvokerHolder.h>

#include <fbjni/fbjni.h>
#include <fbjni/ByteBuffer.h>

#include <dlfcn.h>
#include "OfficeAssetApi.h"

#include <rnxreacthost/JJsiRuntimeRef.h>

#include "IReactInstanceInternal.h"

using namespace Mso::React;
using namespace facebook;
using namespace facebook::jni;

namespace {

class ReactInstanceAndroidInternal;

class ReactInstanceAndroid final
    : public Mso::ActiveObject<Mso::React::IReactInstanceInternal> {
	using Super = ActiveObjectType;

public: // IReactInstance
	const std::string &JsBundleName() const noexcept override;
	std::string SDXBasePath() const noexcept override;

	bool NeedsReload() const noexcept override {
		VerifyElseCrashSzTag(false, "Not implemented", 0x1e450410 /* tag_4rqqq */);
	}

	void SetAsNeedsReload() noexcept override {
		VerifyElseCrashSzTag(false, "Not implemented", 0x1e45040f /* tag_4rqqp */);
	}

	bool GetQuirk(SDXQuirk /*quirk*/) const noexcept override {
		VerifyElseCrashSzTag(false, "Not implemented", 0x1e45040e /* tag_4rqqo */);
	}

	const ReactOptions &Options() const noexcept override;

public: // IReactInstanceInternal
	Mso::Future<void> Destroy() noexcept override;
	void onInitialized() noexcept;
	void onBundleLoaded(std::string &&bundleName) noexcept;
	Mso::JSHost::RekaBridgeOptions createRekaBridgeOptions() noexcept;
    void onDoRuntimeInstall(jsi::Runtime& runtime) noexcept;

private:
	friend MakePolicy;
	ReactInstanceAndroid(Mso::React::IReactHost &reactHost,
	                     Mso::React::ReactOptions &&options) noexcept;

	void Initialize() noexcept override;
	void Finalize() noexcept override;
	void NotifyRekaInitialized() noexcept;
	void NotifyRekaDestroyed() noexcept;
	std::atomic<bool> m_isRekaInitialized{false};

private: // fields controlled by queue
	const Mso::ActiveField<Mso::TCntPtr<Mso::JSHost::IRekaContext>> m_rekaContext{Queue()};

public:
	ReactInstanceAndroidInternal &InternalState() {
		return *m_internalState;
	}

private:
	Mso::React::ReactOptions m_options;
	Mso::TCntPtr<ReactInstanceAndroidInternal> m_internalState;
};

struct JMsoFuture : jni::HybridClass<JMsoFuture> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/MsoFuturePeer;";
	static void registerNatives();
	static jni::local_ref<jhybridobject> create();
	Mso::Future<void> getMsoFuture();
	void Set();
	JMsoFuture()
	    : promise_(Mso::Promise<void>()) {
	}

private:
	Mso::Promise<void> promise_;
};

void JMsoFuture::registerNatives() {
	registerHybrid({makeNativeMethod("Set", JMsoFuture::Set)});
}

void JMsoFuture::Set() {
	promise_.SetValue();
}

jni::local_ref<JMsoFuture::jhybridobject> JMsoFuture::create() {
	return newObjectCxxArgs();
}

Mso::Future<void> JMsoFuture::getMsoFuture() {
	return promise_.AsFuture();
}

struct JReactDevOptions : jni::HybridClass<JReactDevOptions> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactDevOptions;";
	static void registerNatives();
	static jni::local_ref<jhybridobject> create(Mso::React::ReactOptions &);
	JReactDevOptions(Mso::React::ReactOptions &options)
	    : options_(options) {
	}

private:
	bool getIsDevModeEnabled();
	void setIsDevModeEnabled(bool isDevModeEnabled);
	bool getUseDirectDebugger();
	void setUseDirectDebugger(bool useDirectDebugger);
	std::string getDebuggerRuntimeName();
	void setDebuggerRuntimeName(std::string debuggerRuntimeName);
	std::string getSourceBundleName();
	void setSourceBundleName(std::string sourceBundleName);

private:
	Mso::React::ReactOptions &options_;
};

struct JJSBundleInfo : jni::JavaClass<JJSBundleInfo> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundleInfo;";
	static Mso::React::JSBundleInfo get(jni::alias_ref<JJSBundleInfo> thizz);
	static jni::local_ref<JJSBundleInfo> create(Mso::React::JSBundleInfo info);
};

// Note :: This could be a hybrid object instead. But keeping it simple for now, and copying the object fields across the JNI boundary except the bundle shared buffer
struct JJSBundle : jni::JavaClass<JJSBundle> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundle;";
	static Mso::TCntPtr<Mso::React::IJSBundle> get(jni::alias_ref<JJSBundle> thizz);
	static jni::local_ref<JJSBundle> create(Mso::React::IJSBundle &bundle);
};

struct JInstanceCreatedCallback;
struct JInstanceLoadedCallback;
struct JLogHandler;
struct JFatalErrorHandler;

struct JReactOptions : jni::HybridClass<JReactOptions> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactOptions;";
	static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
	static void registerNatives();
	static jni::local_ref<jhybridobject> create(const Mso::React::ReactOptions &&);
	JReactOptions(const Mso::React::ReactOptions &&options)
	    : options_(std::move(options)) {
	}
	const Mso::React::ReactOptions &Options() const noexcept;
	std::string getIdentity();
	void setIdentity(std::string identity);
	jni::alias_ref<jni::JArrayList<jni::JString>> getJavaModuleNames();
	void setJavaModuleNames(jni::alias_ref<jni::JArrayList<jni::JString>>);
	void addJavaModuleName(std::string javaModuleName);
	std::string getDataServiceProviderName();
	void setDataServiceProviderName(std::string dataServiceProviderName);
	jni::alias_ref<jni::JArrayList<JJSBundle>> getJSBundles();
	void setJSBundles(jni::alias_ref<jni::JArrayList<JJSBundle>>);
	void addJSBundle(jni::alias_ref<JJSBundle>);
	void setInstanceCreatedCallback(jni::alias_ref<JInstanceCreatedCallback>);
	jni::alias_ref<JInstanceCreatedCallback> getInstanceCreatedCallback();
	void setInstanceLoadedCallback(jni::alias_ref<JInstanceLoadedCallback>);
	jni::alias_ref<JInstanceLoadedCallback> getInstanceLoadedCallback();

	void setLogHandler(jni::alias_ref<JLogHandler>);
	jni::alias_ref<JLogHandler> getLogHandler();

	void setFatalErrorHandler(jni::alias_ref<JFatalErrorHandler>);
	jni::alias_ref<JFatalErrorHandler> getFatalErrorHandler();

	jni::alias_ref<JReactDevOptions::jhybridobject> createDeveloperSettingsPeer();

private:
	Mso::React::ReactOptions options_;
};

struct JErrorCode : jni::HybridClass<JErrorCode> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ErrorCode;";
	static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
	static void registerNatives();
	static jni::local_ref<jhybridobject> create(Mso::ErrorCode &&);
	std::string toString();
    Mso::ErrorCode& getMsoErrorCode();
	JErrorCode(Mso::ErrorCode &&errorCode)
	    : errorCode_(std::move(errorCode)) {
	}

private:
	Mso::ErrorCode errorCode_;
};

local_ref<JErrorCode::jhybriddata> JErrorCode::initHybrid(alias_ref<jhybridobject> /*jThis*/) {
	Mso::ErrorCode errorCode;
	return makeCxxInstance(std::move(errorCode));
}

std::string JErrorCode::toString() {
	return errorCode_.ToString();
}

Mso::ErrorCode& JErrorCode::getMsoErrorCode() {
    return errorCode_;
}

jni::local_ref<JErrorCode::jhybridobject> JErrorCode::create(Mso::ErrorCode &&errorCode) {
	return newObjectCxxArgs(std::move(errorCode));
}

void JErrorCode::registerNatives() {
	registerHybrid({
	    makeNativeMethod("initHybrid", JErrorCode::initHybrid),
	    makeNativeMethod("toString", JErrorCode::toString),
	});
}

struct JRekaBridgeOptions : jni::HybridClass<JRekaBridgeOptions> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/reka/RekaBridgeOptions;";
};

struct JReactInstance : jni::HybridClass<JReactInstance> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactInstance;";
	static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
	static void registerNatives();

	static void onInitialized(jni::alias_ref<jhybridobject> jThis /*, jni::alias_ref<JReactContextHolder> contextHolder*/);
	static void onBundleLoaded(jni::alias_ref<jhybridobject> jThis,
	                           jni::alias_ref<JString> bundleName);

    static void onDoRuntimeInstall(jni::alias_ref<jhybridobject> jThis,
                               jni::alias_ref<react::JJsiRuntimeRef::jhybridobject> runtimeRef);

	static jni::local_ref<JRekaBridgeOptions::jhybridobject> createRekaBridgeOptions(jni::alias_ref<jhybridobject> jThis);
	static jni::local_ref<jhybridobject> create(jni::alias_ref<JReactOptions::jhybridobject>, Mso::TCntPtr<ReactInstanceAndroid> nativeInstance);

	static facebook::react::RuntimeExecutor GetRuntimeExecutor(jni::alias_ref<JReactInstance::javaobject>);
	static facebook::jsi::Runtime *GetJsiRuntime(jni::alias_ref<JReactInstance::javaobject> instance);
	static std::shared_ptr<facebook::react::CallInvoker> getJSCallInvokerHolder(jni::alias_ref<JReactInstance::javaobject> instance);

    static void onLog(jni::alias_ref<JReactInstance::javaobject> instance, jni::alias_ref<JString>, int logLevel);
    static void onFatalError(jni::alias_ref<JReactInstance::javaobject> instance, jni::alias_ref<JErrorCode::javaobject> errorCode);

private:
	Mso::WeakPtr<ReactInstanceAndroid> m_wNativeInstance;
};

struct ReactInstanceAndroidInternal : public Mso::RefCountedObject<Mso::IRefCounted> {
	void Initialize(ReactOptions optionsCopy, Mso::WeakPtr<ReactInstanceAndroid>);
	void Destroy();

	jni::alias_ref<JReactInstance::jhybridobject> JInstance();
	jni::alias_ref<JReactOptions::jhybridobject> JOptions();

private:
	jni::global_ref<JReactOptions::jhybridobject> m_jOptions;
	jni::global_ref<JReactInstance::jhybridobject> m_jReactInstance;
};

jni::alias_ref<JReactInstance::jhybridobject> ReactInstanceAndroidInternal::JInstance() {
	return jni::wrap_alias(m_jReactInstance.get());
}
jni::alias_ref<JReactOptions::jhybridobject> ReactInstanceAndroidInternal::JOptions() {
	return jni::wrap_alias(m_jOptions.get());
}

struct JInstanceCreatedCallback : jni::JavaClass<JInstanceCreatedCallback> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceCreatedCallback;";
	static void run(jni::alias_ref<JInstanceCreatedCallback> callback, jni::alias_ref<JReactInstance::jhybridobject>);
};

struct JInstanceLoadedCallback : jni::JavaClass<JInstanceLoadedCallback> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceLoadedCallback;";

	static void run(jni::alias_ref<JInstanceLoadedCallback> callback,
	                jni::alias_ref<JReactInstance::jhybridobject>,
	                jni::alias_ref<JErrorCode::jhybridobject>);
};

struct JLogHandler : jni::JavaClass<JLogHandler> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ILogHandler;";
	static void run(jni::alias_ref<JLogHandler> callback, jni::alias_ref<jni::JString>,
					int);
};

struct JFatalErrorHandler : jni::JavaClass<JFatalErrorHandler> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IFatalErrorHandler;";
	static void run(jni::alias_ref<JFatalErrorHandler> callback, jni::alias_ref<JErrorCode::javaobject>);
};

void JLogHandler::run(jni::alias_ref<JLogHandler> callback,
                      jni::alias_ref<jni::JString> message, int logLevel) {
    static auto clazz = javaClassStatic();
    auto method = clazz->getMethod<void(jni::alias_ref<JString>,int)>("run");
    method(callback, message, logLevel);
}

void JFatalErrorHandler::run(jni::alias_ref<JFatalErrorHandler> callback,
                             jni::alias_ref<JErrorCode::javaobject>) {
    std::abort();
}

void JInstanceCreatedCallback::run(jni::alias_ref<JInstanceCreatedCallback> callback,
                                   jni::alias_ref<JReactInstance::jhybridobject> instance) {
	static auto clazz = javaClassStatic();
	auto method = clazz->getMethod<void(jni::alias_ref<JReactInstance::jhybridobject>)>("run");
	method(callback, instance);
}

void JInstanceLoadedCallback::run(jni::alias_ref<JInstanceLoadedCallback> callback,
                                  jni::alias_ref<JReactInstance::jhybridobject> instance,
                                  jni::alias_ref<JErrorCode::jhybridobject> errorCode) {
	static auto clazz = javaClassStatic();
	auto method = clazz->getMethod<void(jni::alias_ref<JReactInstance::jhybridobject>, jni::alias_ref<JErrorCode::jhybridobject>)>("run");
	method(callback, instance, errorCode);
}

struct ByteBufferJSBundle : public ::Mso::UnknownObject<IJSBundle> {
public:
	std::string_view Content() noexcept override;
	JSBundleInfo Info() noexcept override;

private:
	friend MakePolicy; // Allows Mso::Make to access private constructor
	ByteBufferJSBundle(JSBundleInfo &&info, local_ref<JByteBuffer>) noexcept;
	uint64_t Timestamp() noexcept;

private:
	JSBundleInfo m_info;
	global_ref<JByteBuffer> m_byteBuffer; // Owns the java ByteBuffer
};

struct InfoJSBundle : public ::Mso::UnknownObject<IJSBundle> {
public:
	std::string_view Content() noexcept override;
	JSBundleInfo Info() noexcept override;

private:
	friend MakePolicy; // Allows Mso::Make to access private constructor
	InfoJSBundle(JSBundleInfo &&info) noexcept;
	uint64_t Timestamp() noexcept;

private:
	JSBundleInfo m_info;
};

ByteBufferJSBundle::ByteBufferJSBundle(JSBundleInfo &&info,
                                       local_ref<JByteBuffer> content) noexcept
    : m_info(std::move(info))
    , m_byteBuffer(make_global(content)) {
}

InfoJSBundle::InfoJSBundle(JSBundleInfo &&info) noexcept
    : m_info(std::move(info)) {
}

std::string_view InfoJSBundle::Content() noexcept {
	return std::string_view();
}

JSBundleInfo InfoJSBundle::Info() noexcept {
	return m_info;
}

std::string_view ByteBufferJSBundle::Content() noexcept {
	size_t size = m_byteBuffer->getDirectSize();
	const char *bytes = reinterpret_cast<const char *>(m_byteBuffer->getDirectBytes());
	return std::string_view(bytes, size);
}

JSBundleInfo ByteBufferJSBundle::Info() noexcept {
	return m_info;
}

::Mso::TCntPtr<::Mso::React::IJSBundle> JJSBundle::get(alias_ref<JJSBundle> thizz) {
	const auto cls = javaClassStatic();
	const auto iFieldContent = cls->getField<JByteBuffer>("Content");
	local_ref<JByteBuffer> content = make_local(thizz->getFieldValue(iFieldContent));
	const auto iFieldInfo = cls->getField<JJSBundleInfo>("Info");
	local_ref<JJSBundleInfo> info = make_local(thizz->getFieldValue(iFieldInfo));

	::Mso::TCntPtr<::Mso::React::IJSBundle> jsBundle;
	if (content) {
		jsBundle = ::Mso::Make<ByteBufferJSBundle>(JJSBundleInfo::get(info), content);
	} else {
		jsBundle = ::Mso::Make<InfoJSBundle>(JJSBundleInfo::get(info));
	}
	return jsBundle;
}

::Mso::React::JSBundleInfo JJSBundleInfo::get(jni::alias_ref<JJSBundleInfo> thizz) {
	::Mso::React::JSBundleInfo info;
	const auto cls = javaClassStatic();
	const auto iFieldId = cls->getField<JString>("Id");
	auto idValue = thizz->getFieldValue(iFieldId);
	if (idValue) {
		info.Id = idValue->toStdString();
	}

	const auto iFieldFileName = cls->getField<JString>("FileName");
	auto fileNameValue = thizz->getFieldValue(iFieldFileName);
	if (fileNameValue) {
		info.FileName = fileNameValue->toStdString();
	}

	const auto iFieldTimestamp = cls->getField<JLong>("Timestamp");
	auto timestampValue = thizz->getFieldValue(iFieldTimestamp);
	if (timestampValue) {
		info.Timestamp = timestampValue->value();
	}

	return info;
}

jni::local_ref<JJSBundleInfo> JJSBundleInfo::create(::Mso::React::JSBundleInfo info) {
	return newInstance(make_jstring(info.Id), make_jstring(info.FileName),
	                   JLong::valueOf(info.Timestamp));
}

jni::local_ref<JJSBundle> JJSBundle::create(::Mso::React::IJSBundle &bundle) {
	local_ref<jni::JByteBuffer> content;
	if (!bundle.Content().empty()) {
		content = jni::JByteBuffer::wrapBytes(
		    const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(bundle.Content().data())),
		    bundle.Content().size());
	}

	return newInstance(content, JJSBundleInfo::create(bundle.Info()));
}

local_ref<JReactOptions::jhybridobject>
JReactOptions::create(const Mso::React::ReactOptions &&options) {
	return newObjectCxxArgs(std::move(options));
}

local_ref<JReactOptions::jhybriddata>
JReactOptions::initHybrid(alias_ref<jhybridobject> /*jThis*/) {
	Mso::React::ReactOptions options;
	return makeCxxInstance(std::move(options));
}

std::string JReactOptions::getIdentity() {
	return options_.Identity;
}

void JReactOptions::setIdentity(std::string identity) {
	options_.Identity = identity;
}

jni::alias_ref<jni::JArrayList<jni::JString>>
JReactOptions::getJavaModuleNames() {
	auto jJavaModuleNames = JArrayList<JString>::create();
	std::for_each(options_.JavaModuleNames.begin(), options_.JavaModuleNames.end(),
	              [&jJavaModuleNames](std::string item) {
		              jJavaModuleNames->add(make_jstring(item));
	              });
	return jJavaModuleNames.release();
}

void JReactOptions::setJavaModuleNames(
    jni::alias_ref<jni::JArrayList<jni::JString>> jJavaModuleNames) {
	options_.JavaModuleNames.clear();
	for (const auto &elem : *jJavaModuleNames) {
		options_.JavaModuleNames.push_back(elem->toStdString());
	}
}

void JReactOptions::addJavaModuleName(std::string javaModuleName) {
	options_.JavaModuleNames.push_back(javaModuleName);
}

std::string JReactOptions::getDataServiceProviderName() {
	return options_.DataServiceProviderName;
}

void JReactOptions::setDataServiceProviderName(std::string dataServiceProviderName) {
	options_.DataServiceProviderName = dataServiceProviderName;
}

jni::alias_ref<jni::JArrayList<JJSBundle>> JReactOptions::getJSBundles() {
	auto jJSBundles = jni::JArrayList<JJSBundle>::create(options_.JSBundles.size());
	for (auto &jsBundle : options_.JSBundles) {
		jJSBundles->add(JJSBundle::create(*jsBundle));
	}
	return jJSBundles.release();
}

// Note :: This clears the existing bundles.
void JReactOptions::setJSBundles(
    jni::alias_ref<jni::JArrayList<JJSBundle>> jsBundles) {
	options_.JSBundles.clear();
	for (const auto &elem : *jsBundles) {
		options_.JSBundles.push_back(JJSBundle::get(make_local(elem)));
	}
}

void JReactOptions::addJSBundle(jni::alias_ref<JJSBundle> jsBundle) {
	options_.JSBundles.push_back(JJSBundle::get(make_local(jsBundle)));
}

void JReactOptions::setInstanceCreatedCallback(
    jni::alias_ref<JInstanceCreatedCallback> callback) {
	options_.OnInstanceCreated = [callback = make_global(callback)](IReactInstance &instance) {
		// TODO :: Make it safer
		ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
		JInstanceCreatedCallback::run(callback, instanceImpl->InternalState().JInstance());
	};
}

jni::alias_ref<JInstanceCreatedCallback> JReactOptions::getInstanceCreatedCallback() {
	return nullptr;
}

void JReactOptions::setInstanceLoadedCallback(
    jni::alias_ref<JInstanceLoadedCallback> callback) {
	options_.OnInstanceLoaded = [callback = make_global(callback)](IReactInstance &instance,
	                                                               const Mso::ErrorCode &errorCode) {
		// TODO :: Make it safer
		ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
		::Mso::ErrorCode errorCodeCopy = errorCode;
		JInstanceLoadedCallback::run(callback, instanceImpl->InternalState().JInstance(),
		                             JErrorCode::create(std::move(errorCodeCopy)));
	};
}

jni::alias_ref<JInstanceLoadedCallback> JReactOptions::getInstanceLoadedCallback() {
	return nullptr;
}

void JReactOptions::setLogHandler(jni::alias_ref<JLogHandler> callback) {
	options_.OnLogging = [callback = make_global(callback)](LogLevel logLevel, const char* message) {
		JLogHandler::run(callback, jni::make_jstring(message), static_cast<int>(logLevel));
	};
}

jni::alias_ref<JLogHandler> JReactOptions::getLogHandler() {
    return nullptr;
}

void JReactOptions::setFatalErrorHandler(jni::alias_ref<JFatalErrorHandler> callback) {
    options_.OnError = [callback = make_global(callback)](const Mso::ErrorCode&) {
        // JFatalErrorHandler::run(jni::make_jstring(message));
    };
}

jni::alias_ref<JFatalErrorHandler> JReactOptions::getFatalErrorHandler() {
    return nullptr;
}

const ReactOptions &JReactOptions::Options() const noexcept {
	return options_;
}

jni::alias_ref<JReactDevOptions::jhybridobject>
JReactOptions::createDeveloperSettingsPeer() {
	return JReactDevOptions::create(options_).release();
}

jni::local_ref<JReactDevOptions::jhybridobject>
JReactDevOptions::create(Mso::React::ReactOptions &options) {
	return JReactDevOptions::newObjectCxxArgs(options);
}

bool JReactDevOptions::getIsDevModeEnabled() {
	return options_.DeveloperSettings.IsDevModeEnabled;
}

void JReactDevOptions::setIsDevModeEnabled(bool isDevModeEnabled) {
	options_.DeveloperSettings.IsDevModeEnabled = isDevModeEnabled;
}

bool JReactDevOptions::getUseDirectDebugger() {
	return options_.DeveloperSettings.UseDirectDebugger;
}

void JReactDevOptions::setUseDirectDebugger(bool useDirectDebugger) {
	options_.DeveloperSettings.UseDirectDebugger = useDirectDebugger;
}

std::string JReactDevOptions::getDebuggerRuntimeName() {
	return options_.DeveloperSettings.DebuggerRuntimeName;
}

void JReactDevOptions::setDebuggerRuntimeName(std::string debuggerRuntimeName) {
	options_.DeveloperSettings.DebuggerRuntimeName = debuggerRuntimeName;
}

std::string JReactDevOptions::getSourceBundleName() {
	return options_.DeveloperSettings.SourceBundleName;
}

void JReactDevOptions::setSourceBundleName(std::string sourceBundleName) {
	options_.DeveloperSettings.SourceBundleName = sourceBundleName;
}

void JReactDevOptions::registerNatives() {
	registerHybrid({makeNativeMethod("getIsDevModeEnabled", "()Z",
	                                 JReactDevOptions::getIsDevModeEnabled),
	                makeNativeMethod("setIsDevModeEnabled", "(Z)V",
	                                 JReactDevOptions::setIsDevModeEnabled),
	                makeNativeMethod("getUseDirectDebugger", "()Z",
	                                 JReactDevOptions::getUseDirectDebugger),
	                makeNativeMethod("setUseDirectDebugger", "(Z)V",
	                                 JReactDevOptions::setUseDirectDebugger),
	                makeNativeMethod("getDebuggerRuntimeName",
	                                 JReactDevOptions::getDebuggerRuntimeName),
	                makeNativeMethod("setDebuggerRuntimeName",
	                                 JReactDevOptions::setDebuggerRuntimeName),
	                makeNativeMethod("getSourceBundleName",
	                                 JReactDevOptions::getSourceBundleName),
	                makeNativeMethod("setSourceBundleName",
	                                 JReactDevOptions::setSourceBundleName)});
}

void JReactOptions::registerNatives() {
	registerHybrid({makeNativeMethod("initHybrid", JReactOptions::initHybrid),
	                makeNativeMethod("getIdentity", JReactOptions::getIdentity),
	                makeNativeMethod("setIdentity", JReactOptions::setIdentity),
	                makeNativeMethod("getJavaModuleNames", JReactOptions::getJavaModuleNames),
	                makeNativeMethod("setJavaModuleNames", JReactOptions::setJavaModuleNames),
	                makeNativeMethod("AddJavaModuleName", JReactOptions::addJavaModuleName),
	                makeNativeMethod("getDataServiceProviderName",
	                                 JReactOptions::getDataServiceProviderName),
	                makeNativeMethod("setDataServiceProviderName",
	                                 JReactOptions::setDataServiceProviderName),
	                makeNativeMethod("getJSBundles", JReactOptions::getJSBundles),
	                makeNativeMethod("setJSBundles", JReactOptions::setJSBundles),
	                makeNativeMethod("AddJSBundle", JReactOptions::addJSBundle),
	                makeNativeMethod("getInstanceCreatedCallback",
	                                 JReactOptions::getInstanceCreatedCallback),
	                makeNativeMethod("setInstanceCreatedCallback",
	                                 JReactOptions::setInstanceCreatedCallback),
	                makeNativeMethod("getInstanceLoadedCallback",
	                                 JReactOptions::getInstanceLoadedCallback),
	                makeNativeMethod("setInstanceLoadedCallback",
	                                 JReactOptions::setInstanceLoadedCallback),
	                makeNativeMethod("createDeveloperSettingsPeer",
	                                 JReactOptions::createDeveloperSettingsPeer),
					makeNativeMethod("getOnLogging",
									 JReactOptions::getLogHandler),
					makeNativeMethod("setOnLogging",
									 JReactOptions::setLogHandler),
					makeNativeMethod("getOnError",
									 JReactOptions::getFatalErrorHandler),
					makeNativeMethod("setOnError",
									 JReactOptions::setFatalErrorHandler)});
}

jni::local_ref<JReactInstance::jhybriddata>
JReactInstance::initHybrid(jni::alias_ref<jhybridobject> /*jThis*/) {
	return makeCxxInstance();
}

void JReactInstance::onInitialized(
    jni::alias_ref<jhybridobject> jThis /*, jni::alias_ref<JReactContextHolder> contextHolder*/) {
	auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
	if (nativeInstance) {
		nativeInstance->onInitialized(/*contextHolder*/);
	}
}

void JReactInstance::onBundleLoaded(jni::alias_ref<jhybridobject> jThis,
                                    jni::alias_ref<JString> bundleName) {
	auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
	if (nativeInstance) {
		nativeInstance->onBundleLoaded(bundleName->toStdString());
	}
}

void JReactInstance::onDoRuntimeInstall(jni::alias_ref<jhybridobject> jThis,
							   jni::alias_ref<react::JJsiRuntimeRef::jhybridobject> runtimeRef) {
	auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
	if (nativeInstance) {
		nativeInstance->onDoRuntimeInstall(runtimeRef->cthis()->get());
	}
}

jni::local_ref<JRekaBridgeOptions::jhybridobject>
JReactInstance::createRekaBridgeOptions(jni::alias_ref<jhybridobject> jThis) {
	static const WCHAR c_wzReactRekaLibNickName[] = L"reactrekadroid";
	auto rekahandle = loadCommonLibrary(c_wzReactRekaLibNickName, RTLD_NOW);
	auto method = (jni::local_ref<JRekaBridgeOptions::jhybridobject>(*)(
	    Mso::JSHost::RekaBridgeOptions && rekaOptions))(dlsym(rekahandle, "_ZN9reactreka18JRekaBridgeOptions6createEON3Mso6JSHost17RekaBridgeOptionsE"));
	auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
	VerifyElseCrashTag(nativeInstance, 0x1e450412 /* tag_4rqqs */);
	return method(nativeInstance->createRekaBridgeOptions());
}

void JReactInstance::registerNatives() {
	registerHybrid({
	    makeNativeMethod("initHybrid", JReactInstance::initHybrid),
	    makeNativeMethod("onInitialized", JReactInstance::onInitialized),
	    makeNativeMethod("onBundleLoaded", JReactInstance::onBundleLoaded),
		makeNativeMethod("onDoRuntimeInstall", JReactInstance::onDoRuntimeInstall),
	    makeNativeMethod("createRekaBridgeOptions", JReactInstance::createRekaBridgeOptions),
        makeNativeMethod("onLogImpl", JReactInstance::onLog),
        makeNativeMethod("onFatalErrorImpl", JReactInstance::onFatalError),
	});
}

jni::local_ref<JReactInstance::jhybridobject>
JReactInstance::create(jni::alias_ref<JReactOptions::jhybridobject> options,
                       Mso::TCntPtr<ReactInstanceAndroid> nativeInstance) {
	// Create the java peer.
	auto jInstance = JReactInstance::newObjectJavaArgs(options);

	// Associate the native peer with the native instance.
	jInstance->cthis()->m_wNativeInstance = Mso::WeakPtr(nativeInstance);

	// Initialize the instance
	auto clazz = javaClassLocal();
	auto initializeMethod = clazz->getMethod<void()>("initialize");
	initializeMethod(jInstance);

	return jInstance;
}

// TODO:: Null check
facebook::react::RuntimeExecutor JReactInstance::GetRuntimeExecutor(
    jni::alias_ref<JReactInstance::javaobject> instance) {
	auto getRuntimeExecutorMethod = javaClassLocal()->getMethod<facebook::react::JRuntimeExecutor::jhybridobject()>("getRuntimeExecutor");
	local_ref<facebook::react::JRuntimeExecutor::jhybridobject> jRuntimeExecutor = make_local(getRuntimeExecutorMethod(instance));

	static const WCHAR c_wzRNLibNickName[] = L"reactnativejni";
	auto rnhandle = loadCommonLibrary(c_wzRNLibNickName, RTLD_NOW);
	auto method = (facebook::react::RuntimeExecutor(*)(facebook::react::JRuntimeExecutor *))(dlsym(rnhandle, "_ZN8facebook5react16JRuntimeExecutor3getEv"));
	auto runtimeExecutor = method(jRuntimeExecutor->cthis());

	return runtimeExecutor;
}

// TODO:: Null check
facebook::jsi::Runtime *
JReactInstance::GetJsiRuntime(jni::alias_ref<JReactInstance::javaobject> instance) {
	auto getJsiRuntimeMethod = javaClassLocal()->getMethod<jlong()>("getJsiRuntimeRef");
	long jsiRuntimeRef = getJsiRuntimeMethod(instance);
	return reinterpret_cast<facebook::jsi::Runtime *>(jsiRuntimeRef);
}

// TODO:: Null check`
std::shared_ptr<facebook::react::CallInvoker> JReactInstance::getJSCallInvokerHolder(
    jni::alias_ref<JReactInstance::javaobject> instance) {
	auto getJSCallInvokerHolderMethod = javaClassLocal()->getMethod<facebook::react::CallInvokerHolder::jhybridobject()>("getJSCallInvokerHolder");
	auto jsCallInvokerHolder = getJSCallInvokerHolderMethod(instance);

	static const WCHAR c_wzRNLibNickName[] = L"reactnativejni";
	auto rnhandle = loadCommonLibrary(c_wzRNLibNickName, RTLD_NOW);
	auto method = (std::shared_ptr<facebook::react::CallInvoker>(*)(
	    facebook::react::CallInvokerHolder *))(dlsym(rnhandle,
	                                                 "_ZN8facebook5react17CallInvokerHolder14getCallInvokerEv"));
	std::shared_ptr<facebook::react::CallInvoker> callInvoker = method(
	    jsCallInvokerHolder->cthis());

	return callInvoker;
	// return jsCallInvokerHolder->cthis()->getCallInvoker();
}

void JReactInstance::onLog(jni::alias_ref<JReactInstance::javaobject> jInstance, jni::alias_ref<JString> jMessage, int logLevel) {
    auto instance = jInstance->cthis()->m_wNativeInstance.GetStrongPtr();
    if(instance)
        instance->Options().OnLogging(static_cast<LogLevel>(logLevel), jMessage->toStdString().c_str());
}

void JReactInstance::onFatalError(jni::alias_ref<JReactInstance::javaobject> jInstance, jni::alias_ref<JErrorCode::javaobject> errorCode) {
    auto instance = jInstance->cthis()->m_wNativeInstance.GetStrongPtr();
    if(instance)
        instance->Options().OnError(errorCode->cthis()->getMsoErrorCode());

}

struct JReactViewOptions : jni::HybridClass<JReactViewOptions> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewOptions;";

	static jni::local_ref<jhybriddata>
	initHybrid(jni::alias_ref<jhybridobject> jThis);

	static void registerNatives();

	JReactViewOptions(Mso::React::ReactViewOptions &&options)
	    : options_(std::move(options)) {
	}

	static jni::local_ref<jhybridobject>
	create(const Mso::React::ReactViewOptions &&);

	Mso::React::ReactViewOptions options_;

	std::string getComponentName();

	void setComponentName(std::string componentName);

	std::string getInitialProps();

	void setInitialProps(std::string initialProps);

	const Mso::React::ReactViewOptions &Options() const noexcept;
};

local_ref<JReactViewOptions::jhybridobject>
JReactViewOptions::create(const Mso::React::ReactViewOptions &&options) {
	return newObjectCxxArgs(std::move(const_cast<Mso::React::ReactViewOptions &&>(options)));
}

local_ref<JReactViewOptions::jhybriddata>
JReactViewOptions::initHybrid(alias_ref<jhybridobject> /*jThis*/) {
	Mso::React::ReactViewOptions options;
	return makeCxxInstance(std::move(options));
}

const Mso::React::ReactViewOptions &JReactViewOptions::Options() const noexcept {
	return options_;
}

std::string JReactViewOptions::getComponentName() {
	return options_.ComponentName;
}

void JReactViewOptions::setComponentName(std::string componentName) {
	options_.ComponentName = componentName;
}

std::string JReactViewOptions::getInitialProps() {
	return options_.InitialProps;
}

void JReactViewOptions::setInitialProps(std::string initialProps) {
	options_.InitialProps = initialProps;
}

void JReactViewOptions::registerNatives() {
	registerHybrid({makeNativeMethod("initHybrid", JReactViewOptions::initHybrid),
	                makeNativeMethod("getComponentName", JReactViewOptions::getComponentName),
	                makeNativeMethod("setComponentName", JReactViewOptions::setComponentName),
	                makeNativeMethod("getInitialProps", JReactViewOptions::getInitialProps),
	                makeNativeMethod("setInitialProps", JReactViewOptions::setInitialProps)});
}

struct OfficeReactViewInstance;

struct JBaseRootView : jni::HybridClass<JBaseRootView> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/BaseRootView;";

	static jni::local_ref<jhybriddata>
	initHybrid(jni::alias_ref<jhybridobject> jThis);

	static void registerNatives();

	Mso::Future<void> Reload(const Mso::TCntPtr<Mso::React::IReactInstance> &&reactInstance,
	                         const Mso::React::ReactViewOptions &&viewOptions) noexcept;

	Mso::Future<void> Unload() noexcept;

	JBaseRootView(jni::alias_ref<jhybridobject> jThis);

	OfficeReactViewInstance &ViewInstance() {
		return *m_ReactViewInstance;
	}

	Mso::TCntPtr<OfficeReactViewInstance> m_ReactViewInstance;
	jni::weak_ref<JBaseRootView::jhybridobject> m_jThis;
};

struct OfficeReactViewInstance : public Mso::UnknownObject<Mso::React::IReactViewInstance> {
	virtual Mso::Future<void> Reload(Mso::TCntPtr<Mso::React::IReactInstance> &&reactInstance,
	                                 Mso::React::ReactViewOptions &&viewOptions) noexcept override;

	virtual Mso::Future<void> Unload() noexcept override;

	OfficeReactViewInstance(jni::alias_ref<JBaseRootView::jhybridobject> jThis)
	    : m_jThis(jni::make_weak(std::move(jThis))) {
	}

	jni::weak_ref<JBaseRootView::jhybridobject> m_jThis;
};

void JBaseRootView::registerNatives() {
	registerHybrid({
	    makeNativeMethod("initHybrid", JBaseRootView::initHybrid),
	});
}

local_ref<JBaseRootView::jhybriddata>
JBaseRootView::initHybrid(alias_ref<jhybridobject> jThis) {
	return makeCxxInstance(std::move(jThis));
}

JBaseRootView::JBaseRootView(alias_ref<jhybridobject> jThis) {
	m_jThis = make_weak(std::move(jThis));
	m_ReactViewInstance = Mso::Make<OfficeReactViewInstance>(std::move(jThis));
}

Mso::Future<void>
JBaseRootView::Reload(const Mso::TCntPtr<Mso::React::IReactInstance> &&reactInstance,
                      const Mso::React::ReactViewOptions &&viewOptions) noexcept {
	auto future = JMsoFuture::create();
	auto msoFuture = future->cthis()->getMsoFuture();

	auto reloadMethod = javaClassLocal()->getMethod<void(JReactInstance::jhybridobject, JReactViewOptions::jhybridobject, JMsoFuture::jhybridobject)>("Reload");
	reloadMethod(m_jThis.lockLocal().release(),
	             static_cast<ReactInstanceAndroid *>(reactInstance.Get())->InternalState().JInstance().get(),
	             JReactViewOptions::create(std::move(viewOptions)).release(), future.release());
	return msoFuture;
}

Mso::Future<void> JBaseRootView::Unload() noexcept {
	auto future = JMsoFuture::create();
	auto msoFuture = future->cthis()->getMsoFuture();

	auto unloadMethod = javaClassLocal()->getMethod<void(JMsoFuture::jhybridobject)>("Unload");
	unloadMethod(m_jThis.lockLocal().release(), future.release());
	return msoFuture;
}

Mso::Future<void>
OfficeReactViewInstance::Reload(Mso::TCntPtr<Mso::React::IReactInstance> &&reactInstance,
                                Mso::React::ReactViewOptions &&viewOptions) noexcept {
	Mso::Future<void> result;
	ThreadScope::WithClassLoader(
	    [this, &result, reactInstance, viewOptions2 = std::move(viewOptions)]() {
		    auto local = m_jThis.lockLocal();
		    result = local->cthis()->Reload(std::move(reactInstance),
		                                    std::move(viewOptions2));
	    });

	return result;
}

Mso::Future<void> OfficeReactViewInstance::Unload() noexcept {
	Mso::Future<void> result;
	ThreadScope::WithClassLoader([this, &result]() {
            auto local = m_jThis.lockLocal();
            result = local->cthis()->Unload(); });

	return result;
}

struct JReactViewHost : jni::HybridClass<JReactViewHost> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewHost;";

	static void registerNatives();

	static jni::local_ref<jhybridobject>
	create(Mso::TCntPtr<Mso::React::IReactViewHost> viewHost);

	JReactViewHost(Mso::TCntPtr<Mso::React::IReactViewHost> &&viewHost)
	    : viewHost_(
	          std::move(viewHost)) {
	}

	Mso::TCntPtr<Mso::React::IReactViewHost> viewHost_;

	void AttachViewInstance(jni::alias_ref<JBaseRootView::jhybridobject> jView);
	void DetachViewInstance();
    void ReloadViewInstance();
    void ReloadViewInstanceWithOptions(jni::alias_ref<JReactViewOptions::jhybridobject> jOptions);
	jobject ReactHost();
};

void JReactViewHost::registerNatives() {
	registerHybrid(
	    {
			makeNativeMethod("ReactHostImpl", JReactViewHost::ReactHost),
            makeNativeMethod("AttachViewInstance", JReactViewHost::AttachViewInstance),
			makeNativeMethod("DetachViewInstance", JReactViewHost::DetachViewInstance),
            makeNativeMethod("ReloadViewInstance", JReactViewHost::ReloadViewInstance),
            makeNativeMethod("ReloadViewInstanceWithOptions", JReactViewHost::ReloadViewInstanceWithOptions)
        });
}

void JReactViewHost::AttachViewInstance(
    jni::alias_ref<JBaseRootView::jhybridobject> jView) {
	viewHost_->AttachViewInstance(jView->cthis()->ViewInstance());
}

void JReactViewHost::DetachViewInstance() {
	viewHost_->DetachViewInstance();
}

jni::local_ref<JReactViewHost::jhybridobject>
JReactViewHost::create(Mso::TCntPtr<Mso::React::IReactViewHost> viewHost) {
	return newObjectCxxArgs(std::move(viewHost));
}

void JReactViewHost::ReloadViewInstance() {
    viewHost_->ReloadViewInstance();
}

void JReactViewHost::ReloadViewInstanceWithOptions(jni::alias_ref<JReactViewOptions::jhybridobject> jOptions) {
    ReactViewOptions optionsCopy = jOptions->cthis()->options_;
    viewHost_->ReloadViewInstanceWithOptions(std::move(optionsCopy));
}

struct JReactHost : jni::HybridClass<JReactHost> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHost;";

	// static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject> jThis);
	static void registerNatives();

	static jni::local_ref<jhybridobject>
	create(Mso::TCntPtr<Mso::React::IReactHost> host);

	JReactHost(Mso::TCntPtr<Mso::React::IReactHost> &&host)
	    : host_(std::move(host)) {
	}

	~JReactHost() {
		FBJNI_LOGE("~JReactHost");
	};

	jni::alias_ref<JReactOptions::jhybridobject> Options();

	jni::alias_ref<JReactInstance::jhybridobject> Instance();

	jni::alias_ref<JReactViewHost::jhybridobject>
	MakeViewHost(jni::alias_ref<JReactViewOptions::jhybridobject> jOptions);

	void ReloadInstance();

	void
	ReloadInstanceWithOptions(jni::alias_ref<JReactOptions::jhybridobject> jOptions);

	void UnloadInstance();

	Mso::TCntPtr<Mso::React::IReactHost> host_;
};

void JReactHost::registerNatives() {
	registerHybrid({makeNativeMethod("Options", JReactHost::Options),
	                makeNativeMethod("Instance", JReactHost::Instance),
	                makeNativeMethod("MakeViewHost", JReactHost::MakeViewHost),
	                makeNativeMethod("ReloadInstance", JReactHost::ReloadInstance),
	                makeNativeMethod("ReloadInstanceWithOptions",
	                                 JReactHost::ReloadInstanceWithOptions),
	                makeNativeMethod("UnloadInstance", JReactHost::UnloadInstance)});
}

jni::alias_ref<JReactOptions::jhybridobject> JReactHost::Options() {
	auto instance = host_->Instance();
	return static_cast<ReactInstanceAndroid &>(*instance).InternalState().JOptions();
}

jni::alias_ref<JReactInstance::jhybridobject> JReactHost::Instance() {
	auto instance = host_->Instance();
	return static_cast<ReactInstanceAndroid &>(*instance).InternalState().JInstance();
}

alias_ref<JReactViewHost::jhybridobject>
JReactHost::MakeViewHost(alias_ref<JReactViewOptions::jhybridobject> jOptions) {
	ReactViewOptions viewOptions = jOptions->cthis()->Options();
	return JReactViewHost::create(host_->MakeViewHost(std::move(viewOptions))).release();
}

void JReactHost::ReloadInstance() {
	host_->ReloadInstance();
}

void JReactHost::ReloadInstanceWithOptions(
    jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
	auto options = jOptions->cthis()->Options();
	host_->ReloadInstanceWithOptions(std::move(options));
}

void JReactHost::UnloadInstance() {
	host_->UnloadInstance();
}

jni::local_ref<JReactHost::jhybridobject>
JReactHost::create(Mso::TCntPtr<Mso::React::IReactHost> host) {
	return newObjectCxxArgs(std::move(host));
}

jobject JReactViewHost::ReactHost() {
	auto& reactHost = viewHost_->ReactHost();
	return jni::getPlainJniReference(JReactHost::create(Mso::TCntPtr<Mso::React::IReactHost>(&reactHost)).release());
}

struct JReactHostStatics : jni::JavaClass<JReactHostStatics> {
	static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHostStatics;";
//
//	static void registerNatives();
//
//	static jni::alias_ref<JReactHost::jhybridobject>
//	makeJReactHost(jni::alias_ref<jclass>,
//	               jni::alias_ref<JReactOptions::jhybridobject> jOptions);
};

//jni::alias_ref<JReactHost::jhybridobject>
//JReactHostStatics::makeJReactHost(alias_ref<jclass>,
//                                 jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
//	auto options = jOptions->cthis()->Options();
//	Mso::TCntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));  TODO
//	return JReactHost::create(reactHost).release();
//}

//void JReactHostStatics::registerNatives() {
//	javaClassLocal()->registerNatives(
//	    {
//	        makeNativeMethod("makeReactHost", JReactHostStatics::makeJReactHost),
//	    });
//}

ReactInstanceAndroid::ReactInstanceAndroid(IReactHost &reactHost,
                                           ReactOptions &&options) noexcept
#ifdef USE_OPENSOUCE_MSO
    : Super{reactHost.NativeQueue()}
#else
    : Super{&reactHost.NativeQueue()}
#endif
    , m_options(std::move(options)) {
}

void ReactInstanceAndroid::onInitialized(/*jni::alias_ref<JReactContextHolder> contextHolder*/) noexcept {
	if (Options().OnInstanceCreated)
		Options().OnInstanceCreated(*this);
}

void ReactInstanceAndroid::onBundleLoaded(std::string && /*bundleName*/) noexcept {
	if (Options().OnInstanceLoaded)
		Options().OnInstanceLoaded(*this, Mso::HResultErrorProvider().MakeErrorCode(S_OK));
}

void ReactInstanceAndroid::onDoRuntimeInstall(jsi::Runtime& runtime) noexcept {
    auto runtimeInstallerProp = Options().Properties.Get(Mso::React::JsiRuntimeInstallerProperty);
    if(runtimeInstallerProp) {
		JsiRuntimeInstallerHolder runtimeInstallerHolder = runtimeInstallerProp.value();
		runtimeInstallerHolder.runtimeInstaller(runtime);
    }
}

void ReactInstanceAndroidInternal::Initialize(ReactOptions optionsCopy,
                                              Mso::WeakPtr<ReactInstanceAndroid> instanceWeak) {
	auto instance = instanceWeak.GetStrongPtr();
	VerifyElseCrashTag(instance, 0x1e450411 /* tag_4rqqr */);

	// Note: This is required as the AcitiveObject thread is a raw pthread thread (When running outside OMR) which don't have a Java stack in it yet.
	// https://developer.android.com/training/articles/perf-jni#faq_FindClass
	ThreadScope::WithClassLoader([this, optionsCopy = std::move(optionsCopy), instance]() {
            m_jOptions = make_global(JReactOptions::create(std::move(optionsCopy)));
            m_jReactInstance = make_global(JReactInstance::create(m_jOptions, instance)); });
}

void ReactInstanceAndroidInternal::Destroy() {
	ThreadScope::WithClassLoader([this]() {
            m_jOptions.release();
            m_jReactInstance.release(); });
}

void ReactInstanceAndroid::Initialize() noexcept {
	m_internalState = Mso::Make<ReactInstanceAndroidInternal>();
	m_internalState->Initialize(m_options, Mso::WeakPtr(this));
}

void ReactInstanceAndroid::Finalize() noexcept {
	m_internalState->Destroy();
	m_internalState->Release();
}

//! This method must be called from the native queue.
Mso::Future<void> ReactInstanceAndroid::Destroy() noexcept {
	return Mso::MakeSucceededFuture();
}

const std::string &ReactInstanceAndroid::JsBundleName() const noexcept {
	return Options().Identity;
}

std::string ReactInstanceAndroid::SDXBasePath() const noexcept {
	return Options().SDXBasePath;
}

const ReactOptions &ReactInstanceAndroid::Options() const noexcept {
	// TODO
	return m_internalState->JOptions()->cthis()->Options();
}

void ReactInstanceAndroid::NotifyRekaInitialized() noexcept {
	// We want to make sure that notify about Reka initialization only once and only after the ReactInstance
	// is loaded and the Reka is initialized. Thus, this method is called when the ReactInstance
	// is successfully loaded and when the Reka is initialized because we do not know what comes first.
	if (!m_isRekaInitialized) {
		if (auto rekaContext = m_rekaContext.Load().Get()) {
			m_isRekaInitialized = true;

			if (auto onRekaInitialized = m_options.OnInitialized.Get()) {
				onRekaInitialized->Invoke(*rekaContext);
			}
		}
	}
}

void ReactInstanceAndroid::NotifyRekaDestroyed() noexcept {
	if (m_isRekaInitialized) {
		m_isRekaInitialized = false;

		if (auto rekaContext = m_rekaContext.Load().Get()) {
			if (auto onRekaDestroyed = m_options.OnDestroyed.Get()) {
				onRekaDestroyed->Invoke(*rekaContext);
			}
		}
	}
}

Mso::JSHost::RekaBridgeOptions ReactInstanceAndroid::createRekaBridgeOptions() noexcept {
	Mso::JSHost::RekaBridgeOptions rekaOptions{};
	const auto &reactOptions = Options();
	if (Options().RekaProviderFactory) {
		// Custom Provider Factory
		rekaOptions.RekaServiceProviderFactory = reactOptions.RekaProviderFactory;
	} else {
		// Default Provider Factory
		rekaOptions.RekaServiceProviderFactory = [rekaServiceProviderName = reactOptions.DataServiceProviderName](
		                                             Mso::JSHost::IRekaContext &context) mutable noexcept {
			return Mso::JSHost::GetOrMakeRekaServiceProvider(rekaServiceProviderName.c_str(),
			                                                 context);
		};
	}

	rekaOptions.OnInitialized = [weakThis = Mso::WeakPtr(this)](
	                                Mso::JSHost::IRekaContext &context) noexcept {
		if (auto strongThis = weakThis.GetStrongPtr()) {
			strongThis->m_rekaContext.Store(&context);
			strongThis->NotifyRekaInitialized();
		}
	};

	rekaOptions.OnDestroyed = [weakThis = Mso::WeakPtr(this)](
	                              Mso::JSHost::IRekaContext & /*context*/) noexcept {
		if (auto strongThis = weakThis.GetStrongPtr()) {
			strongThis->NotifyRekaDestroyed();
			strongThis->m_rekaContext.Exchange(nullptr);
		}
	};

	// Note:: We are configuring JsExecutor on native module initialization.
	//    rekaOptions.JsExecutor = Mso::Make<reactreka::ReactNativeRekaBridge, Mso::JSHost::IRekaBridge>(
	//            reactreka::ReactContextHolder:: m_jReactContextHolder /*reinterpret_cast ??*/);

#ifdef USE_OPENSOUCE_MSO
	rekaOptions.NativeQueue = Queue();
#else
	rekaOptions.NativeQueue = &Queue();
#endif

	return rekaOptions;
}

} // namespace

namespace Mso::React::Android::Deferred::Details {

facebook::jsi::Runtime *GetJsiRuntime(IReactInstance &instance) noexcept {
	ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
	return JReactInstance::GetJsiRuntime(instanceImpl->InternalState().JInstance());
}

std::shared_ptr<facebook::react::CallInvoker>
GetJSThreadCallInvoker(IReactInstance &instance) noexcept {
	ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
	return JReactInstance::getJSCallInvokerHolder(instanceImpl->InternalState().JInstance());
}

// TODO :: Handle exceptions
facebook::react::RuntimeExecutor
GetRuntimeExecutor(Mso::React::IReactInstance &instance) noexcept {
	ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
	return JReactInstance::GetRuntimeExecutor(instanceImpl->InternalState().JInstance());
}

JReactOptions_::JavaPart::javaobject ToJReactOptions(ReactOptions &&options) {
	return reinterpret_cast<JReactOptions_::JavaPart::javaobject>(JReactOptions::create(std::move(options)).release());
}

ReactOptions ToReactOptions(JReactOptions_::JavaPart::javaobject jReactOptions) {
	return reinterpret_cast<JReactOptions *>(wrap_alias<JReactOptions_::jhybridobject>(jReactOptions)->cthis())->Options();
}

JReactHost_::JavaPart::javaobject ToJReactHost(Mso::TCntPtr<IReactHost> reactHost) {
	return reinterpret_cast<JReactHost_::JavaPart::javaobject>(JReactHost::create(reactHost).release());
}

namespace {
void ensureInitialized() {
	ThreadScope::WithClassLoader([]() {
                auto cls = findClassStatic("com/microsoft/office/reacthost/ReactHostStatics");
                static const auto instanceFieldId = cls->getStaticField<JReactHostStatics::javaobject>("INSTANCE");
                auto reactHostStaticsInstance = cls->getStaticFieldValue(instanceFieldId);
                static const auto ensureInitializedMethodId = cls->getMethod<jboolean()>("ensureInitialized");
                VerifyElseCrashTag(ensureInitializedMethodId(reactHostStaticsInstance), 0); });
}
} // namespace

/** NOTE: This method is resolved at runtime from ReactNativeHost/ReactInstanceAndroidDeferred.cpp */
Mso::TCntPtr<IReactInstanceInternal>
MakeReactInstance(IReactHost &reactHost, ReactOptions &&options, Mso::JSHost::IRekaContextProxy & /*rekaContextProxy*/, Mso::Promise<void> && /*whenLoaded*/) noexcept {
	ensureInitialized();
	return Mso::Make<ReactInstanceAndroid, IReactInstanceInternal>(reactHost, std::move(options));
}

}; // namespace Mso::React::Android::Deferred::Details

extern "C" jint JNI_OnLoad(JavaVM *vm, void *) {
	return jni::initialize(vm, [] {
        JReactOptions::registerNatives();
        JReactDevOptions::registerNatives();
        JReactViewOptions::registerNatives();
        JReactHost::registerNatives();
//#ifdef USE_OPENSOUCE_MSO
//       JReactHostStatics::registerNatives();
//#endif
        JReactInstance::registerNatives();
        JReactViewHost::registerNatives();
        JMsoFuture::registerNatives();
        JBaseRootView::registerNatives(); });
}
