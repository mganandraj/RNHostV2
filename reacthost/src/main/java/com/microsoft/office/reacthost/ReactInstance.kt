package com.microsoft.office.reacthost

import android.util.Log
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceManager
import com.facebook.react.bridge.RuntimeExecutor
import com.microsoft.office.reactnative.host.JSBundle
import com.microsoft.office.reactnative.host.ReactNativeHost

class ReactInstance internal constructor(reactOptions: ReactOptions) {
    companion object {
        val LOG_TAG = "ReactInstance"

        init {
            System.loadLibrary("reacthost");
        }
    }

    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
    private var mReactNativeHost: ReactNativeHost? = null
    private val mReactOptions: ReactOptions

    private external fun onInitialized()

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions

        var identity = mReactOptions.identity
        var javaModuleNames = reactOptions.JavaModuleNames

        var platformBundles = ArrayList<com.microsoft.office.reactnative.host.JSBundle>()
        mReactOptions.JSBundles.forEach{
            var rnxBundleInfo = com.microsoft.office.reactnative.host.JSBundleInfo(it.Info?.Id, it.Info?.FileName, it.Info?.Timestamp);
            var rnxJSBundle = com.microsoft.office.reactnative.host.JSBundle(it.Content, rnxBundleInfo);
            platformBundles.add(rnxJSBundle)
        }

        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            this.mReactNativeHost = ReactNativeHost.Builder()
                .activity(initialActivity!!.get()!!)
                .application(initialActivity!!.get()!!.application)
                .isDev(true)
                .jsMainModulePath("index")
                .platformBundles(platformBundles)
                .onJSRuntimeInitialized {
                    onInitialized();
                    Log.i(LOG_TAG, "ReactIntegration.RootView.onJSRuntimeInitialized")
                }
                .onJSBundleLoaded {  bundleName: String ->
                    // mReactOptions.OnInstanceCreated?.run();
                }
                .build() }
        )
    }

    fun getReactInstanceManager() : ReactInstanceManager {
       return mReactNativeHost?.reactInstanceManager!!
    }

    fun getRuntimeExecutor() : RuntimeExecutor? {
        return mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.catalystInstance?.runtimeExecutor
    }
}