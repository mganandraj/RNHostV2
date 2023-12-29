#include "ReactNativeHeaders.h"
#include <hermes/hermes.h>
#include <jsi/decorator.h>
#include <jsireact/JSIExecutor.h>
#include <hermes/executor/HermesExecutorFactory.h>
#include <react/jni/JReactMarker.h>

#include "HermesExecutorOverride.h"


using namespace facebook;

namespace facebook::react {

void JLogHandler::OnLog(const std::string& message, int logLevel) const
{
    static const auto method = getClass()->getMethod<void(jstring,jint)>("onLog");
    method(self(), jni::make_jstring(message).get(), logLevel);
}

void JLogHandler::OnError(const std::string& message) const
{
    static const auto method = getClass()->getMethod<void(jstring)>("onError");
    method(self(), jni::make_jstring(message).get());
}

static void hermesFatalHandler(const std::string &reason) {
    __android_log_assert(nullptr, "Hermes", "%s", reason.c_str());
}

static void hermesLoggingHandler(
        const std::string &message,
        android_LogPriority logLevel) {
    __android_log_print(logLevel, "ReactNativeJS", "%s", message.c_str());
}

static void hermesLoggingHandler(
        const std::string &message,
        unsigned int logLevel) {
    hermesLoggingHandler(
            message, static_cast<android_LogPriority>(logLevel + ANDROID_LOG_DEBUG));
}

static void installBindings(jsi::Runtime &runtime) {
    Logger androidLogger =
            static_cast<void (*)(const std::string &, unsigned int)>(
                    &hermesLoggingHandler);

    react::bindNativeLogger(runtime, androidLogger);
    react::bindNativePerformanceNow(runtime);
}

static ::hermes::vm::RuntimeConfig makeRuntimeConfig(jlong heapSizeMB) {
    namespace vm = ::hermes::vm;
    auto gcConfigBuilder =
            vm::GCConfig::Builder()
                    .withName("RN")
                            // For the next two arguments: avoid GC before TTI by initializing the
                            // runtime to allocate directly in the old generation, but revert to
                            // normal operation when we reach the (first) TTI point.
                    .withAllocInYoung(false)
                    .withRevertToYGAtTTI(true);

    if (heapSizeMB > 0) {
        gcConfigBuilder.withMaxHeapSize(heapSizeMB << 20);
    }

    return vm::RuntimeConfig::Builder()
            .withGCConfig(gcConfigBuilder.build())
            .withEnableSampleProfiling(true)
            .build();
}


jni::local_ref<HermesExecutorOverride::jhybriddata>
HermesExecutorOverride::initHybridDefaultConfig(
        jni::alias_ref<jclass>,
        bool enableDebugger,
        std::string debuggerName,
        facebook::jni::alias_ref<JLogHandler> logHandler,
        facebook::jni::alias_ref<JRuntimeInstaller> runtimeInstaller) {
    JReactMarker::setLogPerfMarkerIfNeeded();

    logHandler->OnLog("Hello", 1);

    std::call_once(flag, []() {
        facebook::hermes::HermesRuntime::setFatalHandler(hermesFatalHandler);
    });
    auto factory = std::make_unique<HermesExecutorFactory>([logHandlerWeak = facebook::jni::make_weak(logHandler),
                                                                   runtimeInstallerWeak=facebook::jni::make_weak(runtimeInstaller)]
            (jsi::Runtime &runtime){

        Logger androidLogger = [logHandlerWeak](const std::string &message, unsigned int level) {
            auto logHandler = logHandlerWeak.lockLocal();
            if(logHandler)
                logHandler->OnLog(message, level);
        };

        auto runtimeInstaller = runtimeInstallerWeak.lockLocal();
        if(runtimeInstaller) {
            runtimeInstaller->DoInstall(JJsiRuntimeRef::create(runtime));
        }

        react::bindNativeLogger(runtime, androidLogger);
        react::bindNativePerformanceNow(runtime);
    });
    factory->setEnableDebugger(enableDebugger);
    if (!debuggerName.empty()) {
        factory->setDebuggerName(debuggerName);
    }
    return makeCxxInstance(std::move(factory));
}

jni::local_ref<HermesExecutorOverride::jhybriddata> HermesExecutorOverride::initHybrid(
        jni::alias_ref<jclass>,
        bool enableDebugger,
        std::string debuggerName,
        jlong heapSizeMB,
        facebook::jni::alias_ref<JLogHandler> logHandler,
        facebook::jni::alias_ref<JRuntimeInstaller> runtimeInstaller) {
    JReactMarker::setLogPerfMarkerIfNeeded();
    auto runtimeConfig = makeRuntimeConfig(heapSizeMB);
    std::call_once(flag, []() {
        facebook::hermes::HermesRuntime::setFatalHandler(hermesFatalHandler);
    });
    auto factory = std::make_unique<HermesExecutorFactory>(
            installBindings, JSIExecutor::defaultTimeoutInvoker, runtimeConfig);
    factory->setEnableDebugger(enableDebugger);
    if (!debuggerName.empty()) {
        factory->setDebuggerName(debuggerName);
    }
    return makeCxxInstance(std::move(factory));
}

bool
HermesExecutorOverride::canLoadFile(jni::alias_ref<jclass>, const std::string &path) {
    return true;
}

void HermesExecutorOverride::registerNatives() {
    HermesExecutorOverride::registerHybrid(
            { makeNativeMethod("initHybrid", HermesExecutorOverride::initHybrid),
             makeNativeMethod(
                     "initHybridDefaultConfig",
                     HermesExecutorOverride::initHybridDefaultConfig),
             makeNativeMethod("canLoadFile", HermesExecutorOverride::canLoadFile)});
}

}