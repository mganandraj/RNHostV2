package com.example.rnhostv2_2;

import com.facebook.jni.HybridData;

public class ReactOptions {

    static {
        System.loadLibrary("fbjni");
        System.loadLibrary("rnhostv2_2");
    }

    private native HybridData initHybrid();
    private final HybridData mHybridData;

    ReactOptions(HybridData hybridData) {
        mHybridData = hybridData;
    }

    public native String getIdentity();
    public native void setIdentity(String identity);

    ReactOptions() {
        mHybridData = initHybrid();
    }


}
