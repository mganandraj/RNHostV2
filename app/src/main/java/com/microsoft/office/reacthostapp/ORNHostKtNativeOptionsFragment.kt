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
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentOrnhostkt1Binding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        var reactOptions = getReactOptions()
        binding.reactViewHost.addView(AutoRootView(this.activity, null, 0, reactOptions, "AwesomeProject", Bundle(), ViewListener()))
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