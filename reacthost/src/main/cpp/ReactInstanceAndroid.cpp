#include "precomp.h"

#include <ReactNativeHost/JSBundle.h>
#include <ReactNativeHost/React.h>
#include <ReactNativeHost/React_Android.h>

// For ReactHostRegistry
#include <ReactHost.h>

#include <errorCode/hresultErrorProvider.h>
#include "ReactInstanceAndroid.h"

#include <JRekaBridgeOptions.h>

#include <ReactCommon/RuntimeExecutor.h>
#include <react/jni/JRuntimeExecutor.h>
#include <ReactCommon/CallInvokerHolder.h>

#include <fbjni/fbjni.h>
#include <fbjni/ByteBuffer.h>
#include <smartPtr/cntPtr.h>

#include <dlfcn.h>

using namespace Mso::React;
using namespace facebook;
using namespace facebook::jni;

namespace {


struct JMsoFuture : facebook::jni::HybridClass<JMsoFuture> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/MsoFuturePeer;";
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create();
    Mso::Future<void> getMsoFuture();
    void Set();

    JMsoFuture()
            : promise_(Mso::Promise<void>()) {}

    Mso::Promise<void> promise_;
};



/*static*/ void JMsoFuture::registerNatives() {
    registerHybrid({
                           makeNativeMethod("Set", JMsoFuture::Set)
                   });
}

void JMsoFuture::Set() {
    promise_.SetValue();
}

/*static*/ facebook::jni::local_ref<JMsoFuture::jhybridobject> JMsoFuture::create() {
    return newObjectCxxArgs();
}

Mso::Future<void> JMsoFuture::getMsoFuture() {
    return promise_.AsFuture();
}



struct JReactDevOptions : facebook::jni::HybridClass<JReactDevOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactDevOptions;";

    static void registerNatives();

    JReactDevOptions(Mso::React::ReactOptions &options) :
            options_(options) {}

    static facebook::jni::local_ref<jhybridobject> create(Mso::React::ReactOptions &);

    Mso::React::ReactOptions &options_;

    bool getIsDevModeEnabled();

    void setIsDevModeEnabled(bool isDevModeEnabled);

    std::string getSourceBundleName();

    void setSourceBundleName(std::string sourceBundleName);
};

struct JJSBundleInfo : facebook::jni::JavaClass<JJSBundleInfo> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundleInfo;";

    static Mso::React::JSBundleInfo get(facebook::jni::alias_ref<JJSBundleInfo> thizz);

    static facebook::jni::local_ref<JJSBundleInfo> create(Mso::React::JSBundleInfo info);
};

// Note :: This could be a hybrid object instead. But keeping it simple for now, and copying the object fields across the JNI boundary except the bundle shared buffer
struct JJSBundle : facebook::jni::JavaClass<JJSBundle> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/JSBundle;";

    static Mso::CntPtr<Mso::React::IJSBundle> get(facebook::jni::alias_ref<JJSBundle> thizz);

    static facebook::jni::local_ref<JJSBundle> create(Mso::React::IJSBundle &bundle);
};

struct JInstanceCreatedCallback;
struct JInstanceLoadedCallback;

struct JReactOptions : facebook::jni::HybridClass<JReactOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactOptions;";

    static facebook::jni::local_ref<jhybriddata>
    initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);

    static void registerNatives();

    JReactOptions(const Mso::React::ReactOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(const Mso::React::ReactOptions &&);

    Mso::React::ReactOptions options_;

    const Mso::React::ReactOptions &Options() const noexcept;

    std::string getIdentity();

    void setIdentity(std::string identity);

    facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>>
    getJavaModuleNames();

    void setJavaModuleNames(
            facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>>);

    void addJavaModuleName(std::string javaModuleName);

    std::string getDataServiceProviderName();

    void setDataServiceProviderName(std::string dataServiceProviderName);

    facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> getJSBundles();

    void setJSBundles(facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>>);

    void addJSBundle(facebook::jni::alias_ref<JJSBundle>);

    void setInstanceCreatedCallback(facebook::jni::alias_ref<JInstanceCreatedCallback>);

    facebook::jni::alias_ref<JInstanceCreatedCallback> getInstanceCreatedCallback();

    void setInstanceLoadedCallback(facebook::jni::alias_ref<JInstanceLoadedCallback>);

    facebook::jni::alias_ref<JInstanceLoadedCallback> getInstanceLoadedCallback();

    facebook::jni::alias_ref<JReactDevOptions::jhybridobject> createDeveloperSettingsPeer();
};

struct JErrorCode : facebook::jni::HybridClass<JErrorCode> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ErrorCode;";

    static facebook::jni::local_ref<jhybriddata>
    initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);

    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::ErrorCode &&);

    std::string toString();

    JErrorCode(Mso::ErrorCode &&errorCode) : errorCode_(std::move(errorCode)) {}

    Mso::ErrorCode errorCode_;
};

/*static*/ local_ref<JErrorCode::jhybriddata> JErrorCode::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::ErrorCode errorCode;
    return makeCxxInstance(std::move(errorCode));
}

std::string JErrorCode::toString() {
    return errorCode_.ToString();
}

/*static */facebook::jni::local_ref<JErrorCode::jhybridobject> JErrorCode::create(Mso::ErrorCode&& errorCode) {
    return newObjectCxxArgs(std::move(errorCode));
}

/*static*/ void JErrorCode::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JErrorCode::initHybrid),
                           makeNativeMethod("toString", JErrorCode::toString),
                   });
}

struct JReactContextHolder : public facebook::jni::JavaClass<JReactContextHolder> {
    constexpr static auto kJavaDescriptor =
            "Lcom/microsoft/office/reacthost/ReactContextHolder;";
};

struct JReactInstance  : facebook::jni::HybridClass<JReactInstance> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactInstance;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static void onInitialized(facebook::jni::alias_ref<jhybridobject> jThis/*, facebook::jni::alias_ref<JReactContextHolder> contextHolder*/);
    static void onBundleLoaded(facebook::jni::alias_ref<jhybridobject> jThis, facebook::jni::alias_ref<JString> bundleName);
    static facebook::jni::local_ref<reactreka::JRekaBridgeOptions::jhybridobject> createRekaBridgeOptions(facebook::jni::alias_ref<jhybridobject> jThis);

    static facebook::jni::local_ref<jhybridobject> create(facebook::jni::alias_ref<JReactOptions::jhybridobject>, Mso::CntPtr<ReactInstanceAndroid> nativeInstance);
    static facebook::react::RuntimeExecutor GetRuntimeExecutor(facebook::jni::alias_ref<JReactInstance::javaobject>);
    static facebook::jsi::Runtime* GetJsiRuntime(facebook::jni::alias_ref<JReactInstance::javaobject> instance);
    static std::shared_ptr<facebook::react::CallInvoker> getJSCallInvokerHolder(facebook::jni::alias_ref<JReactInstance::javaobject> instance);

    Mso::WeakPtr<ReactInstanceAndroid> m_wNativeInstance;
};

}

namespace Mso::React {

struct ReactInstanceAndroidInternal : public Mso::RefCountedObject<Mso::IRefCounted> {
    void Initialize(ReactOptions optionsCopy, Mso::WeakPtr<ReactInstanceAndroid>);
    void Destroy();

    facebook::jni::global_ref<JReactOptions::jhybridobject> m_jOptions;
    facebook::jni::global_ref<JReactInstance::jhybridobject> m_jReactInstance;
    facebook::jni::global_ref<JReactContextHolder> m_jReactContextHolder;
};

}

namespace {

struct JInstanceCreatedCallback : facebook::jni::JavaClass<JInstanceCreatedCallback> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceCreatedCallback;";
    static void run(facebook::jni::alias_ref<JInstanceCreatedCallback> callback, facebook::jni::alias_ref<JReactInstance::jhybridobject>);
};

struct JInstanceLoadedCallback : facebook::jni::JavaClass<JInstanceLoadedCallback> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/IInstanceLoadedCallback;";
    static void run(facebook::jni::alias_ref<JInstanceLoadedCallback> callback,
                    facebook::jni::alias_ref<JReactInstance::jhybridobject>,
                    facebook::jni::alias_ref<JErrorCode::jhybridobject>);
};


/*static */void JInstanceCreatedCallback::run(facebook::jni::alias_ref<JInstanceCreatedCallback> callback, facebook::jni::alias_ref<JReactInstance::jhybridobject> instance) {
    static auto clazz = javaClassStatic();
    auto method = clazz->getMethod<void(facebook::jni::alias_ref<JReactInstance::jhybridobject>)>("run");
    method(callback, instance);
}



/*static */void JInstanceLoadedCallback::run(facebook::jni::alias_ref<JInstanceLoadedCallback> callback,
                                             facebook::jni::alias_ref<JReactInstance::jhybridobject> instance,
                                             facebook::jni::alias_ref<JErrorCode::jhybridobject> errorCode) {
    static auto clazz = javaClassStatic();
    auto method = clazz->getMethod<void(facebook::jni::alias_ref<JReactInstance::jhybridobject>, facebook::jni::alias_ref<JErrorCode::jhybridobject>)>("run");
    method(callback, instance, errorCode);
}




    struct ByteBufferJSBundle : public ::Mso::UnknownObject<IJSBundle>
{
public:
    std::string_view Content() noexcept override;
    JSBundleInfo Info() noexcept override;

private:
    friend MakePolicy; // Allows Mso::Make to access private constructor
    ByteBufferJSBundle(JSBundleInfo&& info, local_ref<JByteBuffer>) noexcept;
    uint64_t Timestamp() noexcept;

private:
    JSBundleInfo m_info;
    global_ref<JByteBuffer> m_byteBuffer; // Owns the java ByteBuffer

};

struct InfoJSBundle : public ::Mso::UnknownObject<IJSBundle>
{
public:
    std::string_view Content() noexcept override;
    JSBundleInfo Info() noexcept override;

private:
    friend MakePolicy; // Allows Mso::Make to access private constructor
    InfoJSBundle(JSBundleInfo&& info) noexcept;
    uint64_t Timestamp() noexcept;

private:
    JSBundleInfo m_info;
};

ByteBufferJSBundle::ByteBufferJSBundle(
        JSBundleInfo&& info,
        local_ref<JByteBuffer> content) noexcept
        : m_info(std::move(info))
        , m_byteBuffer(make_global(content))
{}

InfoJSBundle::InfoJSBundle(
        JSBundleInfo&& info) noexcept
        : m_info(std::move(info))
{}

std::string_view InfoJSBundle::Content() noexcept  {
    return std::string_view();
}

JSBundleInfo InfoJSBundle::Info() noexcept  {
    return m_info;
}

std::string_view ByteBufferJSBundle::Content() noexcept  {
    size_t size = m_byteBuffer->getDirectSize();
    const char* bytes = reinterpret_cast<const char*>(m_byteBuffer->getDirectBytes());
    return std::string_view(bytes, size);
}

JSBundleInfo ByteBufferJSBundle::Info() noexcept  {
    return m_info;
}

/*static */::Mso::CntPtr<::Mso::React::IJSBundle> JJSBundle::get(alias_ref<JJSBundle> thizz) {
    const auto cls = javaClassStatic();
    const auto iFieldContent = cls->getField<JByteBuffer>("Content");
    local_ref<JByteBuffer> content = make_local(thizz->getFieldValue(iFieldContent));
    const auto iFieldInfo = cls->getField<JJSBundleInfo>("Info");
    local_ref<JJSBundleInfo> info = make_local(thizz->getFieldValue(iFieldInfo));

    ::Mso::CntPtr<::Mso::React::IJSBundle>jsBundle;
    if(content) {
        jsBundle = ::Mso::Make<ByteBufferJSBundle>( std::move(JJSBundleInfo::get(info)), content);
    } else {
        jsBundle = ::Mso::Make<InfoJSBundle>( std::move(JJSBundleInfo::get(info)));
    }
    return jsBundle;
}

/*static */::Mso::React::JSBundleInfo JJSBundleInfo::get(facebook::jni::alias_ref<JJSBundleInfo> thizz) {
    ::Mso::React::JSBundleInfo info;
    const auto cls = javaClassStatic();
    const auto iFieldId = cls->getField<JString>("Id");
    auto idValue = thizz->getFieldValue(iFieldId);
    if(idValue) {
        info.Id = idValue->toStdString();
    }

    const auto iFieldFileName = cls->getField<JString>("FileName");
    auto fileNameValue = thizz->getFieldValue(iFieldFileName);
    if(fileNameValue) {
        info.FileName = fileNameValue->toStdString();
    }

    const auto iFieldTimestamp = cls->getField<JLong>("Timestamp");
    auto timestampValue = thizz->getFieldValue(iFieldTimestamp);
    if(timestampValue) {
        info.Timestamp = timestampValue->value();
    }

    return info;
}

/*static */facebook::jni::local_ref<JJSBundleInfo> JJSBundleInfo::create(::Mso::React::JSBundleInfo info) {
    return newInstance(make_jstring(info.Id), make_jstring(info.FileName), JLong::valueOf(info.Timestamp));
}

/*static */facebook::jni::local_ref<JJSBundle> JJSBundle::create(::Mso::React::IJSBundle& bundle) {
    local_ref<facebook::jni::JByteBuffer> content;
    if(!bundle.Content().empty()) {
        content = facebook::jni::JByteBuffer::wrapBytes(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(bundle.Content().data())), bundle.Content().size());
    }

    return newInstance(content, JJSBundleInfo::create(bundle.Info()));
}


    local_ref<JReactOptions::jhybridobject> JReactOptions::create(const Mso::React::ReactOptions&& options) {
        return newObjectCxxArgs(std::move(options));
    }

/*static*/ local_ref<JReactOptions::jhybriddata> JReactOptions::initHybrid(alias_ref<jhybridobject> jThis){
        Mso::React::ReactOptions options;
        return makeCxxInstance(std::move(options));
    }

    std::string JReactOptions::getIdentity() {
        return options_.Identity;
    }

    void JReactOptions::setIdentity(std::string identity) {
        options_.Identity = identity;
    }

    facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>> JReactOptions::getJavaModuleNames() {
        auto jJavaModuleNames = JArrayList<JString>::create();
        std::for_each(options_.JavaModuleNames.begin(), options_.JavaModuleNames.end(), [&jJavaModuleNames](std::string item) {
            jJavaModuleNames->add(make_jstring(item));
        } );
        return jJavaModuleNames.release();
    }

    void JReactOptions::setJavaModuleNames(facebook::jni::alias_ref<facebook::jni::JArrayList<facebook::jni::JString>> jJavaModuleNames) {
        options_.JavaModuleNames.clear();
        for (const auto& elem : *jJavaModuleNames) {
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

    facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> JReactOptions::getJSBundles() {
        auto jJSBundles = facebook::jni::JArrayList<JJSBundle>::create(options_.JSBundles.size());
        for (auto&jsBundle: options_.JSBundles) {
            jJSBundles->add(JJSBundle::create(*jsBundle));
        }

        // TODO :: Is this the right pattern to return ? What is releaseAlias ?
        return jJSBundles.release();
    }

// Note :: This clears the existing bundles.
    void JReactOptions::setJSBundles(facebook::jni::alias_ref<facebook::jni::JArrayList<JJSBundle>> jsBundles) {
        options_.JSBundles.clear();
        for (const auto& elem : *jsBundles) {
            options_.JSBundles.push_back(JJSBundle::get(make_local(elem)));
        }
    }

    void JReactOptions::addJSBundle(facebook::jni::alias_ref<JJSBundle> jsBundle) {
        options_.JSBundles.push_back(JJSBundle::get(make_local(jsBundle)));
    }

    void JReactOptions::setInstanceCreatedCallback(facebook::jni::alias_ref<JInstanceCreatedCallback> callback) {
        options_.OnInstanceCreated = [callback = make_global(callback)](IReactInstance& instance){
            // TODO :: Make it safer
            ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(&instance);
            JInstanceCreatedCallback::run(callback, instanceImpl->m_internalState->m_jReactInstance);
        };
    }

    facebook::jni::alias_ref<JInstanceCreatedCallback> JReactOptions::getInstanceCreatedCallback() {

    }

    void JReactOptions::setInstanceLoadedCallback(facebook::jni::alias_ref<JInstanceLoadedCallback> callback) {
        options_.OnInstanceLoaded = [callback = make_global(callback)](IReactInstance& instance, const Mso::ErrorCode& errorCode){
            // TODO :: Make it safer
            ReactInstanceAndroid* instanceImpl = static_cast<ReactInstanceAndroid*>(&instance);
            ::Mso::ErrorCode errorCodeCopy = errorCode;
            JInstanceLoadedCallback::run(callback, instanceImpl->m_internalState->m_jReactInstance, JErrorCode::create(std::move(errorCodeCopy)));
        };
    }

    facebook::jni::alias_ref<JInstanceLoadedCallback> JReactOptions::getInstanceLoadedCallback() {

    }

    const ReactOptions& JReactOptions::Options() const noexcept
    {
        return options_;
    }

    facebook::jni::alias_ref<JReactDevOptions::jhybridobject> JReactOptions::createDeveloperSettingsPeer() {
        return JReactDevOptions::create(options_).release();
    }

/*static*/ facebook::jni::local_ref<JReactDevOptions::jhybridobject> JReactDevOptions::create(Mso::React::ReactOptions& options) {
        return JReactDevOptions::newObjectCxxArgs(options);
    }

    bool JReactDevOptions::getIsDevModeEnabled() {
        return options_.DeveloperSettings.IsDevModeEnabled;
    }

    void JReactDevOptions::setIsDevModeEnabled(bool isDevModeEnabled) {
        options_.DeveloperSettings.IsDevModeEnabled = isDevModeEnabled;
    }

    std::string JReactDevOptions::getSourceBundleName() {
        return options_.DeveloperSettings.SourceBundleName;
    }

    void JReactDevOptions::setSourceBundleName(std::string sourceBundleName) {
        options_.DeveloperSettings.SourceBundleName = sourceBundleName;
    }

/*static*/ void JReactDevOptions::registerNatives() {
    registerHybrid({
                           makeNativeMethod("getIsDevModeEnabled", JReactDevOptions::getIsDevModeEnabled),
                           makeNativeMethod("setIsDevModeEnabled", JReactDevOptions::setIsDevModeEnabled),
                           makeNativeMethod("getSourceBundleName", JReactDevOptions::getSourceBundleName),
                           makeNativeMethod("setSourceBundleName", JReactDevOptions::setSourceBundleName)
                   });
    }

/*static*/ void JReactOptions::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JReactOptions::initHybrid),
                           makeNativeMethod("getIdentity", JReactOptions::getIdentity),
                           makeNativeMethod("setIdentity", JReactOptions::setIdentity),
                           makeNativeMethod("getJavaModuleNames", JReactOptions::getJavaModuleNames),
                           makeNativeMethod("setJavaModuleNames", JReactOptions::setJavaModuleNames),
                           makeNativeMethod("AddJavaModuleName", JReactOptions::addJavaModuleName),
                           makeNativeMethod("getDataServiceProviderName", JReactOptions::getDataServiceProviderName),
                           makeNativeMethod("setDataServiceProviderName", JReactOptions::setDataServiceProviderName),
                           makeNativeMethod("getJSBundles", JReactOptions::getJSBundles),
                           makeNativeMethod("setJSBundles", JReactOptions::setJSBundles),
                           makeNativeMethod("AddJSBundle", JReactOptions::addJSBundle),
                           makeNativeMethod("getInstanceCreatedCallback", JReactOptions::getInstanceCreatedCallback),
                           makeNativeMethod("setInstanceCreatedCallback", JReactOptions::setInstanceCreatedCallback),
                           makeNativeMethod("getInstanceLoadedCallback", JReactOptions::getInstanceLoadedCallback),
                           makeNativeMethod("setInstanceLoadedCallback", JReactOptions::setInstanceLoadedCallback),
                           makeNativeMethod("createDeveloperSettingsPeer", JReactOptions::createDeveloperSettingsPeer)
                   });
}


/*static */facebook::jni::local_ref<JReactInstance::jhybriddata> JReactInstance::initHybrid(facebook::jni::alias_ref<jhybridobject> jThis) {
        return makeCxxInstance();
    }

/*static */void JReactInstance::onInitialized(facebook::jni::alias_ref<jhybridobject> jThis/*, facebook::jni::alias_ref<JReactContextHolder> contextHolder*/) {

        auto nativeInstance= jThis->cthis()->m_wNativeInstance.GetStrongPtr();
        if(nativeInstance) {
            nativeInstance->onInitialized(/*contextHolder*/);
        }
    }

/*static*/ void JReactInstance::onBundleLoaded(facebook::jni::alias_ref<jhybridobject> jThis, facebook::jni::alias_ref<JString> bundleName) {
        auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
        if(nativeInstance) {
            nativeInstance->onBundleLoaded(bundleName->toStdString());
        }
    }

/*static */facebook::jni::local_ref<reactreka::JRekaBridgeOptions::jhybridobject> JReactInstance::createRekaBridgeOptions(facebook::jni::alias_ref<jhybridobject> jThis) {


        auto rnhandle = dlopen("libreactreka.so", RTLD_NOW | RTLD_GLOBAL);
        auto method = (facebook::jni::local_ref<reactreka::JRekaBridgeOptions::jhybridobject> (*)(Mso::JSHost::RekaBridgeOptions &&rekaOptions))(dlsym(rnhandle, "_ZN9reactreka18JRekaBridgeOptions6createEON3Mso6JSHost17RekaBridgeOptionsE"));
        // return reactreka::JRekaBridgeOptions::create(jThis->cthis()->m_wNativeInstance.GetStrongPtr()->createRekaBridgeOptions());
        auto nativeInstance = jThis->cthis()->m_wNativeInstance.GetStrongPtr();
        VerifyElseCrash(nativeInstance);
        return method(nativeInstance->createRekaBridgeOptions());
}

/*static */void JReactInstance::registerNatives() {
        registerHybrid({
                               makeNativeMethod("initHybrid", JReactInstance::initHybrid),
                               makeNativeMethod("onInitialized", JReactInstance::onInitialized),
                               makeNativeMethod("onBundleLoaded", JReactInstance::onBundleLoaded),
                               makeNativeMethod("createRekaBridgeOptions", JReactInstance::createRekaBridgeOptions),
                       });
    }

/*static*/ facebook::jni::local_ref<JReactInstance::jhybridobject> JReactInstance::create(facebook::jni::alias_ref<JReactOptions::jhybridobject> options, Mso::CntPtr<ReactInstanceAndroid> nativeInstance) {

        // Create the java peer.
        auto jInstance = JReactInstance::newObjectJavaArgs(options);

        // Associate the native peer with the native instance.
        jInstance->cthis()->m_wNativeInstance=Mso::WeakPtr(nativeInstance);

        // Initialize the instance
        auto clazz = javaClassLocal();
        auto initializeMethod = clazz->getMethod<void()>("initialize");
        initializeMethod(jInstance);

        return jInstance;
    }

// TODO:: Null check
/*static */facebook::react::RuntimeExecutor JReactInstance::GetRuntimeExecutor(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
        auto getRuntimeExecutorMethod = javaClassLocal()->getMethod<facebook::react::JRuntimeExecutor::jhybridobject ()>("getRuntimeExecutor");
        local_ref<facebook::react::JRuntimeExecutor::jhybridobject> jRuntimeExecutor = make_local(getRuntimeExecutorMethod(instance));



        auto rnhandle = dlopen("libreactnativejni.so", RTLD_NOW | RTLD_GLOBAL);
        auto method = (facebook::react::RuntimeExecutor (*)(facebook::react::JRuntimeExecutor*))(dlsym(rnhandle, "_ZN8facebook5react16JRuntimeExecutor3getEv"));
        auto runtimeExecutor = method(jRuntimeExecutor->cthis());


        return runtimeExecutor;
    }

// TODO:: Null check
/*static */facebook::jsi::Runtime* JReactInstance::GetJsiRuntime(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
        auto getJsiRuntimeMethod = javaClassLocal()->getMethod<jlong()>("getJsiRuntimeRef");
        long jsiRuntimeRef = getJsiRuntimeMethod(instance);
        return reinterpret_cast<facebook::jsi::Runtime*>(jsiRuntimeRef);
    }

// TODO:: Null check`
/*static */std::shared_ptr<facebook::react::CallInvoker> JReactInstance::getJSCallInvokerHolder(facebook::jni::alias_ref<JReactInstance::javaobject> instance) {
        auto getJSCallInvokerHolderMethod = javaClassLocal()->getMethod<facebook::react::CallInvokerHolder::jhybridobject()>("getJSCallInvokerHolder");
        auto jsCallInvokerHolder = getJSCallInvokerHolderMethod(instance);

        auto rnhandle = dlopen("libreactnativejni.so", RTLD_NOW | RTLD_GLOBAL);
        auto method = (std::shared_ptr<facebook::react::CallInvoker> (*)(facebook::react::CallInvokerHolder*))(dlsym(rnhandle, "_ZN8facebook5react17CallInvokerHolder14getCallInvokerEv"));
        std::shared_ptr<facebook::react::CallInvoker> callInvoker = method(jsCallInvokerHolder->cthis());

        return callInvoker;
        // return jsCallInvokerHolder->cthis()->getCallInvoker();
    }




struct JReactViewOptions : facebook::jni::HybridClass<JReactViewOptions> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewOptions;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    JReactViewOptions(Mso::React::ReactViewOptions &&options) :
            options_(std::move(options)) {}

    static facebook::jni::local_ref<jhybridobject> create(const Mso::React::ReactViewOptions&&);
    Mso::React::ReactViewOptions options_;

    std::string getComponentName();
    void setComponentName(std::string componentName);

    std::string getInitialProps();
    void setInitialProps(std::string initialProps);

    const Mso::React::ReactViewOptions& Options() const noexcept;

};




local_ref<JReactViewOptions::jhybridobject> JReactViewOptions::create(const Mso::React::ReactViewOptions&& options) {
    return newObjectCxxArgs(std::move(const_cast<Mso::React::ReactViewOptions&&>(options)));
}

/*static*/ local_ref<JReactViewOptions::jhybriddata> JReactViewOptions::initHybrid(alias_ref<jhybridobject> jThis){
    Mso::React::ReactViewOptions options;
    return makeCxxInstance(std::move(options));
}

const Mso::React::ReactViewOptions& JReactViewOptions::Options() const noexcept {
    return options_;
}

std::string JReactViewOptions::getComponentName(){
    return options_.ComponentName;
}

void JReactViewOptions::setComponentName(std::string componentName){
    options_.ComponentName = componentName;
}

std::string JReactViewOptions::getInitialProps() {
    return options_.InitialProps;
}

void JReactViewOptions::setInitialProps(std::string initialProps) {
    options_.InitialProps = initialProps;
}

/*static*/ void JReactViewOptions::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JReactViewOptions::initHybrid),
                           makeNativeMethod("getComponentName", JReactViewOptions::getComponentName),
                           makeNativeMethod("setComponentName", JReactViewOptions::setComponentName),
                           makeNativeMethod("getInitialProps", JReactViewOptions::getInitialProps),
                           makeNativeMethod("setInitialProps", JReactViewOptions::setInitialProps)
                   });
}





struct OfficeReactViewInstance;
struct JOfficeReactRootView : facebook::jni::HybridClass<JOfficeReactRootView> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/OfficeReactRootView;";
    static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    Mso::Future<void> Reload(const Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance, const Mso::React::ReactViewOptions&& viewOptions) noexcept;
    Mso::Future<void> Unload() noexcept;

    JOfficeReactRootView(facebook::jni::alias_ref<jhybridobject> jThis);

    OfficeReactViewInstance& ViewInstance() { return *m_ReactViewInstance; }

    Mso::CntPtr<OfficeReactViewInstance> m_ReactViewInstance;
    facebook::jni::weak_ref<JOfficeReactRootView::jhybridobject> m_jThis;
};

struct OfficeReactViewInstance : public Mso::UnknownObject<Mso::React::IReactViewInstance> {
    virtual Mso::Future<void> Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                     Mso::React::ReactViewOptions&& viewOptions) noexcept override;
    virtual Mso::Future<void> Unload() noexcept override;

    OfficeReactViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jThis)
            :m_jThis(facebook::jni::make_weak(std::move(jThis))) {}
    facebook::jni::weak_ref<JOfficeReactRootView::jhybridobject> m_jThis;
};





void JOfficeReactRootView::registerNatives() {
    registerHybrid({
                           makeNativeMethod("initHybrid", JOfficeReactRootView::initHybrid),
                   });
}

/*static*/ local_ref<JOfficeReactRootView::jhybriddata> JOfficeReactRootView::initHybrid(alias_ref<jhybridobject> jThis){
    return makeCxxInstance(std::move(jThis));
}

JOfficeReactRootView::JOfficeReactRootView(alias_ref<jhybridobject> jThis) {
    m_jThis = make_weak(std::move(jThis));
    m_ReactViewInstance = Mso::Make<OfficeReactViewInstance>(std::move(jThis));
}

Mso::Future<void> JOfficeReactRootView::Reload(const Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance, const Mso::React::ReactViewOptions&& viewOptions) noexcept {
    auto future = JMsoFuture::create();
    auto msoFuture = future->cthis()->getMsoFuture();

    auto reloadMethod = javaClassLocal()->getMethod<void(JReactInstance::jhybridobject, JReactViewOptions::jhybridobject, JMsoFuture::jhybridobject)>("Reload");
    reloadMethod(m_jThis.lockLocal(), static_cast<ReactInstanceAndroid*>(reactInstance.Get())->m_internalState->m_jReactInstance.get(), JReactViewOptions::create(std::move(viewOptions)).release(), future.release());
    return msoFuture;
}

Mso::Future<void> JOfficeReactRootView::Unload() noexcept {
    auto future = JMsoFuture::create();
    auto msoFuture = future->cthis()->getMsoFuture();

    auto unloadMethod = javaClassLocal()->getMethod<void(JMsoFuture::jhybridobject)>("Unload");
    unloadMethod(m_jThis.lockLocal(), future.release());
    return msoFuture;
}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Reload(Mso::CntPtr<Mso::React::IReactInstance>&& reactInstance,
                                                          Mso::React::ReactViewOptions&& viewOptions) noexcept {

    Mso::Future<void> result;
    ThreadScope::WithClassLoader([this, &result, reactInstance, viewOptions2=std::move(viewOptions)]() {
        auto local = m_jThis.lockLocal();
        result = local->cthis()->Reload(std::move(reactInstance), std::move(viewOptions2));
    });

    return result;
}

/*virtual*/ Mso::Future<void> OfficeReactViewInstance::Unload() noexcept {
    Mso::Future<void> result;
    ThreadScope::WithClassLoader([this, &result]() {
        auto local = m_jThis.lockLocal();
        result = local->cthis()->Unload();

    });

    return result;
}







    struct JReactViewHost : facebook::jni::HybridClass<JReactViewHost>  {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactViewHost;";
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::CntPtr<Mso::React::IReactViewHost> viewHost);

    JReactViewHost(Mso::CntPtr<Mso::React::IReactViewHost> &&viewHost)
            : viewHost_(std::move(viewHost)) {}

    Mso::CntPtr<Mso::React::IReactViewHost> viewHost_;

    void AttachViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jView);
};

void JReactViewHost::registerNatives() {
    registerHybrid({
                           makeNativeMethod("AttachViewInstance", JReactViewHost::AttachViewInstance)
                   });
}

void JReactViewHost::AttachViewInstance(facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> jView) {
    viewHost_->AttachViewInstance(jView->cthis()->ViewInstance());
}

/*static*/ facebook::jni::local_ref<JReactViewHost::jhybridobject> JReactViewHost::create(Mso::CntPtr<Mso::React::IReactViewHost> viewHost) {
    return newObjectCxxArgs(std::move(viewHost));
}

struct JReactHost : facebook::jni::HybridClass<JReactHost> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHost;";
    // static facebook::jni::local_ref<jhybriddata> initHybrid(facebook::jni::alias_ref<jhybridobject> jThis);
    static void registerNatives();

    static facebook::jni::local_ref<jhybridobject> create(Mso::CntPtr<Mso::React::IReactHost> host);

    JReactHost(Mso::CntPtr<Mso::React::IReactHost> &&host)
            : host_(std::move(host)) {}

    ~JReactHost() {
        FBJNI_LOGE("~JReactHost");
    };

    facebook::jni::alias_ref<JReactViewHost::jhybridobject> MakeViewHost(facebook::jni::alias_ref<JReactViewOptions::jhybridobject> jOptions);
    void ReloadInstance();
    void ReloadInstanceWithOptions(facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
    void UnloadInstance();

    Mso::CntPtr<Mso::React::IReactHost> host_;
};



void JReactHost::registerNatives() {
    registerHybrid({
                           makeNativeMethod("MakeViewHost", JReactHost::MakeViewHost),
                           makeNativeMethod("ReloadInstance", JReactHost::ReloadInstance),
                           makeNativeMethod("ReloadInstanceWithOptions", JReactHost::ReloadInstanceWithOptions),
                           makeNativeMethod("UnloadInstance", JReactHost::UnloadInstance)
                   });
}

alias_ref<JReactViewHost::jhybridobject> JReactHost::MakeViewHost(alias_ref<JReactViewOptions::jhybridobject> jOptions) {
    ReactViewOptions viewOptions = jOptions->cthis()->Options();
    return JReactViewHost::create(host_->MakeViewHost(std::move(viewOptions))).release();
}

void JReactHost::ReloadInstance() {
    host_->ReloadInstance();
}

void JReactHost::ReloadInstanceWithOptions(facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
    auto options = jOptions->cthis()->Options();
    host_->ReloadInstanceWithOptions(std::move(options));
}

void JReactHost::UnloadInstance() {
    host_->UnloadInstance();
}

/*static*/ facebook::jni::local_ref<JReactHost::jhybridobject> JReactHost::create(Mso::CntPtr<Mso::React::IReactHost> host) {
    return newObjectCxxArgs(std::move(host));
}


struct JReactHostStatics : facebook::jni::JavaClass<JReactHostStatics>  {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reacthost/ReactHostStatics;";
    static void registerNatives();

    static facebook::jni::alias_ref<JReactHost::jhybridobject> makeJReactHost(facebook::jni::alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
    static void libletInit(facebook::jni::alias_ref<jclass>);

    // static facebook::jni::local_ref<JReactHost::jhybridobject> test(facebook::jni::alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions);
};


/*static*/ facebook::jni::alias_ref<JReactHost::jhybridobject> JReactHostStatics::makeJReactHost(alias_ref<jclass>, facebook::jni::alias_ref<JReactOptions::jhybridobject> jOptions) {
    auto options = jOptions->cthis()->Options();
    Mso::CntPtr<IReactHost> reactHost = MakeReactHost(std::move(options));
    return JReactHost::create(reactHost).release();
}

/*static*/ void JReactHostStatics::libletInit(alias_ref<jclass>) {
    ReactHostRegistry::OnLibletInit();
}

/*static*/ void JReactHostStatics::registerNatives() {
    javaClassLocal()->registerNatives({
                                              makeNativeMethod("makeReactHost", JReactHostStatics::makeJReactHost),
                                              makeNativeMethod("libletInit", JReactHostStatics::libletInit),
                                      });
}

}


namespace Mso::React {

ReactInstanceAndroid::ReactInstanceAndroid(IReactHost& reactHost, ReactOptions&& options) noexcept
        : Super{reactHost.NativeQueue()}
        , m_weakReactHost(&reactHost)
        , m_options(std::move(options))
{
}

ReactInstanceAndroid::~ReactInstanceAndroid() noexcept
{
    // Destroy();
}

void ReactInstanceAndroid::onInitialized(/*facebook::jni::alias_ref<JReactContextHolder> contextHolder*/) noexcept {
    // m_jReactContextHolder = make_global(contextHolder);
    if(Options().OnInstanceCreated)
        Options().OnInstanceCreated(*this);
}

void ReactInstanceAndroid::onBundleLoaded(std::string&& /*bundleName*/) noexcept {
    if(Options().OnInstanceLoaded)
        Options().OnInstanceLoaded(*this, Mso::HResultErrorProvider().MakeErrorCode(S_OK));
}

void ReactInstanceAndroidInternal::Initialize(ReactOptions optionsCopy, Mso::WeakPtr<ReactInstanceAndroid> instanceWeak) {
    auto instance = instanceWeak.GetStrongPtr();
    VerifyElseCrash(instance);

    // Note: This is required as the AcitiveObject thread is a raw pthread thread which don't have a Java stack in it yet.
    // https://developer.android.com/training/articles/perf-jni#faq_FindClass
    ThreadScope::WithClassLoader([this, optionsCopy = std::move(optionsCopy), instance](){
        m_jOptions = make_global(JReactOptions::create(std::move(optionsCopy)));
        m_jReactInstance = make_global(JReactInstance::create(m_jOptions, instance));
    });
}

void ReactInstanceAndroidInternal::Destroy() {
    ThreadScope::WithClassLoader([this](){
        m_jOptions.release();
        m_jReactInstance.release();
    });
}

void ReactInstanceAndroid::Initialize() noexcept
{
    m_internalState = Mso::Make<ReactInstanceAndroidInternal>();
    m_internalState->Initialize(m_options, Mso::WeakPtr(this));
}

void ReactInstanceAndroid::Finalize() noexcept
{
    m_internalState->Destroy();
    m_internalState->Release();
}

//! This method must be called from the native queue.
Mso::Future<void> ReactInstanceAndroid::Destroy() noexcept
{
    return Mso::MakeSucceededFuture();
}

const std::string& ReactInstanceAndroid::JsBundleName() const noexcept
{
    return Options().Identity;
}

std::string ReactInstanceAndroid::SDXBasePath() const noexcept
{
    return Options().SDXBasePath;
}

const ReactOptions& ReactInstanceAndroid::Options() const noexcept
{
    // TODO
    return m_internalState->m_jOptions->cthis()->Options();
}


void ReactInstanceAndroid::NotifyRekaInitialized() noexcept
{
    // We want to make sure that notify about Reka initialization only once and only after the ReactInstance
    // is loaded and the Reka is initialized. Thus, this method is called when the ReactInstance
    // is successfully loaded and when the Reka is initialized because we do not know what comes first.
    if (m_isLoaded && !m_isRekaInitialized)
    {
        if (auto rekaContext = m_rekaContext.Load().Get())
        {
            m_isRekaInitialized = true;

            m_rekaContextProxy->SetContext(*rekaContext);
            if (auto onRekaInitialized = m_options.OnInitialized.Get())
            {
                onRekaInitialized->Invoke(*rekaContext);
            }
        }
    }
}

void ReactInstanceAndroid::NotifyRekaDestroyed() noexcept
{
    if (m_isRekaInitialized)
    {
        m_isRekaInitialized = false;

        if (auto rekaContext = m_rekaContext.Load().Get())
        {
            if (auto onRekaDestroyed = m_options.OnDestroyed.Get())
            {
                onRekaDestroyed->Invoke(*rekaContext);
            }
        }

        m_rekaContextProxy->SetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
    }
}

Mso::JSHost::RekaBridgeOptions ReactInstanceAndroid::createRekaBridgeOptions() noexcept
{
    Mso::JSHost::RekaBridgeOptions rekaOptions {};
    const auto& reactOptions = Options();
    if (Options().RekaProviderFactory)
    {
        // Custom Provider Factory
        rekaOptions.RekaServiceProviderFactory = reactOptions.RekaProviderFactory;
    }
    else
    {
        // Default Provider Factory
        rekaOptions.RekaServiceProviderFactory =
                [rekaServiceProviderName = reactOptions.DataServiceProviderName](Mso::JSHost::IRekaContext& context) mutable noexcept
                {
                    return Mso::JSHost::GetOrMakeRekaServiceProvider(rekaServiceProviderName.c_str(), context);
                };
    }

    rekaOptions.OnInitialized = [weakThis = Mso::WeakPtr(this)](Mso::JSHost::IRekaContext& context) noexcept
    {
        if (auto strongThis = weakThis.GetStrongPtr())
        {
            strongThis->m_rekaContext.Store(&context);
            strongThis->NotifyRekaInitialized();
        }
    };

    rekaOptions.OnDestroyed = [weakThis = Mso::WeakPtr(this)](Mso::JSHost::IRekaContext& /*context*/) noexcept
    {
        if (auto strongThis = weakThis.GetStrongPtr())
        {
            strongThis->NotifyRekaDestroyed();
            strongThis->m_rekaContext.Exchange(nullptr);
        }
    };

    // Note:: We are configuring JsExecutor on native module initialization.
//    rekaOptions.JsExecutor = Mso::Make<reactreka::ReactNativeRekaBridge, Mso::JSHost::IRekaBridge>(
//            reactreka::ReactContextHolder:: m_jReactContextHolder /*reinterpret_cast ??*/);

    rekaOptions.NativeQueue = Queue();
    return rekaOptions;
}

facebook::jsi::Runtime* GetJsiRuntime(IReactInstance& instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_internalState->m_jReactInstance;
    return JReactInstance::GetJsiRuntime(jReactInstance);
}

std::shared_ptr<facebook::react::CallInvoker> GetJSThreadCallInvoker(IReactInstance& instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_internalState->m_jReactInstance;
    return JReactInstance::getJSCallInvokerHolder(jReactInstance);
}

// TODO :: Handle exceptions
facebook::react::RuntimeExecutor GetRuntimeExecutor2(Mso::React::IReactInstance &instance) noexcept {
    ReactInstanceAndroid *instanceImpl = static_cast<ReactInstanceAndroid *>(&instance);
    facebook::jni::global_ref<JReactInstance::jhybridobject> jReactInstance = instanceImpl->m_internalState->m_jReactInstance;
    return JReactInstance::GetRuntimeExecutor(jReactInstance);
}


Mso::TCntPtr<Mso::React::IReactViewInstance> GetReactViewInstanceFromView(jobject view) {
    facebook::jni::alias_ref<JOfficeReactRootView::jhybridobject> viewInstance = reinterpret_cast<JOfficeReactRootView::jhybridobject>(view);
    return viewInstance->cthis()->m_ReactViewInstance;
}

}; // namespace Mso::React


extern "C" jint JNI_OnLoad(JavaVM* vm, void*) {
    return facebook::jni::initialize(vm, [] {
        JReactOptions::registerNatives();
        JReactDevOptions::registerNatives();
        JReactViewOptions::registerNatives();
        JReactHost::registerNatives();
        JReactHostStatics::registerNatives();
        JReactInstance::registerNatives();
        JReactViewHost::registerNatives();
        JMsoFuture::registerNatives();
        JOfficeReactRootView::registerNatives();
    });
}
