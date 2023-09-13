package com.microsoft.office.reacthost

import com.facebook.jni.HybridData

class ReactOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    internal constructor() {
        mHybridData = initHybrid()
    }

    var identity: String?
        external get
        external set

    companion object {
        init {
            System.loadLibrary("fbjni")
            System.loadLibrary("reacthost")
        }
    }
}