package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import java.io.File
import java.nio.ByteBuffer
import java.nio.charset.StandardCharsets

class ReactOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
        DeveloperSettings = createDeveloperSettingsPeer()
    }

    constructor() {
        mHybridData = initHybrid()
        DeveloperSettings = createDeveloperSettingsPeer()
    }

    external fun createDeveloperSettingsPeer() : ReactDevOptions

    var identity: String?
        external get
        external set

    // CxxModuleNames - Unsupported
    // CxxModuleProviders - Unsupported

    external fun AddJavaModuleName(javaModuleName: String)
    var JavaModuleNames: ArrayList<String>
        external get
        external set

    // AppleModuleProviders

    var DataServiceProviderName: String?
        external get
        external set

    // SDXBasePath

    external fun AddJSBundle(jsBundle: JSBundle)
    var JSBundles: ArrayList<JSBundle>
        external set
        external get

    // Reka lifetime callbacks can be added in future if needed
    // OnInitialized
    // OnDestroyed

    // OnError
    // OnLogging - The default log handler writes to logcat. It is non-trivial to override it.
    // OnJSException - The default platform behaviour is to handle through ExceptionManager native module

    // EnableBytecode
    // EnableJITCompilation
    // EnableNativePerformanceNow
    // EnableSensitiveTelemetry
    // Quirks

    var DeveloperSettings: ReactDevOptions;

    // MemoryTracker

    // RekaProviderFactory
    // Properties - Can be done in future if needed.

    // JSExecutor assumes the Id to be the name of the packaged asset.
    fun AddRegisteredJSBundle(jsBundleId: String) {
        AddJSBundle(JSBundle(null, JSBundleInfo(jsBundleId, null, null)))
    }

    fun AddFileJSBundle(fileName: String) {
        AddJSBundle(JSBundle(null, JSBundleInfo(fileName, null, 0)))
    }

    fun AddDynamicJSBundle(jsBundleId: String, jsBundleContent: String) {
        val utf8Buffer = StandardCharsets.UTF_8.encode(jsBundleContent)
        AddJSBundle(JSBundle(ByteBuffer.allocateDirect(utf8Buffer.capacity()).put(utf8Buffer), JSBundleInfo(jsBundleId, null, null)))
    }

    var instanceCreatedCallback: IInstanceCreatedCallback
        public external get
        public external set

    var instanceLoadedCallback: IInstanceLoadedCallback
        public external get
        public external set

    companion object {
        init {
            System.loadLibrary("fbjni")
            System.loadLibrary("reacthost")
        }
    }
}