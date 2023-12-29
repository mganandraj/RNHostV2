package com.microsoft.office.reactnative.host;


import com.facebook.hermes.instrumentation.HermesSamplingProfiler;
import com.facebook.hermes.reactexecutor.RuntimeConfig;
import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.react.bridge.JavaScriptExecutorFactory;

public class HermesExecutorFactoryOverride implements JavaScriptExecutorFactory {
    private static final String TAG = "Hermes";

    private final RuntimeConfig mConfig;
    private boolean mEnableDebugger = true;
    private String mDebuggerName = "";

    private HermesExecutorOverride.LogHandler mLogHandler;
    private RuntimeInstaller mRuntimeInstaller;

    public HermesExecutorFactoryOverride() {
        this(null);
    }

    public HermesExecutorFactoryOverride(RuntimeConfig config) {
        mConfig = config;
    }

    public void setEnableDebugger(boolean enableDebugger) {
        mEnableDebugger = enableDebugger;
    }

    public void setDebuggerName(String debuggerName) {
        mDebuggerName = debuggerName;
    }

    public void setLoggHandler(HermesExecutorOverride.LogHandler logHandler) {
        mLogHandler = logHandler;
    }

    public void setRuntimeInstaller(RuntimeInstaller runtimeInstaller) {
        mRuntimeInstaller = runtimeInstaller;
    }

    @Override
    public JavaScriptExecutor create() {
        return new HermesExecutorOverride(mConfig, mEnableDebugger, mDebuggerName, mLogHandler, mRuntimeInstaller);
    }

    @Override
    public void startSamplingProfiler() {
        HermesSamplingProfiler.enable();
    }

    @Override
    public void stopSamplingProfiler(String filename) {
        HermesSamplingProfiler.dumpSampledTraceToFile(filename);
        HermesSamplingProfiler.disable();
    }

    @Override
    public String toString() {
        return "JSIExecutor+HermesRuntime";
    }
}
