package com.microsoft.office.reacthostapp

import android.graphics.Color
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.microsoft.office.reacthost.*
import com.microsoft.office.reacthostapp.databinding.FragmentFirstBinding

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class FirstFragment : Fragment() {

    private var _binding: FragmentFirstBinding? = null

    var mreactViewInstance: OfficeReactRootView? = null
    var mReactHost: ReactHost? = null
    var mReactViewHost: ReactViewHost? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        if(mReactHost == null) {
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
        }

        _binding = FragmentFirstBinding.inflate(inflater, container, false)
        return binding.root

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        mreactViewInstance = OfficeReactRootView(this.activity)
        var viewOptions: ReactViewOptions = ReactViewOptions()
        viewOptions.ComponentName = "AwesomeProject"
        mReactViewHost = mReactHost!!.MakeViewHost(viewOptions)
        mReactViewHost!!.AttachViewInstance(mreactViewInstance)

        binding.reactViewHost.addView(mreactViewInstance)
        binding.reactViewHost.setBackgroundColor(Color.RED)

        binding.buttonFirst.setOnClickListener {
            findNavController().navigate(R.id.action_FirstFragment_to_SecondFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}