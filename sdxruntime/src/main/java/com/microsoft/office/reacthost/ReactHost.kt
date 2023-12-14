package com.microsoft.office.reacthost

import com.facebook.jni.HybridData
import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
class ReactHost {

    private val mHybridData: HybridData
    internal constructor(hybridData: HybridData) {
        mHybridData = hybridData
    }

	//! Returns a copy of react options.
    external fun Options(): ReactOptions?

    //! Returns current ReactInstance. The result can be null if instance is unloaded.
    external fun Instance() : ReactInstance?

    // Note :: We can project ReactContext to Kotline in future if needed.
    //! Gets Reka context proxy to queue up and run Reka proxies that call JavaScript services or JavaScript event handlers.
    // virtual Mso::JSHost::IRekaContext& RekaContext() noexcept = 0;

    //! A native sequential queue associated with IReactHost.
    //! All internal operations are done using this queue to synchronize internal state.
    // virtual Mso::IDispatchQueue& NativeQueue() const noexcept = 0;
    // virtual Mso::DispatchQueue NativeQueue() const noexcept = 0;

    //! Reloads the ReactNative instance and associated ReactViews.
    /*Mso::Future<void>*/ external fun ReloadInstance()

    //! Updates ReactOptions associated with the IReactHost and reloads the ReactNative instance.
    /*Mso::Future<void>*/ external fun ReloadInstanceWithOptions(options: ReactOptions)

    //! Unloads the ReactNative instance and associated ReactViews.
    /*Mso::Future<void>*/ external fun UnloadInstance()

    //! Creates a new instance of IReactViewHost.
    //! The IReactViewHost is added to the list of view hosts only after a IReactViewInstance is attached to it.
    external fun MakeViewHost(options: ReactViewOptions): ReactViewHost?

    //! Returns a snapshot of IReactViewHost instances associated with the IReactHost.
    // Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> GetViewHostList() noexcept = 0;
}