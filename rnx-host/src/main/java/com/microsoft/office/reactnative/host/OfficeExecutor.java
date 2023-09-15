package com.microsoft.office.reactnative.host;

import android.content.res.AssetManager;

import com.facebook.jni.HybridData;
import com.facebook.proguard.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.soloader.SoLoader;

@DoNotStrip
public class OfficeExecutor extends JavaScriptExecutor {
  static {
    loadLibrary();
  }

  public static void loadLibrary() throws UnsatisfiedLinkError {
    SoLoader.loadLibrary("rnx-host");
  }

  /* package */ OfficeExecutor(AssetManager assetManager, JavaScriptExecutor baseExecutor, String[] preloadBundles, JSBundle[] platformBundles, OfficeExecutorObserver officeExecutorObserver) {
    super(initHybrid(assetManager, baseExecutor, preloadBundles, platformBundles, officeExecutorObserver));
  }

  @Override
  public String getName() {
    return "OfficeExecutor";
  }

  private static native HybridData initHybrid(AssetManager assetManager, JavaScriptExecutor baseExecutor, String[] preloadBundles, JSBundle[] platformBundles, OfficeExecutorObserver officeExecutorObserver);
}