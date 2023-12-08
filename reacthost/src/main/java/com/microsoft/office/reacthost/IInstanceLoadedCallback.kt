package com.microsoft.office.reacthost

import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
interface IInstanceLoadedCallback {
    fun run(instance: ReactInstance?, errorCode: ErrorCode?)
}