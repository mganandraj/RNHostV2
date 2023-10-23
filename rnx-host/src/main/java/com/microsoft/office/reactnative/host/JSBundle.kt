package com.microsoft.office.reactnative.host

import android.app.Application
import java.io.File
import java.nio.ByteBuffer
import java.nio.charset.StandardCharsets

// Implementation of JSBundleFetcher in is responsible for ensuring that the bundle is ready for loading.
// For instance the implementation can download the bundle, or extract the bundle from archive.
// The implementation should throw if the bundle can't be made available locally to be loaded.
// --
// Implementation should return a new JSBundle which reflects the state of the bundle after the fetch
// For e.g. a service delivery fetcher or Union AssetExtracter fetcher may populate
// JSBundle.JSBundleInfo.FileName or JSBundle.Content and may reset the Id field.
// --
// fetch method should be synchronous as of now.
interface JSBundleFetcher {
    fun fetch(bundle: JSBundle) : JSBundle;
}

data class JSBundleInfo (
    val Id: String?, // If not null, assumed to be the asset name, either packaged with the app as an Asset. or JSBundleFetcher can understand.
    val FileName: String?, // Used only if Id is not set.
    val Version: Long? // Currently unused.
)

data class JSBundle (
    val Content: ByteBuffer?, //This must be UTF-8 encoded bytes. Can be null in which case we rely on FileName/Id from Info.
    val Info: JSBundleInfo?
)

fun JSBundleFromString(str: String, url: String): JSBundle {
    val utf8Buffer = StandardCharsets.UTF_8.encode(str)
    return JSBundle(ByteBuffer.allocateDirect(utf8Buffer.capacity()).put(utf8Buffer), JSBundleInfo(url, null, null));
}

fun JSBundleFromFileAssetId(assetId: String): JSBundle {
    return JSBundle(null , JSBundleInfo(assetId, null, null));
}

// TODO :: Verify that this works with Hermes bytecodes.
fun JSBundleFromFilePath2(filePath: String, url: String): JSBundle {
    return JSBundle(
        StandardCharsets.UTF_8.encode(File(filePath).readText()) , JSBundleInfo(url, null, null));
}

fun JSBundleFromFilePath(filePath: String): JSBundle {
    return JSBundle(null , JSBundleInfo(null, filePath, null));
}