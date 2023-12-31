package com.microsoft.office.reacthostapp

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.microsoft.office.reacthostapp.databinding.FragmentDashboardBinding

class DashboardFragment : Fragment() {

    private var _binding: FragmentDashboardBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentDashboardBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonRnxhosttest.setOnClickListener {
            findNavController().navigate(R.id.action_DashboardFragment_to_RNXHostTestFragment)
        }

        binding.buttonOrnhosttestKt1.setOnClickListener {
            findNavController().navigate(R.id.action_DashboardFragment_to_ORNHostTestKt1Fragment)
        }

        binding.buttonOrnhosttestKtNativeoptions.setOnClickListener {
            findNavController().navigate(R.id.action_DashboardFragment_to_ORNHostTestKtNativeOptionsFragment)
        }

        binding.buttonOrnhosttestHeadless.setOnClickListener {
            findNavController().navigate(R.id.action_dashboardFragment_to_ORNHostHeadlessFragment)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}