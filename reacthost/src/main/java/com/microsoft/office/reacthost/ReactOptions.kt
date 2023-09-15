package com.microsoft.office.reacthost

import com.facebook.jni.HybridData

class ReactOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    constructor() {
        mHybridData = initHybrid()
    }

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

    companion object {
        init {
            System.loadLibrary("fbjni")
            System.loadLibrary("reacthost")
        }
    }
}