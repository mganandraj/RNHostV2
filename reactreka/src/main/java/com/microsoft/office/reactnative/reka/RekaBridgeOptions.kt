package com.microsoft.office.reactnative.reka;

import com.facebook.jni.HybridData
import com.facebook.soloader.SoLoader

class RekaBridgeOptions {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    companion object {
        init {
            SoLoader.loadLibrary("fbjni")
            SoLoader.loadLibrary("reactrekadroid")
        }
    }

}
