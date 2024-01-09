package com.microsoft.office.reacthost

import com.microsoft.office.plat.annotation.KeepClassAndMembers

@KeepClassAndMembers
interface ILogHandler {
    fun run(message: String, level: Int)
}