package com.microsoft.office.reactreka
import com.facebook.jni.HybridData

class RekaBridgeOptions {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    companion object {
        init {
            System.loadLibrary("fbjni")
            System.loadLibrary("reactreka")
        }
    }

}