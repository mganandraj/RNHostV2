package com.microsoft.office.reacthost

import android.util.Log
import androidx.core.content.PackageManagerCompat
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceManager
import com.facebook.react.ReactPackage
import com.facebook.react.bridge.RuntimeExecutor
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import com.microsoft.office.reactnative.host.ReactNativeHost
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

    private external fun onInitialized()
    private external fun onBundleLoaded(bundleName: String)

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions

        var identity = mReactOptions.identity
        var javaModuleNames = reactOptions.JavaModuleNames

        var nativeModulePackages: MutableList< ReactPackage> = ArrayList< ReactPackage>();
        javaModuleNames.forEach {
            try {
                val clazz = Class.forName(it)
                val method: Method = clazz.getMethod("GetReactPackage")
                nativeModulePackages.add(method.invoke(null /*invoking a static method*/) as ReactPackage)
            } catch (e: ClassNotFoundException) {
                Log.e(LOG_TAG, Log.getStackTraceString(e))
//                failedToInstantiate.append(
//                    java.lang.String.format(
//                        "Package: %s, Exception: %s ;",
//                        reactPackage,
//                        e.javaClass.simpleName
//                    )
//                )
            } catch (e: NoSuchMethodException) {
                Log.e(LOG_TAG, Log.getStackTraceString(e))
//                failedToInstantiate.append(
//                    java.lang.String.format(
//                        "Package: %s, Exception: %s ;",
//                        reactPackage,
//                        e.javaClass.simpleName
//                    )
//                )
            } catch (e: IllegalAccessException) {
                Log.e(LOG_TAG, Log.getStackTraceString(e))
//                failedToInstantiate.append(
//                    java.lang.String.format(
//                        "Package: %s, Exception: %s ;",
//                        reactPackage,
//                        e.javaClass.simpleName
//                    )
//                )
            } catch (e: InvocationTargetException) {
                Log.e(LOG_TAG, Log.getStackTraceString(e))
//                failedToInstantiate.append(
//                    java.lang.String.format(
//                        "Package: %s, Exception: %s ;",
//                        reactPackage,
//                        e.getClass().getSimpleName()
//                    )
//                )
            }
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
                .isDev(true)
                .jsMainModulePath("index")
                .platformBundles(platformBundles)
                .nativeModulePackages(nativeModulePackages)
                .onJSRuntimeInitialized {
                    onInitialized()
                    Log.i(LOG_TAG, "ReactIntegration.RootView.onJSRuntimeInitialized")
                }
                .onJSBundleLoaded {  bundleName: String ->
                    onBundleLoaded(bundleName)
                    // mReactOptions.OnInstanceCreated?.run();
                }
                .build() }
        )
    }

    fun getReactInstanceManager() : ReactInstanceManager {
       return mReactNativeHost?.reactInstanceManager!!
    }

    fun getRuntimeExecutor() : RuntimeExecutor? {
        var runtimeExecutor = mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.catalystInstance?.runtimeExecutor
        return runtimeExecutor
    }
}