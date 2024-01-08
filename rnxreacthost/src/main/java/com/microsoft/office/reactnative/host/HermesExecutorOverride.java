package com.microsoft.office.reactnative.host;


import com.facebook.hermes.reactexecutor.RuntimeConfig;
import com.facebook.jni.HybridData;
import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.react.common.build.ReactBuildConfig;
import com.facebook.soloader.SoLoader;
import androidx.annotation.Nullable;

public class HermesExecutorOverride extends JavaScriptExecutor {
    private static String mode_;

    static {
        loadLibrary();
    }

    public interface LogHandler {
        void onLog(String message, int level);
        void onError(String message);
    }

    public static void loadLibrary() throws UnsatisfiedLinkError {
        if (mode_ == null) {
            // libhermes must be loaded explicitly to invoke its JNI_OnLoad.
            SoLoader.loadLibrary("hermes");
            SoLoader.loadLibrary("hermes_executor");
            // libhermes_executor is built differently for Debug & Release so we load the proper mode.
            if (ReactBuildConfig.DEBUG == true) {
                mode_ = "Debug";
            } else {
                mode_ = "Release";
            }
        }
    }

    HermesExecutorOverride(@Nullable RuntimeConfig config, boolean enableDebugger, String debuggerName, LogHandler logHandler, RuntimeInstaller runtimeInstaller) {
        super(
                config == null
                        ? initHybridDefaultConfig(enableDebugger, debuggerName, logHandler, runtimeInstaller)
                        : initHybrid(enableDebugger, debuggerName, config.heapSizeMB, logHandler, runtimeInstaller));
    }

    @Override
    public String getName() {
        return "HermesExecutor" + mode_;
    }

    /**
     * Return whether this class can load a file at the given path, based on a binary compatibility
     * check between the contents of the file and the Hermes VM.
     *
     * @param path the path containing the file to inspect.
     * @return whether the given file is compatible with the Hermes VM.
     */
    public static native boolean canLoadFile(String path);

    private static native HybridData initHybridDefaultConfig(
            boolean enableDebugger, String debuggerName, LogHandler logHandler, RuntimeInstaller runtimeInstaller);

    private static native HybridData initHybrid(
            boolean enableDebugger, String debuggerName, long heapSizeMB, LogHandler logHandler, RuntimeInstaller runtimeInstaller);
}
