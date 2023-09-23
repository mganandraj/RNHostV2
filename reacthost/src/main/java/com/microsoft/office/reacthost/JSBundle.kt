package com.microsoft.office.reacthost

import java.nio.ByteBuffer

data class JSBundleInfo (
    val Id: String?,
    val FileName: String?,
    val Timestamp: Long?
)

data class JSBundle (
    // Note: This must be UTF-8 encoded bytes
    // Can be null in which case we rely on FileName/Id from Info.
    val Content: ByteBuffer?,
    val Info:JSBundleInfo?
)
