package com.microsoft.office.reacthostapp

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.microsoft.office.reacthost.ReactOptions
import com.microsoft.office.reacthostapp.databinding.FragmentOrnhostHeadlessBinding

class ORNHostHeadlessFragment : Fragment() {

    private var _binding: FragmentOrnhostHeadlessBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentOrnhostHeadlessBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonDashboard.setOnClickListener {
            findNavController().navigate(R.id.action_ORNHostHeadlessFragment_to_dashboardFragment)
        }

        runNative()
    }

    external fun runNative()
}