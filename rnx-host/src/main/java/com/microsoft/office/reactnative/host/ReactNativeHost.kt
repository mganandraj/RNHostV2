package com.microsoft.office.reactnative.host

import android.app.Activity
import android.app.Application
import android.content.Context
import android.os.Bundle
import android.util.Log
import com.facebook.hermes.reactexecutor.HermesExecutorFactory
import com.facebook.react.ReactInstanceEventListener
import com.facebook.react.ReactInstanceManager
import com.facebook.react.ReactInstanceManagerBuilder
import com.facebook.react.ReactPackage
import com.facebook.react.ReactRootView
import com.facebook.react.bridge.JSBundleLoader
import com.facebook.react.bridge.JSBundleLoaderDelegate
import com.facebook.react.bridge.JSIModulePackage
import com.facebook.react.bridge.JavaScriptExecutorFactory
import com.facebook.react.bridge.ReactContext
import com.facebook.react.common.LifecycleState
import com.facebook.react.devsupport.interfaces.DevOptionHandler
import com.facebook.react.modules.systeminfo.ReactNativeVersion
import com.facebook.react.shell.MainReactPackage
import com.facebook.soloader.SoLoader
import java.nio.file.Paths

class ReactNativeHost private constructor (
    private val userBundle: JSBundle,
    private val preloadBundles: MutableList<JSBundle>?,
    private val loadUsingRNBundleLoader: Boolean = false,
    private val jsBundleFetcher: JSBundleFetcher?,
    private val jsMainModulePath: String?,
    private val beforeReactNativeInit: (() -> Unit)?,
    private val afterReactNativeInit: (() -> Unit)?,
    private val onJSRuntimeInitialized: (() -> Unit)?,
    private val onJSBundleLoaded: ((bundleName: String) -> Unit)?,
    private val nativeModulePackages: MutableList<ReactPackage>,
    private val javaScriptExecutorFactoryOverride: JavaScriptExecutorFactory?,
    private val customDevOptions: MutableList<Pair<String, DevOptionHandler>>? = null,
    private val eagerInit: Boolean = true,
    private val isDev: Boolean = false,
    private val enableFlipper: Boolean = false,
    private val useFabric: Boolean = false,
    hostApplication: Application,
    private val hostInitialActivity: Activity) : com.facebook.react.ReactNativeHost(hostApplication)
{
    companion object {
        init {
            try { 
                SoLoader.loadLibrary("hermes")
                SoLoader.loadLibrary("hermes_executor") 
            } catch(ex: UnsatisfiedLinkError) {}
        }
    }

    data class Builder(
        private var userBundle: JSBundle? = null, // Feature bundle
        private var preloadBundles: MutableList<JSBundle>? = null, // Bundles to be loaded prior to loading feature bundle
        private var loadUsingRNBundleLoader: Boolean = false,  // We use a custom wrapper JS executor by default. This setting forces using https://github.com/facebook/react-native/blob/main/packages/react-native/ReactAndroid/src/main/java/com/facebook/react/bridge/JSBundleLoader.java instead. This setting doesn't work when loading bundle from metro server.
        private var jsBundleFetcher: JSBundleFetcher? = null,
        private var jsMainModulePath: String? = null,
        private var beforeReactNativeInit: (() -> Unit)? = null,
        private var afterReactNativeInit: (() -> Unit)? = null,
        private var onJSRuntimeInitialized: (() -> Unit)? = null,
        private var onJSBundleLoaded: ((bundleName: String) -> Unit)? = null,
        private var nativeModulePackages: MutableList<ReactPackage> = mutableListOf(),
        private var javaScriptExecutorFactory: JavaScriptExecutorFactory? = null,
        private var customDevOptions: MutableList<Pair<String, DevOptionHandler>>? = null,
        private var useHermes: Boolean = false,
        private var eagerInit: Boolean = true,
        private var isDev: Boolean = false,
        private var enableFlipper: Boolean = false,
        private var useFabric: Boolean = false,
        private var hostApplication: Application? = null,
        private var hostInitialActivity: Activity? = null) {
        fun userBundle(_userBundle: JSBundle) = apply { this.userBundle = _userBundle; return this }
        fun preloadBundles(_preloadBundles: MutableList<JSBundle>) = apply { this.preloadBundles = _preloadBundles; return this }
        fun loadUsingRNBundleLoader(_loadUsingRNBundleLoader: Boolean) = apply { this.loadUsingRNBundleLoader = _loadUsingRNBundleLoader; return this }
        fun jsBundleFetcher(_jsBundleFetcher: JSBundleFetcher) = apply { this.jsBundleFetcher = _jsBundleFetcher; return this }
        fun jsMainModulePath(_jsMainModulePath: String) = apply { this.jsMainModulePath = _jsMainModulePath; return this }
        fun beforeReactNativeInit(_beforeReactNativeInit: (() -> Unit)) = apply { this.beforeReactNativeInit = _beforeReactNativeInit; return this }
        fun afterReactNativeInit(_afterReactNativeInit: (() -> Unit)) = apply { this.afterReactNativeInit = _afterReactNativeInit; return this }
        fun onJSRuntimeInitialized(_onJSRuntimeInitialized: (() -> Unit)) = apply { this.onJSRuntimeInitialized = _onJSRuntimeInitialized; return this }
        fun onJSBundleLoaded(_onJSBundleLoaded: ((bundleName: String) -> Unit)) = apply { this.onJSBundleLoaded = _onJSBundleLoaded; return this }
        fun nativeModulePackages(_nativeModulePackages: MutableList<ReactPackage>) = apply { this.nativeModulePackages = _nativeModulePackages; return this }
        fun shouldEagerInit(_eagerInit: Boolean) = apply { this.eagerInit = _eagerInit; return this }
        fun javaScriptExecutorFactory(_javaScriptExecutorFactory: JavaScriptExecutorFactory) = apply { this.javaScriptExecutorFactory = _javaScriptExecutorFactory; return this }
        fun customDevOptions(_customDevOptions: MutableList<Pair<String, DevOptionHandler>>) = apply { this.customDevOptions = _customDevOptions; return this }
        fun useHermes(_useHermes: Boolean) = apply { this.useHermes = _useHermes; return this }
        fun isDev(_isDev: Boolean) = apply { this.isDev = _isDev; return this }
        fun enableFlipper(_enableFlipper: Boolean) = apply { this.enableFlipper = _enableFlipper; return this }
        fun useFabric(_useFabric: Boolean) = apply { this.useFabric = _useFabric; return this }
        fun application(_hostApplication: Application) = apply { this.hostApplication = _hostApplication; return this }
        fun activity(_hostInitialActivity: Activity) = apply { this.hostInitialActivity = _hostInitialActivity; return this }
        fun build(): ReactNativeHost? = run {
            hostApplication!!
            hostInitialActivity!!
            return ReactNativeHost(
                userBundle!!,
                preloadBundles,
                loadUsingRNBundleLoader,
                jsBundleFetcher,
                jsMainModulePath,
                beforeReactNativeInit,
                afterReactNativeInit,
                onJSRuntimeInitialized,
                onJSBundleLoaded,
                nativeModulePackages,
                javaScriptExecutorFactory,
                customDevOptions,
                eagerInit,
                isDev,
                enableFlipper,
                useFabric,
                hostApplication!!,
                hostInitialActivity!!
            )
        }
    }

    init {
        val reactInstanceListener = object : ReactInstanceEventListener {
            override fun onReactContextInitialized(context: ReactContext?) {
                afterReactNativeInit?.invoke()

                // proactively removing the listener to avoid leaking memory
                // and to avoid dupe calls to afterReactNativeInit()
                reactInstanceManager.removeReactInstanceEventListener(this)
            }
        }
        reactInstanceManager.addReactInstanceEventListener(reactInstanceListener)
        reactInstanceManager.addReactInstanceEventListener { reactContext -> onReactContextInitialized(reactContext) }
        beforeReactNativeInit?.invoke()

        if(this.eagerInit) {
            reactInstanceManager.createReactContextInBackground()
        }

        if (enableFlipper) {
            try {
                Class.forName("com.microsoft.reacttestapp.ReactNativeFlipper")
                    .getMethod("initialize", Context::class.java, ReactInstanceManager::class.java)
                    .invoke(null, application, reactInstanceManager)
            } catch (e: ClassNotFoundException) {
                val flipperVersion = "0.125.0" // TODO : Find the right recommended version
                if (flipperVersion != "0") {
                    val major = ReactNativeVersion.VERSION["major"] as Int
                    val minor = ReactNativeVersion.VERSION["minor"] as Int
                    Log.i(
                        "ReactTestApp",
                        "To use Flipper, define `FLIPPER_VERSION` in your `gradle.properties`. " +
                                "Since you're using React Native $major.$minor, we recommend setting " +
                                "`FLIPPER_VERSION=$flipperVersion`."
                    )
                }
            }
        }

        customDevOptions?.forEach{
            reactInstanceManager.devSupportManager.addCustomDevOption(it.first, it.second)
        }
        application.registerActivityLifecycleCallbacks(RNActivityLifecycleCallbacks())
    }

    fun reloadFromServer() {
        reactInstanceManager.devSupportManager.handleReloadJS()
    }

    fun restartFromDisk(activity: Activity?) { // TODO : Better naming
        clear()

        reactInstanceManager.run {
            createReactContextInBackground()
            if (activity != null) {
                onHostResume(activity)
            }
        }
    }

    private fun configureBundleSettingsUsingRNBundleLoader(builder: ReactInstanceManagerBuilder) {
        builder.setJSBundleLoader(object : JSBundleLoader() {
            override fun loadScript(jsBundleLoaderDelegate: JSBundleLoaderDelegate): String? {
                val bundlesToLoad : MutableList<JSBundle> = ArrayList()
                if (preloadBundles != null) {
                    bundlesToLoad.addAll(preloadBundles)
                }
                bundlesToLoad.add(userBundle)

                for (bundle in bundlesToLoad) {
                    val fetchedBundle = jsBundleFetcher?.fetch(bundle, true) ?: bundle
                    val bundleInfo = fetchedBundle.Info
                    if (bundleInfo == null ||
                        (bundleInfo.Id == null && bundleInfo.FileName == null)) {
                        throw IllegalArgumentException("Bundle should be either an Asset or a file name") // For now !
                    }

                    if (bundleInfo.Id != null) {
                        JSBundleLoader.createAssetLoader(application, bundleInfo.Id, false)
                                .loadScript(jsBundleLoaderDelegate)
                    } else {
                        JSBundleLoader.createFileLoader(bundleInfo.FileName)
                            .loadScript(jsBundleLoaderDelegate)
                    }
                }

                return null
            }
        });
    }

    private fun configureUserBundleSettings(builder: ReactInstanceManagerBuilder) {
        val fetchedBundle = if(!useDeveloperSupport ) jsBundleFetcher?.fetch(userBundle, false) ?: userBundle else userBundle
        val bundleInfo = fetchedBundle.Info
                ?: throw IllegalArgumentException("User bundle should be either an Asset or a file name") // For now !

        if(bundleInfo.FileName != null) {
            if(Paths.get(bundleInfo.FileName).isAbsolute)
                builder.setJSBundleFile(bundleInfo.FileName)
            else
                builder.setBundleAssetName(bundleInfo.FileName)
        } else {
            throw IllegalArgumentException("An Asset or file name of the user bundle must be specified. ")
        }
    }

    private fun configureJSExecutor(builder: ReactInstanceManagerBuilder) {
        var baseExecutorFactory = javaScriptExecutorFactoryOverride ?: HermesExecutorFactory()
        var fetchedBundles = preloadBundles?.map { jsBundleFetcher?.fetch(it, true) ?: it }

        val wrappedExecutorFactory = WrapperJSExecutorFactory(
            application.applicationContext,
            baseExecutorFactory,
            if (loadUsingRNBundleLoader || fetchedBundles.isNullOrEmpty()) arrayOf<JSBundle> () else fetchedBundles.toTypedArray(),
            object :
                JSExecutorObserver {
                override fun OnBundleLoaded(bundleUrl: String?) {
                    onJSBundleLoaded?.invoke(bundleUrl!!)
                }

                override fun OnInitialized() {
                    onJSRuntimeInitialized?.invoke()
                }
            }
        )
        builder.setJavaScriptExecutorFactory(wrappedExecutorFactory)
    }

    override fun createReactInstanceManager(): ReactInstanceManager {
        val builder = ReactInstanceManager.builder()
            .setApplication(application)
            .setJSMainModulePath(jsMainModuleName)
            .setUseDeveloperSupport(useDeveloperSupport)
            .setRedBoxHandler(redBoxHandler)
            .setJSIModulesPackage(jsiModulePackage)

        if(hostInitialActivity != null) {
            builder.setInitialLifecycleState(LifecycleState.RESUMED)
            builder.setCurrentActivity(hostInitialActivity)
        }

        if(loadUsingRNBundleLoader) {
            configureBundleSettingsUsingRNBundleLoader(builder)
        } else {
            configureUserBundleSettings(builder)
        }

        configureJSExecutor(builder)

        builder.addPackages(packages)
        builder.addPackage(MainReactPackage())
        return builder.build()
    }

    private val reactInstanceEventListenerList: MutableList<ReactInstanceEventListener> = ArrayList()
    fun addReactInstanceEventListener(reactInstanceEventListener: ReactInstanceEventListener) {
        reactInstanceEventListenerList.add(reactInstanceEventListener)
    }

    private fun onReactContextInitialized(reactContext: ReactContext) {
        reactInstanceEventListenerList.forEach { it.onReactContextInitialized(reactContext)}
    }

    override fun getUseDeveloperSupport(): Boolean {
        return isDev
    }

    override fun getPackages(): MutableList<ReactPackage> {
        return nativeModulePackages
    }

    override fun getJSMainModuleName(): String? {
        return jsMainModulePath ?: "index.android"
    }

    override fun getJSIModulePackage(): JSIModulePackage? {
        return null
    }

    fun createRootView(componentName: String, initialProps: Bundle?, activity: Activity) : ReactRootView {
        val thisReactInstanceManager = reactInstanceManager
        val rootView = object : ReactRootView(activity) {
            override fun onAttachedToWindow() {
                startReactApplication(thisReactInstanceManager, componentName, initialProps)
                super.onAttachedToWindow()
            }
            override fun onDetachedFromWindow() {
                super.onDetachedFromWindow()
                unmountReactApplication()
            }
        }

        return rootView
    }

    // Note: ActivityLifecycleCallback to notify RN about the lifecycle transition of the host activity
    // This pass on the transition of every activities in the host application
    // TODO :: It is not clear whether this is the best strategy.
    inner class RNActivityLifecycleCallbacks () : Application.ActivityLifecycleCallbacks
    {
        override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {}
        override fun onActivityStarted(activity: Activity) {}
        override fun onActivityResumed(activity: Activity) {
            reactInstanceManager.onHostResume(activity, null /*DefaultHardwareBackBtnHandler*/)
        }
        override fun onActivityPaused(activity: Activity) {
            reactInstanceManager.onHostPause(activity)
        }
        override fun onActivityStopped(activity: Activity) {}
        override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle) {}
        override fun onActivityDestroyed(activity: Activity) {
            reactInstanceManager.onHostDestroy(activity)
        }
    }
}
