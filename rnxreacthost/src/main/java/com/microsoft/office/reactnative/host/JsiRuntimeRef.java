package com.microsoft.office.reactnative.host;

import com.facebook.jni.HybridData;
import androidx.annotation.Keep;

// Wraps a C++ reference to jsi::Runtime. Don't store an instance outside stack.
@Keep
public class JsiRuntimeRef {
    private HybridData mHybridData;
    public JsiRuntimeRef(HybridData hybridData) {
        mHybridData = hybridData;
    }
}
