package com.microsoft.office.reacthostapp

import android.graphics.Color
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.microsoft.office.reacthost.*
import com.microsoft.office.reacthostapp.databinding.FragmentOrnhostkt1Binding

class ORNHostKtNativeOptionsFragment : Fragment() {

    private var _binding: FragmentOrnhostkt1Binding? = null

    // var mreactViewInstance: OfficeReactRootView? = null
    var mReactHost: ReactHost? = null
    var mReactViewHost: ReactViewHost? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        /*if(mReactHost == null) {
            var reactOptions = ReactOptions()
            reactOptions.identity = "V2App"
            reactOptions.DeveloperSettings.IsDevModeEnabled = true

            reactOptions.AddJavaModuleName("com.microsoft.office.reacthostapp.MyReactPackage")
            reactOptions.instanceCreatedCallback = object: IInstanceCreatedCallback {
                override fun run(instance: ReactInstance?) {
                    Log.w("MainActivity", "run");
                }
            };
            // reactOptions.AddRegisteredJSBundle("foundation.android.bundle")
            // reactOptions.AddRegisteredJSBundle("ui.android.bundle")
            reactOptions.AddRegisteredJSBundle("index.android.bundle")
                mReactHost = ReactHostStatics.makeReactHost(reactOptions)
        }*/

        _binding = FragmentOrnhostkt1Binding.inflate(inflater, container, false)
        return binding.root

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        var reactOptions = getReactOptions()
        /*reactOptions.identity = "V2App"
        reactOptions.DeveloperSettings.IsDevModeEnabled = true

        reactOptions.AddJavaModuleName("com.microsoft.office.reacthostapp.MyReactPackage")
        reactOptions.instanceCreatedCallback = object: IInstanceCreatedCallback {
            override fun run(instance: ReactInstance?) {
                Log.w("AutoRootView", "instanceCreatedCallback");
            }
        };
        reactOptions.instanceLoadedCallback = object: IInstanceLoadedCallback {
            override fun run(instance: ReactInstance?, errorCode: ErrorCode?) {
                Log.w("AutoRootView", "instanceLoadedCallback");
            }
        };
        // reactOptions.AddRegisteredJSBundle("foundation.android.bundle")
        // reactOptions.AddRegisteredJSBundle("ui.android.bundle")
        reactOptions.AddRegisteredJSBundle("index.android.bundle")
*/

//        mreactViewInstance = AutoRootView(this.activity, reactOptions, "AwesomeProject", Bundle())
//        var viewOptions: ReactViewOptions = ReactViewOptions()
//        viewOptions.ComponentName = "AwesomeProject"
//        mReactViewHost = mReactHost!!.MakeViewHost(viewOptions)
//        mReactViewHost!!.AttachViewInstance(mreactViewInstance)

        binding.reactViewHost.addView(AutoRootView(this.activity, null, 0, reactOptions, "AwesomeProject", Bundle()))
        binding.reactViewHost.setBackgroundColor(Color.RED)

        binding.buttonDashboard.setOnClickListener {
            findNavController().navigate(R.id.action_ORNHostTestKtNativeOptionsFragment_to_DashboardFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    external fun getReactOptions() : ReactOptions
}