package com.microsoft.office.reacthostapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;

import com.facebook.soloader.SoLoader;
import com.facebook.soloader.nativeloader.NativeLoader;
import com.facebook.soloader.nativeloader.SystemDelegate;
import com.microsoft.office.reacthost.OfficeReactRootView;
import com.microsoft.office.reacthost.ReactHostStatics;

import java.lang.ref.WeakReference;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("reacthost");
        System.loadLibrary("reacthostapp");
    }

    OfficeReactRootView mreactViewInstance = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

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

        mreactViewInstance = new OfficeReactRootView(this);
        runReactOnView(mreactViewInstance);


        // Delayed because the Popup Windows shown by RN DevSupport can't be done too early.
        new Handler().postDelayed(new Runnable() {
            public void run() {
                MainActivity.this.setContentView(mreactViewInstance);
            }
        }, 100);

    }

    public static native void runReactOnView(OfficeReactRootView reactViewInstance);
}