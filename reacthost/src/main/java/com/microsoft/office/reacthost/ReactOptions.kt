package com.microsoft.office.reacthost

import com.facebook.jni.HybridData

class ReactOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
    var DeveloperSettings: ReactDevOptions;

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
        public external set

    // CxxModuleNames - Unsupported
    // CxxModuleProviders - Unsupported

    var JavaModuleNames: ArrayList<String>
        external get
        external set

    var JSBundles: ArrayList<JSBundle>
        external set
        external get

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