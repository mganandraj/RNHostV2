package com.example.rnhostv2_2

import com.facebook.jni.HybridData

class ReactViewOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    internal constructor() {
        mHybridData = initHybrid()
    }

    var ComponentName: String?
        external get
        external set

    var InitialProps: String?
        external get
        external set
}