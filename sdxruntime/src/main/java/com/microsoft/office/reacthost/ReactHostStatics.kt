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

    fun ensureInitialized(): Boolean {
        if(initialActivity?.get() == null)
            return false;

        try {
            loadLibs();
        } catch (ex: UnsatisfiedLinkError) {
            return false
        }

        return true
    }

    private fun loadLibs() {
        SoLoader.loadLibrary("reactnativejni");
        SoLoader.loadLibrary("reactrekadroid");
        SoLoader.loadLibrary("sdxruntime");
        SoLoader.loadLibrary("officereacthost");
    }
}