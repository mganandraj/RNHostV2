package com.microsoft.office.reacthost

import android.content.Context
import android.os.Bundle
import android.os.Parcel
import android.util.AttributeSet
import android.util.Base64
import android.util.Log
import android.view.View
import com.facebook.jni.HybridData
import com.facebook.react.ReactInstanceEventListener
import com.facebook.react.ReactRootView
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import com.microsoft.office.plat.logging.Trace
import org.json.JSONArray
import org.json.JSONException
import org.json.JSONObject


open class ViewListener {
    open fun onViewRendered() {}
    open fun onBeforeStartApplication(instance: ReactInstance) {}
}

@KeepClassAndMembers
open class BaseRootView : ReactRootView {

    private var mViewPreviouslyRendered = false
    private var mViewListener: ViewListener? = null

    constructor(context: Context?) : super(context) {
        mHybridData = initHybrid()
    }
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {
        mHybridData = initHybrid()
    }
    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int) : super(context, attrs, defStyle) {
        mHybridData = initHybrid()
    }
    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int, viewListener: ViewListener?) : super(context, attrs, defStyle) {
        mHybridData = initHybrid()
        mViewListener = viewListener
    }

    override fun onViewAdded(child: View?) {
        super.onViewAdded(child)
        if (!mViewPreviouslyRendered) {
            mViewListener?.onViewRendered()
            mViewPreviouslyRendered = true
        }
    }

    @Suppress("FunctionName")
    fun  Reload(reactInstance: ReactInstance?, viewOptions: ReactViewOptions?, msoFuturePeer: MsoFuturePeer) {
        // Note: We are waiting till ReactContext is available to allow clients to run JS code on `onBeforeStartApplication`
        reactInstance?.enqueueTaskOnReactContextInitialized {
            reactContext ->
            run {
                mViewListener?.onBeforeStartApplication(reactInstance)
                ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
                    this.startReactApplication(
                        reactInstance?.getReactInstanceManager(),
                        viewOptions?.ComponentName,
                        deSerializeBundle(viewOptions?.InitialProps!!)
                    );
                    msoFuturePeer.Set()
                })
            }
        }
    }

    @Suppress("FunctionName")
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

    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int, reactOptions: ReactOptions, componentName: String, launchOptions: Bundle
                , viewListener: ViewListener?) :
            super(context, attrs, defStyle, viewListener) {
        mComponentName = componentName
        mLaunchOptions = launchOptions
        mReactHost = ReactHostStatics.makeReactHost(reactOptions)
        var viewOptions: ReactViewOptions = ReactViewOptions()
        viewOptions.ComponentName = mComponentName
        viewOptions.InitialProps = serializeBundle(launchOptions)
        mReactViewHost = mReactHost!!.MakeViewHost(viewOptions)
    }

    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int, reactOptions: ReactOptions, componentName: String, launchOptions: Bundle) : this(context, attrs, defStyle, reactOptions, componentName, launchOptions, null) {}
}