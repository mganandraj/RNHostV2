package com.microsoft.office.reacthost

import android.app.Activity
import java.lang.ref.WeakReference

object ReactHostStatics {
    var initialActivity: WeakReference<Activity>? = null

    external fun makeReactHost(reactOptions: ReactOptions): ReactHost

    // This is not required when within Office.
    external fun libletInit();
}