package com.microsoft.office.reactnative.host

import android.app.Application
import java.io.File
import java.nio.ByteBuffer
import java.nio.charset.StandardCharsets

// Implementation of JSBundleFetcher in is responsible for ensuring that the bundle is ready for loading.
// For instance the implementation could use the "Id" to resolve and can download the bundle, or extract the bundle from an archive.
// The implementation should throw if the bundle can't be made available locally to be loaded.
// --
// Implementation should return a new JSBundle which reflects the state of the bundle after the fetch
// For e.g. a service delivery fetcher or Union AssetExtracter fetcher may populate
// JSBundle.JSBundleInfo.FileName or JSBundle.Content.
// --
// fetch method should be synchronous as of now.
interface JSBundleFetcher {
    fun fetch(bundle: JSBundle, checked: Boolean) : JSBundle;
}

data class JSBundleInfo (
    val Id: String?, // If not null, JSBundleFetcher can use it to resolve and override the FileName.
    val FileName: String?, // It can be a filesystem path or the name of an APK asset.
    val Version: Long?
)

data class JSBundle (
    val Content: ByteBuffer?, //This must be UTF-8 encoded bytes. Can be null in which case we rely on FileName/Id from Info.
    val Info: JSBundleInfo?
)

fun JSBundleFromString(str: String, url: String): JSBundle {
    val utf8Buffer = StandardCharsets.UTF_8.encode(str)
    return JSBundle(ByteBuffer.allocateDirect(utf8Buffer.capacity()).put(utf8Buffer), JSBundleInfo(url, null, null));
}

// Use a template bundle resolvable by a custom JSBundleFetcher.
fun JSBundleFromId(id: String): JSBundle {
    return JSBundle(null , JSBundleInfo(id, null, null));
}

fun JSBundleFromAssetId(assetId: String): JSBundle {
    return JSBundle(null , JSBundleInfo(assetId, assetId, null));
}

// TODO :: Verify that this works with Hermes bytecodes.
fun JSBundleFromFilePath2(filePath: String, url: String): JSBundle {
    return JSBundle(
        StandardCharsets.UTF_8.encode(File(filePath).readText()) , JSBundleInfo(url, null, null));
}

fun JSBundleFromFilePath(filePath: String): JSBundle {
    return JSBundle(null , JSBundleInfo(filePath, filePath, null));
}