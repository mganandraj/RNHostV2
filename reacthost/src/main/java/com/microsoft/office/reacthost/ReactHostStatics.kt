package com.microsoft.office.reacthost

import android.app.Activity
import com.facebook.soloader.SoLoader
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import com.microsoft.office.plat.logging.Trace
import java.lang.ref.WeakReference

@KeepClassAndMembers
object ReactHostStatics {
    var initialActivity: WeakReference<Activity>? = null

    external fun makeReactHost(reactOptions: ReactOptions): ReactHost

    // This is not required when within Office.
    external fun libletInit();

    fun loadLibs() {
        SoLoader.loadLibrary("reactnativejni");
        SoLoader.loadLibrary("reactrekadroid");
        try {
            SoLoader.loadLibrary("sdxruntime");
        } catch(ex: UnsatisfiedLinkError) {
            Trace.d("LOG_TAG", ex.message);
        }
    }
}