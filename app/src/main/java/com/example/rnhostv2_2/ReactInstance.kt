package com.example.rnhostv2_2

import com.example.rnhostv2_2.ReactHostStatics.initialActivity
import com.example.rnhostv2_2.ReactOptions
import com.example.rnhostv2_2.ReactHostStatics
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceManager
import com.microsoft.office.reactnative.host.ReactNativeHost

class ReactInstance internal constructor(reactOptions: ReactOptions) {
    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
    private var mReactNativeHost: ReactNativeHost? = null
    private val mReactOptions: ReactOptions

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions

        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            this.mReactNativeHost = ReactNativeHost.Builder()
                .activity(initialActivity!!.get()!!)
                .application(initialActivity!!.get()!!.application)
                .isDev(true)
                .jsMainModulePath("index")
                .build() })

    }

    fun getReactInstanceManager() : ReactInstanceManager {
       return mReactNativeHost?.reactInstanceManager!!
    }
}