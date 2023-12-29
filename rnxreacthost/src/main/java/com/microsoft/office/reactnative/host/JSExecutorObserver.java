package com.microsoft.office.reactnative.host;

public interface JSExecutorObserver {
    void OnBundleLoaded(String bundleUrl);
    void OnInitialized();
}
