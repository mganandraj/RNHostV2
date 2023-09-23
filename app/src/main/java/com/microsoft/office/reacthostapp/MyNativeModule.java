package com.microsoft.office.reacthostapp;

import android.util.Log;

import com.facebook.jni.HybridData;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.ReadableNativeMap;
import com.facebook.react.module.annotations.ReactModule;

import com.facebook.soloader.SoLoader;
//import com.microsoft.office.plat.logging.Trace;
//import com.microsoft.office.plat.annotation.KeepClassAndMembers;

import java.util.Map;

@ReactModule(name = MyNativeModule.NAME)
public class MyNativeModule extends ReactContextBaseJavaModule
{
    public static final String NAME = "MyNativeModule";
    private static final String LOG_TAG = "MyNativeModule";

    public MyNativeModule(ReactApplicationContext reactContext)
    {
        super(reactContext);
    }

    @Override
    public String getName()
    {
        return NAME;
    }

    @ReactMethod
    public void hello()
    {
        Log.i(LOG_TAG, "MyNativeModule::Hello");
    }
}