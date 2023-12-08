package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
class ReactDevOptions {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }
    var IsDevModeEnabled: Boolean
        external get
        external set

    var UseDirectDebugger: Boolean
        external get
        external set

    var DebuggerRuntimeName: String
        external get
        external set   

    // The following settings are not respected on Android. Hence not projecting to Kotlin/Java.
    // DebuggerBreakOnNextLine
    // DebuggerPort
    //
    // UseLiveReload - Always enabled in dev mode
    // UseFastRefresh - Always enabled in dev mode
    // DebugHost
    //
    // SourceBundleHost - default to localhost
    // SourceBundlePort - default to 8081

    var SourceBundleName: String?
        external get
        external set

    // SourceBundleModuleName
    // JSIEngineOverride
    // HttpModuleClass
}