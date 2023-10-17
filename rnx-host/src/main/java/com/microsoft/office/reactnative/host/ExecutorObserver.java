package com.microsoft.office.reactnative.host;

public interface ExecutorObserver {
    void OnBundleLoaded(String bundleUrl);
    void OnInitialized();
}
