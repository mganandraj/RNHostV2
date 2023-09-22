package com.microsoft.office.reactreka;

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

@ReactModule(name = RekaNativeModule.NAME)
//@KeepClassAndMembers
public class RekaNativeModule extends ReactContextBaseJavaModule
{
    public static final String NAME = "RekaBridge";
    private static final String LOG_TAG = "RekaNativeModule";

    private final HybridData mHybridData;
    private native HybridData initHybrid(ReactContextHolder contextHolder, RekaBridgeOptions rekaBridgeOptions);
    private RekaBridgeOptions m_rekaBridgeOptions;

    static {
        try {
            // SoLoader.loadLibrary("mso20android");
            SoLoader.loadLibrary("reactreka");
        } catch (Exception ex) {
            Log.d("LOG_TAG", ex.getMessage());
        }
    }

    public RekaNativeModule(ReactApplicationContext reactContext, RekaBridgeOptions rekaBridgeOptions)
    {
        super(reactContext);
        m_rekaBridgeOptions = rekaBridgeOptions;
        mHybridData = initHybrid(new ReactContextHolder(reactContext), m_rekaBridgeOptions);
    }

    @Override
    public String getName()
    {
        return NAME;
    }

    @Override
    public Map<String, Object> getConstants()
    {
        ReadableNativeMap nativeMap = getConstantsImpl();
        return nativeMap.toHashMap();
    }

    @ReactMethod
    public void invokeNative(ReadableMap args)
    {
        invokeNativeImpl((ReadableNativeMap)args);
    }

    public native void invokeNativeImpl(ReadableNativeMap args);
    public native ReadableNativeMap getConstantsImpl();
}