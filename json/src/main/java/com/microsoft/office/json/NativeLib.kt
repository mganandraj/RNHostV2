package com.microsoft.office.json

class NativeLib {

    /**
     * A native method that is implemented by the 'json' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'json' library on application startup.
        init {
            System.loadLibrary("json")
        }
    }
}