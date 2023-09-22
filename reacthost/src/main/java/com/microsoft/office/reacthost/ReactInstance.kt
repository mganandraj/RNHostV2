package com.microsoft.office.reacthost

import android.util.Log
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceEventListener
import com.facebook.react.ReactInstanceManager
import com.facebook.react.ReactPackage
import com.facebook.react.bridge.ReactContext
import com.facebook.react.bridge.RuntimeExecutor
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import com.microsoft.office.reactnative.host.ReactNativeHost
import com.microsoft.office.reactreka.RekaBridgeOptions
import com.microsoft.office.reactreka.RekaReactPackage
import java.lang.ref.WeakReference
import java.lang.reflect.InvocationTargetException
import java.lang.reflect.Method

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

    private external fun onInitialized(contextHolder: ReactContextHolder)
    private external fun onBundleLoaded(bundleName: String)
    private external fun createRekaBridgeOptions() : RekaBridgeOptions

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions
    }

    fun onReactContextInitialized(reactContext: ReactContext?) {
        onInitialized(ReactContextHolder(reactContext!!))
    }

    private fun getReactPackageFromClassName(className: String): ReactPackage? {
        try {
            val clazz = Class.forName(className)
            val method: Method = clazz.getMethod("GetReactPackage")
            return method.invoke(null /*invoking a static method*/) as ReactPackage
        } catch (e: ClassNotFoundException) {
            Log.e(LOG_TAG, Log.getStackTraceString(e))
        } catch (e: NoSuchMethodException) {
            Log.e(LOG_TAG, Log.getStackTraceString(e))
        } catch (e: IllegalAccessException) {
            Log.e(LOG_TAG, Log.getStackTraceString(e))
        } catch (e: InvocationTargetException) {
            Log.e(LOG_TAG, Log.getStackTraceString(e))
        }
        return null
    }

    // Note :: This is called from native
    fun initialize() {
        var identity = mReactOptions.identity
        var javaModuleNames = mReactOptions.JavaModuleNames
        var nativeModulePackages: MutableList< ReactPackage> = ArrayList< ReactPackage>();

        val rekaBridgeOptions = createRekaBridgeOptions()
        nativeModulePackages.add(RekaReactPackage.GetReactPackage(rekaBridgeOptions))

        javaModuleNames.forEach {
            val reactPackage = getReactPackageFromClassName(it)
            if(reactPackage != null)
                nativeModulePackages.add(reactPackage)
        }

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
                .isDev(mReactOptions.DeveloperSettings.IsDevModeEnabled)
                .jsMainModulePath(mReactOptions.DeveloperSettings.SourceBundleName?:"index")
                .platformBundles(platformBundles)
                .nativeModulePackages(nativeModulePackages)
                .onJSRuntimeInitialized {
                    // Log.i(LOG_TAG, "ReactIntegration.RootView.onJSRuntimeInitialized")
                }
                .onJSBundleLoaded {  bundleName: String ->
                    onBundleLoaded(bundleName)
                    // mReactOptions.OnInstanceCreated?.run();
                }
                .build()

            val weakThis = WeakReference(this)
            this.mReactNativeHost?.addReactInstanceEventListener { reactContext ->
                weakThis.get()?.onReactContextInitialized(reactContext);
            }
        })
    }

    fun getReactInstanceManager() : ReactInstanceManager {
       return mReactNativeHost?.reactInstanceManager!!
    }

    fun getRuntimeExecutor() : RuntimeExecutor? {
        var runtimeExecutor = mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.catalystInstance?.runtimeExecutor
        return runtimeExecutor
    }
}