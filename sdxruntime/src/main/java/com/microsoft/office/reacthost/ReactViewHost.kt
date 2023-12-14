package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers
import com.microsoft.office.reacthost.ReactViewOptions
import com.microsoft.office.reacthost.ReactHost
import java.lang.ref.WeakReference

@KeepClassAndMembers
class ReactViewHost {
    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

    //! Returns a copy of current react view options.
    fun Options(): ReactViewOptions? {
        return null
    }

    //! Returns IReactHost associated with this IReactViewHost.
    fun ReactHost(): WeakReference<ReactHost>? {
        return null
    }

    //! Reloads the IReactViewInstance if it is attached.
    /* Mso::Future<void>*/
    fun ReloadViewInstance() {
        throw NotImplementedError()
    }

    //! Reloads IReactViewInstance if it is attached with a new set of options.
    /* Mso::Future<void>*/
    fun ReloadViewInstanceWithOptions(@Suppress("UNUSED_PARAMETER") options: ReactViewOptions?) {
        throw NotImplementedError()
    }

    //! Unloads the attached IReactViewInstance.
    /*Mso::Future<void>*/
    fun UnloadViewInstance() {
        throw NotImplementedError()
    }

    //! Attaches IReactViewInstance to the IReactViewHost.
    /*Mso::Future<void>*/
    external fun AttachViewInstance(viewInstance: BaseRootView?)

    //! Detaches IReactViewInstance from the IReactViewHost.
    /*virtual */
    public fun DetachViewInstance() {

    }
}