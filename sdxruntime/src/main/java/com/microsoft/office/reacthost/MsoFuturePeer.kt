package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers

// Peer object of an Mso::Future<void>
@KeepClassAndMembers
class MsoFuturePeer {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    external fun Set()
}