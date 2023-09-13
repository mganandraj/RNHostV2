package com.microsoft.office.reacthost

import com.facebook.react.ReactRootView
import com.microsoft.office.reactnative.host.ReactNativeHost

class ReactIntegration {
    companion object {
        @JvmStatic
        fun RootView(activity: MainActivity): ReactRootView {
            val host = ReactNativeHost.Builder()
                    .activity(activity)
                    .application(activity.application)
                    .isDev(true)
                    .jsMainModulePath("index")
                    .build();
            return host?.createRootView("AwesomeProject", null, activity)!!;
        }
    }
}