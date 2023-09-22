package com.microsoft.office.reacthost

import com.facebook.react.bridge.ReactContext
import java.lang.ref.WeakReference

class ReactContextHolder(reactContext: ReactContext) {
    private val mReactContext: WeakReference<ReactContext>

    init {
        mReactContext = WeakReference(reactContext)
    }

    val reactContext: ReactContext?
        get() = mReactContext.get()
}