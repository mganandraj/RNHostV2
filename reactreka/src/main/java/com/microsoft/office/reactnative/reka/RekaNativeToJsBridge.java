package com.microsoft.office.reactnative.reka;

import android.util.Log;

import com.facebook.jni.HybridData;
import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.bridge.ReadableNativeMap;
import com.facebook.react.bridge.WritableNativeMap;
import com.microsoft.office.plat.annotation.KeepClassAndMembers;

 @KeepClassAndMembers
public class RekaNativeToJsBridge {

    private static final String LOG_TAG = "RekaNativeToJsBridge";

    private native HybridData initHybrid();
    private final HybridData mHybridData;

    private final ReactContextHolder mReactContextHolder;
    public RekaNativeToJsBridge(ReactContextHolder reactContextHolder) {
        mHybridData = initHybrid();
        mReactContextHolder = reactContextHolder;
    }

    public void CallJSFunction(String methodName, ReadableNativeMap args) {
        // WritableNativeMap currently lacks the java constructor 
        // required to be constuctable from native code. Hence this workaround.
        WritableNativeMap map = new WritableNativeMap();
        map.merge(args);

        ReactContext reactContext = mReactContextHolder.getReactContext();
        if(reactContext != null) {
            reactContext.getCatalystInstance()
                    .callFunction("RekaBridgeEventEmitter", "emit",
                            Arguments.fromJavaArgs(new Object[]{methodName, map}));
        }
    }
}
