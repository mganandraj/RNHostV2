package com.microsoft.office.reacthost

import android.util.Log
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceEventListener
import com.facebook.react.ReactInstanceManager
import com.facebook.react.ReactPackage
import com.facebook.react.bridge.ReactContext
import com.facebook.react.bridge.RuntimeExecutor
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl
import com.facebook.soloader.SoLoader
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import com.microsoft.office.reactnative.host.JsiRuntimeRef
import com.microsoft.office.reactnative.host.RuntimeInstaller
import com.microsoft.office.reactnative.host.OfficeBundleFetcher
import com.microsoft.office.reactnative.host.ReactNativeHost
import com.microsoft.office.reactnative.reka.RekaBridgeOptions
import kotlinx.coroutines.InternalCoroutinesApi
import java.lang.ref.WeakReference
import java.lang.reflect.InvocationTargetException
import java.lang.reflect.Method

@KeepClassAndMembers
class ReactInstance internal constructor(reactOptions: ReactOptions) {
    companion object {
        val LOG_TAG = "ReactInstance"

        init {
            if(!ReactHostStatics.ensureInitialized())
                throw RuntimeException("ReactHost is not initialized")
        }
    }

    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
    private var mReactNativeHost: ReactNativeHost? = null
    private val mReactOptions: ReactOptions

    private external fun onInitialized(/*contextHolder: ReactContextHolder*/)
    private external fun onBundleLoaded(bundleName: String)
    private external fun createRekaBridgeOptions() : RekaBridgeOptions
    private external fun onDoRuntimeInstall(runtimeRef: JsiRuntimeRef)

    init {
        mHybridData = initHybrid()
        mReactOptions = reactOptions
    }

    private val reactInstanceEventListenerList: MutableList<ReactInstanceEventListener> = ArrayList()
    private var isInitialized = false;
    val reactInstanceEventLock = Any()

    @OptIn(InternalCoroutinesApi::class)
    fun enqueueTaskOnReactContextInitialized(task: ReactInstanceEventListener) {
        kotlinx.coroutines.internal.synchronized(reactInstanceEventLock) {
            if(isInitialized) {
                this.mReactNativeHost!!.addReactInstanceEventListener(task)
            } else {
                reactInstanceEventListenerList.add(task)
            }
        }
    }

    @OptIn(InternalCoroutinesApi::class)
    @Suppress("UNUSED_PARAMETER")
    fun onReactContextInitialized(reactContext: ReactContext?) {
        kotlinx.coroutines.internal.synchronized(reactInstanceEventLock) {
            onInitialized(/*ReactContextHolder(reactContext!!)*/)
            reactInstanceEventListenerList.forEach { initialActivity?.get()?.runOnUiThread{ it.onReactContextInitialized(reactContext) } }
            isInitialized = true;
        }
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

    fun getRNXJSBundle(jsBundle: JSBundle) : com.microsoft.office.reactnative.host.JSBundle {
        var rnxBundleInfo = com.microsoft.office.reactnative.host.JSBundleInfo(jsBundle.Info?.Id, jsBundle.Info?.FileName, jsBundle.Info?.Timestamp);
        return com.microsoft.office.reactnative.host.JSBundle(jsBundle.Content, rnxBundleInfo);
    }

    // TODO :: Pending any error case handling ..
    // Note :: This is called from native
    fun initialize() {
        var javaModuleNames = mReactOptions.JavaModuleNames
        var nativeModulePackages: MutableList< ReactPackage> = ArrayList< ReactPackage>();

        val rekaBridgeOptions = createRekaBridgeOptions()
        nativeModulePackages.add(com.microsoft.office.reactnative.reka.RekaReactPackage.GetReactPackage(rekaBridgeOptions))
        // nativeModulePackages.add(PolyesterReactPackage())

        javaModuleNames.forEach {
            val reactPackage = getReactPackageFromClassName(it)
            if(reactPackage != null)
                nativeModulePackages.add(reactPackage)
        }

        var platformBundles = ArrayList<com.microsoft.office.reactnative.host.JSBundle>()
        // All except last one is platform bundles
        mReactOptions.JSBundles.dropLast(1).forEach{
            platformBundles.add(getRNXJSBundle(it))
        }

        val featureBundle = mReactOptions.JSBundles.last()

        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            val builder = ReactNativeHost.Builder()
                .shouldEagerInit(true)
                .activity(initialActivity!!.get()!!)
                .application(initialActivity!!.get()!!.application)
                .isDev(mReactOptions.DeveloperSettings.IsDevModeEnabled?:false)
                .debuggerName(mReactOptions.DeveloperSettings.DebuggerRuntimeName)
                .enableDebugger(mReactOptions.DeveloperSettings.UseDirectDebugger)
                .jsMainModulePath(mReactOptions.DeveloperSettings.SourceBundleName?:"index")
                .preloadBundles(platformBundles)
                .nativeModulePackages(nativeModulePackages)
                .onJSRuntimeInitialized {
                    // Log.i(LOG_TAG, "ReactIntegration.RootView.onJSRuntimeInitialized")
                }
                .onJSBundleLoaded {  bundleName: String ->
                    onBundleLoaded(bundleName)
                    // mReactOptions.OnInstanceCreated?.run();
                }
                .userBundle(getRNXJSBundle(featureBundle))
                .jsBundleFetcher(OfficeBundleFetcher(initialActivity!!.get()!!.application))
                .javaScriptRuntimeInstaller(object: RuntimeInstaller {
                    override fun DoInstall(runtimeRef: JsiRuntimeRef?) {
                        onDoRuntimeInstall(runtimeRef!!);
                    }
                })

            this.mReactNativeHost = builder.build()
            val weakThis = WeakReference(this)
            this.mReactNativeHost?.addReactInstanceEventListener { reactContext ->
                weakThis.get()?.onReactContextInitialized(reactContext);
            }
        })
    }

    fun getReactOptions(): ReactOptions {
        return mReactOptions
    }

    fun getCurrentReactContext(): ReactContext? {
        return mReactNativeHost?.reactInstanceManager!!.currentReactContext
    }

    fun getReactInstanceManager() : ReactInstanceManager? {
       return mReactNativeHost?.reactInstanceManager
    }

    fun getRuntimeExecutor() : RuntimeExecutor? {
        return mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.catalystInstance?.runtimeExecutor
    }

    fun getJsiRuntimeRef() : Long {
        val ref = mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.javaScriptContextHolder?.get()!!
        return ref;
    }

    // TODO :: Make this function return the interface CallInvokerHolder
    fun getJSCallInvokerHolder() : CallInvokerHolderImpl? {
        return mReactNativeHost?.reactInstanceManager!!.getCurrentReactContext()?.catalystInstance?.jsCallInvokerHolder as CallInvokerHolderImpl
    }
}