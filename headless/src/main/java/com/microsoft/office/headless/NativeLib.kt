package com.microsoft.office.headless

class NativeLib {

    /**
     * A native method that is implemented by the 'headless' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'headless' library on application startup.
        init {
            System.loadLibrary("headless")
        }
    }
}