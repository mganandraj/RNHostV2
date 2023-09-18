package com.microsoft.office.reacthost
import com.facebook.jni.HybridData

class ErrorCode {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData

    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    constructor() {
        mHybridData = initHybrid()
    }

    external override fun toString(): String
}