package com.microsoft.office.reacthost

import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
interface IInstanceCreatedCallback {
    fun run(instance: ReactInstance?)
}