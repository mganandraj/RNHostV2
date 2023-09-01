package com.example.rnhostv2_2

import android.content.Context
import android.util.AttributeSet
import com.facebook.react.ReactRootView
import com.example.rnhostv2_2.ReactInstance
import com.example.rnhostv2_2.ReactViewOptions

class ReactViewInstance : ReactRootView {
    constructor(context: Context?) : super(context) {}
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}
    constructor(context: Context?, attrs: AttributeSet?, defStyle: Int) : super(context, attrs, defStyle) {}

    // TODO:: Return Future ?
    fun  Reload(reactInstance: ReactInstance?, viewOptions: ReactViewOptions?) {
        reactInstance?.getReactInstanceManager()?.attachRootView(this)
        this.startReactApplication(reactInstance?.getReactInstanceManager(), viewOptions?.ComponentName);
    }

    // TODO:: Return Future ?
    fun  /*Future?*/Unload() {

    }
}