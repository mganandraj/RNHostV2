package com.microsoft.office.reactnative.host;

public interface OfficeExecutorObserver {
    void OnBundleLoaded(String bundleUrl);
    void OnInitialized();
}
