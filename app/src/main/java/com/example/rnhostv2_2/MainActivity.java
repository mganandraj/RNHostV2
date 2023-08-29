package com.example.rnhostv2_2;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.rnhostv2_2.databinding.ActivityMainBinding;
import com.facebook.soloader.nativeloader.NativeLoader;
import com.facebook.soloader.nativeloader.SystemDelegate;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'rnhostv2_2' library on application startup.
    static {
        System.loadLibrary("fbjni");
        System.loadLibrary("rnhostv2_2");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!NativeLoader.isInitialized()) {
            NativeLoader.init(new SystemDelegate());
        }

        com.facebook.jni.HybridData hybridData = new com.facebook.jni.HybridData();

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());

    }

    /**
     * A native method that is implemented by the 'rnhostv2_2' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}