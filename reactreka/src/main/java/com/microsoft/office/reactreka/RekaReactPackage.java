package com.microsoft.office.reactreka;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import com.facebook.react.LazyReactPackage;
import com.facebook.react.TurboReactPackage;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.module.model.ReactModuleInfo;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.turbomodule.core.interfaces.TurboModule;

import java.util.HashMap;
import java.util.Map;

public class RekaReactPackage extends TurboReactPackage {

    private RekaBridgeOptions m_rekaBridgeOptions;
    public RekaReactPackage(RekaBridgeOptions rekaBridgeOptions) { m_rekaBridgeOptions = rekaBridgeOptions; }
    public RekaReactPackage() { }

    @Override
    public NativeModule getModule(String name, ReactApplicationContext reactApplicationContext) {
        switch (name) {
            case RekaNativeModule.NAME:
                return new RekaNativeModule(reactApplicationContext, m_rekaBridgeOptions);
            default:
                return null;
        }
    }

    @Override
    public ReactModuleInfoProvider getReactModuleInfoProvider() {
        Class<? extends NativeModule>[] moduleList =
                new Class[]{
                        RekaNativeModule.class
                };

        final Map<String, ReactModuleInfo> reactModuleInfoMap = new HashMap<>();
        for (Class<? extends NativeModule> moduleClass : moduleList) {
            ReactModule reactModule = moduleClass.getAnnotation(ReactModule.class);

            reactModuleInfoMap.put(
                    reactModule.name(),
                    new ReactModuleInfo(
                            reactModule.name(),
                            moduleClass.getName(),
                            reactModule.canOverrideExistingModule(),
                            reactModule.needsEagerInit(),
                            reactModule.hasConstants(),
                            reactModule.isCxxModule(),
                            TurboModule.class.isAssignableFrom(moduleClass)));
        }

        return new ReactModuleInfoProvider() {
            @Override
            public Map<String, ReactModuleInfo> getReactModuleInfos() {
                return reactModuleInfoMap;
            }
        };
    }

    @Keep
    @NonNull
    public static RekaReactPackage GetReactPackage(RekaBridgeOptions rekaBridgeOptions) {
        return new RekaReactPackage(rekaBridgeOptions);
    }

    @Keep
    @NonNull
    public static RekaReactPackage GetReactPackage() {
        return new RekaReactPackage();
    }
}
