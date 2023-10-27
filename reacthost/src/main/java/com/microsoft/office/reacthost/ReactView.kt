package com.microsoft.office.reacthost

import android.content.Context
import android.os.Bundle
import android.util.AttributeSet
import android.util.Log
import com.facebook.jni.HybridData
import com.facebook.react.ReactRootView

open class BaseRootView : ReactRootView {
    constructor(context: Context?) : super(context) {
        mHybridData = initHybrid()
    }
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {
        mHybridData = initHybrid()
    }
    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int) : super(context, attrs, defStyle) {
        mHybridData = initHybrid()
    }

    // TODO:: Return Future ?
    fun  Reload(reactInstance: ReactInstance?, viewOptions: ReactViewOptions?, msoFuturePeer: MsoFuturePeer) {
        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            this.startReactApplication(reactInstance?.getReactInstanceManager(), viewOptions?.ComponentName, fromJson(viewOptions?.InitialProps));
            msoFuturePeer.Set()
        })
    }

    // TODO:: Return Future ?
    fun  Unload(msoFuturePeer: MsoFuturePeer) {
        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            this.unmountReactApplication()
            msoFuturePeer.Set()
        })
    }

    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
}

class AutoRootView : BaseRootView {
    private constructor(context: Context?) : super(context) {}
    private constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}
    private constructor(context: Context?, attrs: AttributeSet?, defStyle: Int) : super(context, attrs, defStyle) {}

    var mReactHost: ReactHost? = null
    var mReactViewHost: ReactViewHost? = null
    var mComponentName: String? = null;
    var mLaunchOptions: Bundle? = null;

    override fun onAttachedToWindow() {
        mReactViewHost!!.AttachViewInstance(this)
        super.onAttachedToWindow()
    }
    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        mReactViewHost!!.DetachViewInstance()
    }

    constructor(context: Context?, reactOptions: ReactOptions, componentName: String, launchOptions: Bundle) :
            this(context, null, 0, reactOptions, componentName, launchOptions) {}

    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int, reactOptions: ReactOptions, componentName: String, launchOptions: Bundle) :
            this(context, attrs, defStyle) {
        mComponentName = componentName
        mLaunchOptions = launchOptions
        mReactHost = ReactHostStatics.makeReactHost(reactOptions)
        var viewOptions: ReactViewOptions = ReactViewOptions()
        viewOptions.ComponentName = mComponentName
//        viewOptions.InitialProps = fromJson(). launchOptions
        mReactViewHost = mReactHost!!.MakeViewHost(viewOptions)
    }
}