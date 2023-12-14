package com.microsoft.office.reacthostapp

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.os.Parcel
import android.provider.Settings
import android.util.Base64
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import androidx.navigation.ui.setupActionBarWithNavController
import com.facebook.soloader.SoLoader
import com.facebook.soloader.nativeloader.NativeLoader
import com.facebook.soloader.nativeloader.SystemDelegate
import com.google.android.material.snackbar.Snackbar
import com.microsoft.office.reacthost.ReactHostStatics
import com.microsoft.office.reacthostapp.databinding.ActivityLaunchBinding
import org.json.JSONArray
import org.json.JSONException
import org.json.JSONObject
import java.lang.ref.WeakReference


class LaunchActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityLaunchBinding


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        ReactHostStatics.initialActivity = WeakReference(this@LaunchActivity)
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
        // ReactHostStatics.libletInit();


        binding = ActivityLaunchBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setSupportActionBar(binding.toolbar)

        val navController = findNavController(R.id.nav_host_fragment_content_launch)
        appBarConfiguration = AppBarConfiguration(navController.graph)
        setupActionBarWithNavController(navController, appBarConfiguration)

        binding.fab.setOnClickListener { view ->
            Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                .setAction("Action", null).show()
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_content_launch)
        return navController.navigateUp(appBarConfiguration)
                || super.onSupportNavigateUp()
    }

    companion object {
        init {
            System.loadLibrary("sdxruntime")
            System.loadLibrary("reacthostapp")
        }
    }
}