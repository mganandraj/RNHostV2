package com.microsoft.office.reactnative.host;

import android.content.Context;

import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.react.bridge.JavaScriptExecutorFactory;

import java.lang.ref.WeakReference;

public class WrapperJSExecutorFactory implements JavaScriptExecutorFactory {
  JavaScriptExecutorFactory m_baseExecutorFactory;
  JSBundle[] m_platformBundles;
  WeakReference<Context> m_hostContext;
  JSExecutorObserver m_officeExecutorObserver;

  public WrapperJSExecutorFactory(Context context, JavaScriptExecutorFactory baseExecutorFactory, JSBundle[] platformBundles, JSExecutorObserver officeExecutorObserver) {
    this.m_baseExecutorFactory = baseExecutorFactory;
    this.m_platformBundles = platformBundles;
    this.m_hostContext = new WeakReference<>(context);
    this.m_officeExecutorObserver = officeExecutorObserver;
  }

  @Override
  public JavaScriptExecutor create() throws Exception {
    Context context = m_hostContext.get();
    if(context == null) {
      throw new IllegalStateException();
    }

    JavaScriptExecutor baseExecutor = m_baseExecutorFactory.create();
    return new WrapperJSExecutor(context.getAssets(), baseExecutor, m_platformBundles, m_officeExecutorObserver);
  }

  @Override
  public void startSamplingProfiler() {
    // Nope
  }

  @Override
  public void stopSamplingProfiler(String filename) {
    // Nope
  }

  @Override
  public String toString() {
    return "OfficeExecutorFactory";
  }
}