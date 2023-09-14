package com.microsoft.office.reacthost

import com.facebook.jni.HybridData

class ReactViewOptions {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    constructor() {
        mHybridData = initHybrid()
    }

    var ComponentName: String?
        external get
        external set

    var InitialProps: String?
        external get
        external set
}