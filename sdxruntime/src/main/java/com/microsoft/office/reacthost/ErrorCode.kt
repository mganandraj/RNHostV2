package com.microsoft.office.reacthost
import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
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