package com.microsoft.office.reactnative.host;

import android.content.res.AssetManager;

import com.facebook.jni.HybridData;
import com.facebook.proguard.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.soloader.SoLoader;

@DoNotStrip
public class WrapperJSExecutor extends JavaScriptExecutor {
  static {
    loadLibrary();
  }

  public static void loadLibrary() throws UnsatisfiedLinkError {
    // SoLoader.loadLibrary("mso20android");
    SoLoader.loadLibrary("reactnativejni");
    SoLoader.loadLibrary("rnxreacthost");
  }

  /* package */ WrapperJSExecutor(AssetManager assetManager, JavaScriptExecutor baseExecutor, JSBundle[] platformBundles, JSExecutorObserver officeExecutorObserver) {
    super(initHybrid(assetManager, baseExecutor, platformBundles, officeExecutorObserver));
  }

  @Override
  public String getName() {
    return "OfficeExecutor";
  }

  private static native HybridData initHybrid(AssetManager assetManager, JavaScriptExecutor baseExecutor, JSBundle[] platformBundles, JSExecutorObserver officeExecutorObserver);
}