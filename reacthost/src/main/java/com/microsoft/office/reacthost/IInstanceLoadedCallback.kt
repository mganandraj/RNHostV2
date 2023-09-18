package com.microsoft.office.reacthost

interface IInstanceLoadedCallback {
    fun run(instance: ReactInstance?, errorCode: ErrorCode?)
}