package com.example.rnhostv2_2;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.example.rnhostv2_2.databinding.ActivityMainBinding;
import com.facebook.jni.HybridData;
import com.facebook.soloader.SoLoader;
import com.facebook.soloader.nativeloader.NativeLoader;
import com.facebook.soloader.nativeloader.SystemDelegate;

import java.lang.ref.WeakReference;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("fbjni");
        System.loadLibrary("rnhostv2_2");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /*String json1 = "{\n" +
                "  \"Id\": 78912,\n" +
                "  \"Customer\": \"Jason Sweet\",\n" +
                "  \"Quantity\": 1,\n" +
                "  \"Price\": 18.00\n" +
                "}";
        Bundle bundle1 = UtilsKt.fromJson(json1);
        System.out.printf(bundle1.toString());*/

        ReactHostStatics.INSTANCE.setInitialActivity(new WeakReference< Activity >(MainActivity.this));

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (!Settings.canDrawOverlays(this)) {
                Intent serviceIntent = new Intent(
                        Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
                startActivity(serviceIntent);
                return;
            }
        }

        if (!NativeLoader.isInitialized()) {
            NativeLoader.init(new SystemDelegate());
        }

        SoLoader.init(this.getApplicationContext(), false);
        setContentView(R.layout.activity_main);

        final ReactViewInstance reactViewInstance = new ReactViewInstance(this);
        runReactOnView(reactViewInstance);
        // runReactOnView();
        // ReactOptions options =
        // String identity = options.getIdentity();


        // Delayed because the Popup Windows shown by RN DevSupport can't be done too early.
        new Handler().postDelayed(new Runnable() {
            public void run() {
                MainActivity.this.setContentView(reactViewInstance);
                // show the popup window
                // which view you pass in doesn't matter, it is only used for the window tolken
                // MainActivity.this.setContentView(ReactIntegration.RootView(MainActivity.this));
            }
        }, 100);

    }

    /**
     * A native method that is implemented by the 'rnhostv2_2' native library,
     * which is packaged with this application.
     */
    public static native void runReactOnView(ReactViewInstance reactViewInstance);
}