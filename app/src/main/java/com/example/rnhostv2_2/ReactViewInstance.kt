package com.example.rnhostv2_2

import android.content.Context
import android.util.AttributeSet
import com.facebook.jni.HybridData
import com.facebook.react.ReactRootView

class OfficeReactRootView : ReactRootView {
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
    fun  Reload(reactInstance: ReactInstance?, viewOptions: ReactViewOptions?) {
        ReactHostStatics.initialActivity?.get()?.runOnUiThread(Runnable {
            this.startReactApplication(reactInstance?.getReactInstanceManager(), viewOptions?.ComponentName, fromJson(viewOptions?.InitialProps));
        })
    }

    // TODO:: Return Future ?
    /*fun  Reload(viewOptions: ReactViewOptions?) {
        // val compName = viewOptions?.ComponentName;
        this.startReactApplication(reactInstance?.getReactInstanceManager(), viewOptions?.ComponentName)
    }*/

    fun  Reload() {
        // this.startReactApplication(reactInstance?.getReactInstanceManager(), viewOptions?.ComponentName)
    }

    // TODO:: Return Future ?
    fun  Unload() {
        // this.unmountReactApplication()
    }

    private external fun initHybrid(): HybridData
    private val mHybridData: HybridData
}