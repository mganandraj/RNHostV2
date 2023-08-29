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

    ReactOptions() {
        mHybridData = initHybrid();
    }


}
