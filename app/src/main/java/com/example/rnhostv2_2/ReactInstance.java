package com.example.rnhostv2_2;

import com.facebook.jni.HybridData;

public class ReactInstance {

    private native HybridData initHybrid();
    private final HybridData mHybridData;

    private final ReactOptions mReactOptions;
    ReactInstance(ReactOptions reactOptions) {
        mHybridData = initHybrid();
        mReactOptions = reactOptions;
    }
}
