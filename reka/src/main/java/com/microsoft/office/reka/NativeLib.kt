package com.microsoft.office.reka

class NativeLib {

    /**
     * A native method that is implemented by the 'reka' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'reka' library on application startup.
        init {
            System.loadLibrary("reka")
        }
    }
}