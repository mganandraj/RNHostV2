package com.microsoft.office.reacthost

import com.facebook.react.bridge.ReactContext
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import java.lang.ref.WeakReference

@KeepClassAndMembers
class ReactContextHolder(reactContext: ReactContext) {
    private val mReactContext: WeakReference<ReactContext>

    init {
        mReactContext = WeakReference(reactContext)
    }

    val reactContext: ReactContext?
        get() = mReactContext.get()
}