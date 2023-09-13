package com.microsoft.office.reactnative.host;

import android.content.Context;

import com.facebook.react.bridge.JavaScriptExecutor;
import com.facebook.react.bridge.JavaScriptExecutorFactory;

import java.lang.ref.WeakReference;

public class OfficeExecutorFactory implements JavaScriptExecutorFactory {
  JavaScriptExecutorFactory m_baseExecutorFactory;
  String[] m_preloadBundles;
  WeakReference<Context> m_hostContext;
  OfficeExecutorObserver m_officeExecutorObserver;

  public OfficeExecutorFactory(Context context, JavaScriptExecutorFactory baseExecutorFactory, String[] preloadBundles, OfficeExecutorObserver officeExecutorObserver) {
    this.m_baseExecutorFactory = baseExecutorFactory;
    this.m_preloadBundles = preloadBundles;
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
    return new OfficeExecutor(context.getAssets(), baseExecutor, m_preloadBundles, m_officeExecutorObserver);
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