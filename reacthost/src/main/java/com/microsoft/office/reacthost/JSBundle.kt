package com.microsoft.office.reacthost

import java.io.File
import java.nio.ByteBuffer
import java.nio.charset.StandardCharsets

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

fun JSBundleFromString(str: String, url: String): JSBundle {
    val utf8Buffer = StandardCharsets.UTF_8.encode(str)
    return JSBundle(ByteBuffer.allocateDirect(utf8Buffer.capacity()).put(utf8Buffer), JSBundleInfo(url, null, null));
}

// TODO :: Verify that this works with Hermes bytecodes.
// TODO :: Very this is performant
fun JSBundleFromFilePath(filePath: String, url: String): JSBundle {
    return JSBundle(
        StandardCharsets.UTF_8.encode(File(filePath).readText()) , JSBundleInfo(url, filePath, null));
}

// We have native code in the JS executor to read the content from the APK assets. Hence we are not doing File IO here.
// We assume the ID as the asset name if the buffer and filePath is not set
fun JSBundleFromAssetName(assetName: String): JSBundle {
    return JSBundle(null , JSBundleInfo(assetName, null, null));
}