package com.microsoft.office.reacthost

import android.app.Activity
import android.app.Application
import android.os.Bundle
import android.os.Looper
import com.facebook.soloader.SoLoader
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import com.microsoft.office.plat.logging.Trace
import java.lang.ref.WeakReference

@KeepClassAndMembers
object ReactHostStatics {
    private var initialActivity_:WeakReference<Activity>? = null
    private var initialActivityInitialized_ = false
    var initialActivity: WeakReference<Activity>
        get() {
            return initialActivity_!!
        }
        set (newActivity) {
            if(!Looper.getMainLooper().isCurrentThread)
                throw IllegalStateException("ReactHostStatics.initialActivity must be initialized on UI thread.")

            if(!initialActivityInitialized_) {
                newActivity.get()!!.application.registerActivityLifecycleCallbacks(object : Application.ActivityLifecycleCallbacks {
                    override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {
                    }

                    override fun onActivityStarted(activity: Activity) {
                    }

                    override fun onActivityResumed(activity: Activity) {
                        initialActivity_ = WeakReference(activity)
                    }

                    override fun onActivityPaused(activity: Activity) {
                    }

                    override fun onActivityStopped(activity: Activity) {
                    }

                    override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle) {
                    }

                    override fun onActivityDestroyed(activity: Activity) {
                    }
                })
                initialActivityInitialized_=true;
            }
            initialActivity_ = newActivity
        }

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