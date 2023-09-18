package com.microsoft.office.reacthostapp

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.provider.Settings
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.facebook.soloader.SoLoader
import com.facebook.soloader.nativeloader.NativeLoader
import com.facebook.soloader.nativeloader.SystemDelegate
import com.microsoft.office.reacthost.*
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import java.lang.ref.WeakReference


class MainActivity : AppCompatActivity() {
    var mreactViewInstance: OfficeReactRootView? = null
    var mReactHost: ReactHost? = null
    var mReactViewHost: ReactViewHost? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

//        test(object: IInstanceCreatedCallback{
//            override fun run(/*instance: ReactInstance?*/) {
//                Log.w("MainActivity", "run");
//            }
//        })

        initialActivity = WeakReference(this@MainActivity)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                val serviceIntent = Intent(
                    Settings.ACTION_MANAGE_OVERLAY_PERMISSION
                )
                startActivity(serviceIntent)
                return
            }
        }
        if (!NativeLoader.isInitialized()) {
            NativeLoader.init(SystemDelegate())
        }
        SoLoader.init(this.applicationContext, false)
        setContentView(R.layout.activity_main)
        mreactViewInstance = OfficeReactRootView(this)

        libletInit();
        // runReactOnView(mreactViewInstance);

        testHeadless();
//
//
//        var reactOptions = ReactOptions()
//        reactOptions.identity = "AwesomeProject"
//
//        // TODO :: This is not intuitive .. Should be more like "reactOptions.JavaModuleNames.add("MyClass")"
//        // TODO :: Avoid copying.
//        var javaModuleNames = ArrayList<String>()
//        javaModuleNames.add("MyClass")
//
//        reactOptions.JavaModuleNames = javaModuleNames;
//        reactOptions.instanceCreatedCallback = object: IInstanceCreatedCallback{
//            override fun run(instance: ReactInstance?) {
//                Log.w("MainActivity", "run");
//            }
//        };

//        var jsBundles = ArrayList<JSBundle>()
//        jsBundles.add(JSBundleFromAssetName("foundation.android.bundle"))
//        jsBundles.add(JSBundleFromAssetName("ui.android.bundle"))
//        reactOptions.JSBundles = jsBundles

//        mReactHost = ReactHostStatics.makeReactHost(reactOptions)
//
//        var viewOptions: ReactViewOptions = ReactViewOptions()
//        viewOptions.ComponentName = "AwesomeProject"
//        mReactViewHost = mReactHost?.MakeViewHost(viewOptions)

        // mReactViewHost?.AttachViewInstance(mreactViewInstance)

        // Delayed because the Popup Windows shown by RN DevSupport can't be done too early.
        Handler().postDelayed({ this@MainActivity.setContentView(mreactViewInstance) }, 100)
    }

    companion object {
        init {
            System.loadLibrary("reacthost")
            System.loadLibrary("reacthostapp")
        }
    }

    external fun runReactOnView(reactViewInstance: OfficeReactRootView?)
    external fun libletInit()
    external fun testHeadless()

    external fun test(callback: com.microsoft.office.reacthost.IInstanceCreatedCallback)
}