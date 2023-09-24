package com.microsoft.office.reacthost

import com.facebook.jni.HybridData

// Peer object of an Mso::Future<void>
class MsoFuturePeer {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    external fun Set()
}