package com.microsoft.office.reactreka;

import com.facebook.react.bridge.ReactContext;

import java.lang.ref.WeakReference;

public class ReactContextHolder {
    private final WeakReference<ReactContext> mReactContext;
    public ReactContextHolder(ReactContext reactContext) {
        mReactContext = new WeakReference<ReactContext>(reactContext);
    }

    public ReactContext getReactContext() {
        return mReactContext.get();
    }
}
