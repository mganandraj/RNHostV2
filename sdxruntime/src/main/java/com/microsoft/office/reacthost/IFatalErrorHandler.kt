package com.microsoft.office.reacthost

import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
interface IFatalErrorHandler {
    fun run(message: String)
}