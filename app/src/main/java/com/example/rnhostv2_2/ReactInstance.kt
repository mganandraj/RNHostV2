package com.example.rnhostv2_2

import com.example.rnhostv2_2.ReactHostStatics.initialActivity
import com.example.rnhostv2_2.ReactOptions
import com.example.rnhostv2_2.ReactHostStatics
import com.facebook.jni.HybridData
import com.microsoft.office.reactnative.host.ReactNativeHost

class ReactInstance internal constructor(reactOptions: ReactOptions) {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
    private val mReactNativeHost: ReactNativeHost?
    private val mReactOptions: ReactOptions

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions
        mReactNativeHost = ReactNativeHost.Builder()
                .activity(initialActivity!!.get()!!)
                .application(initialActivity!!.get()!!.application)
                .isDev(true)
                .jsMainModulePath("index")
                .build()
    }
}