#pragma once

#include <react/jni/JavaScriptExecutorHolder.h>
#include "JRuntimeInstaller.h"

namespace facebook::react {
static std::once_flag flag;

struct JLogHandler : facebook::jni::JavaClass<JLogHandler> {
    static constexpr auto kJavaDescriptor = "Lcom/microsoft/office/reactnative/host/HermesExecutorOverride$LogHandler;";
    void OnLog(const std::string& bundleUrl, int logLevel) const;
    void OnError(const std::string& bundleUrl) const;
};

struct HermesExecutorOverride : facebook::jni::HybridClass<HermesExecutorOverride, JavaScriptExecutorHolder> {
    static constexpr auto kJavaDescriptor =
            "Lcom/microsoft/office/reactnative/host/HermesExecutorOverride;";

    static jni::local_ref<jhybriddata> initHybridDefaultConfig(
            jni::alias_ref<jclass>,
            bool enableDebugger,
            std::string debuggerName,
            facebook::jni::alias_ref<JLogHandler> logHandler,
            facebook::jni::alias_ref<JRuntimeInstaller> runtimeInstaller);

    static jni::local_ref<jhybriddata> initHybrid(
            jni::alias_ref<jclass>,
            bool enableDebugger,
            std::string debuggerName,
            jlong heapSizeMB,
            facebook::jni::alias_ref<JLogHandler> logHandler,
            facebook::jni::alias_ref<JRuntimeInstaller> runtimeInstaller);

    static bool canLoadFile(jni::alias_ref<jclass>, const std::string &path);

    static void registerNatives();

private:
    friend HybridBase;
    using HybridBase::HybridBase;
};
}