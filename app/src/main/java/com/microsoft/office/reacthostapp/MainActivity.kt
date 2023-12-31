package com.microsoft.office.reacthostapp

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import androidx.appcompat.app.AppCompatActivity
import com.facebook.soloader.SoLoader
import com.facebook.soloader.nativeloader.NativeLoader
import com.facebook.soloader.nativeloader.SystemDelegate
import com.microsoft.office.reacthost.*
import com.microsoft.office.reacthost.ReactHostStatics.initialActivity
import java.lang.ref.WeakReference

class MainActivity : AppCompatActivity() {
    var mreactViewInstance: BaseRootView? = null
    var mReactHost: ReactHost? = null
    var mReactViewHost: ReactViewHost? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

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
    }

    override fun onResume() {
        super.onResume()

        mreactViewInstance = BaseRootView(this)

        libletInit();
        runReactOnView(mreactViewInstance as java.lang.Object);

        // testHeadless();
/*


        var reactOptions = ReactOptions()
        reactOptions.identity = "V2App"
        reactOptions.DeveloperSettings.IsDevModeEnabled = true

        reactOptions.AddJavaModuleName("com.microsoft.office.reacthostapp.MyReactPackage")
        reactOptions.instanceCreatedCallback = object: IInstanceCreatedCallback{
            override fun run(instance: ReactInstance?) {
                Log.w("MainActivity", "run");
            }
        };
        // reactOptions.AddRegisteredJSBundle("foundation.android.bundle")
        // reactOptions.AddRegisteredJSBundle("ui.android.bundle")
        reactOptions.AddRegisteredJSBundle("index.android.bundle")

        mReactHost = ReactHostStatics.makeReactHost(reactOptions)

        var viewOptions: ReactViewOptions = ReactViewOptions()
        viewOptions.ComponentName = "AwesomeProject"
        mReactViewHost = mReactHost?.MakeViewHost(viewOptions)

        mReactViewHost?.AttachViewInstance(mreactViewInstance)
*/

        // Delayed because the Popup Windows shown by RN DevSupport can't be done too early.
        // Handler().postDelayed({ this@MainActivity.setContentView(mreactViewInstance) }, 100)
        setContentView(mreactViewInstance)
    }

    companion object {
        init {
            System.loadLibrary("sdxruntime")
            System.loadLibrary("reacthostapp")
        }
    }

    external fun runReactOnView(reactViewInstance: java.lang.Object?)
    external fun libletInit()
    external fun testHeadless()

    external fun test(callback: com.microsoft.office.reacthost.IInstanceCreatedCallback)
}