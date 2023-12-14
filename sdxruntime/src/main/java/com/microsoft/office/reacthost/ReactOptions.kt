package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import java.io.File
import java.nio.ByteBuffer
import java.nio.charset.StandardCharsets

@KeepClassAndMembers
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

    fun AddRegisteredJSBundle(jsBundleId: String) {
        AddJSBundle(JSBundle(null, JSBundleInfo(jsBundleId, jsBundleId, null)))
    }

    fun AddFileJSBundle(fileName: String) {
        AddJSBundle(JSBundle(null, JSBundleInfo(fileName, fileName, 0)))
    }

    fun AddDynamicJSBundle(jsBundleId: String, jsBundleContent: String) {
        val utf8Buffer = StandardCharsets.UTF_8.encode(jsBundleContent)
        AddJSBundle(JSBundle(ByteBuffer.allocateDirect(utf8Buffer.capacity()).put(utf8Buffer), JSBundleInfo(jsBundleId, null, null)))
    }

    var instanceCreatedCallback: IInstanceCreatedCallback
        external get
        external set

    var instanceLoadedCallback: IInstanceLoadedCallback
        external get
        external set

    companion object {
        init {
            if(!ReactHostStatics.ensureInitialized())
                throw RuntimeException("ReactHost is not initialized")
        }
    }
}