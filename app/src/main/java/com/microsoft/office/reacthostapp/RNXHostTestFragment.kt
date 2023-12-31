package com.microsoft.office.reacthostapp

import android.app.Activity
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.facebook.react.ReactPackage
import com.microsoft.office.reacthost.ReactHostStatics
import com.microsoft.office.reacthostapp.databinding.FragmentRnxhosttestBinding
import com.microsoft.office.reactnative.host.JSBundle
import com.microsoft.office.reactnative.host.ReactNativeHost
import com.microsoft.office.reactnative.reka.RekaReactPackage

class RNXHostTestFragment : Fragment() {

    private var _binding: FragmentRnxhosttestBinding? = null
    private var mReactNativeHost: ReactNativeHost? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        val builder = ReactNativeHost.Builder()
            .activity(ReactHostStatics.initialActivity!!.get()!!)
            .application(ReactHostStatics.initialActivity!!.get()!!.application)
            .isDev(true)
            .jsMainModulePath("index")
            .shouldEagerInit(true)
            .nativeModulePackages(mutableListOf<ReactPackage>(
                MyReactPackage.GetReactPackage(),
                RekaReactPackage.GetReactPackage()
            ))
            .onJSRuntimeInitialized {
                // Log.i(LOG_TAG, "ReactIntegration.RootView.onJSRuntimeInitialized")
            }
            .onJSBundleLoaded {  bundleName: String ->
                // onBundleLoaded(bundleName)
                // mReactOptions.OnInstanceCreated?.run();
            }

        var preloadBundles = ArrayList<JSBundle>()
        // preloadBundles.add(com.microsoft.office.reactnative.host.JSBundleFromFileAssetId( "foundation.android.bundle"))
        // preloadBundles.add(com.microsoft.office.reactnative.host.JSBundleFromFileAssetId( "ui.android.bundle"))
        builder.preloadBundles(preloadBundles)
        builder.userBundle(com.microsoft.office.reactnative.host.JSBundleFromAssetId( "index.android.bundle"))

        this.mReactNativeHost = builder.build()

        _binding = FragmentRnxhosttestBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.reactViewHost.addView(mReactNativeHost!!.createRootView("AwesomeProject", Bundle(), activity as Activity))
        binding.buttonDashboard.setOnClickListener {
            findNavController().navigate(R.id.action_RNXHostTestFragment_to_DashboardFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}