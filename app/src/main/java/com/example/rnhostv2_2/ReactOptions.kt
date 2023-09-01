package com.example.rnhostv2_2

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
            System.loadLibrary("rnhostv2_2")
        }
    }
}