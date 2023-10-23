package com.microsoft.office.reactnative.host

import android.app.Application
// import com.microsoft.office.plat.assets.OfficeAssetsManagerUtil
// import com.microsoft.office.plat.FileUtils
import java.io.IOException
import java.util.Arrays

class OfficeBundleFetcher  constructor (
    private val application: Application) : JSBundleFetcher {


    // In defaultAssetList we keep the list of assets in assets root folder
    // Currently all feature bundles, except ones coming from APK extension file
    // are placed in assets root dir and not inside any subdir within it, hence
    // we are fetching this list once to avoid doing it for each iteration of loop
    companion object var defaultAssetList: List<String>? = null

    fun getDefaultAssetList(application: Application) :List<String>?{
        if (defaultAssetList == null) {
            try {
                defaultAssetList = Arrays.asList(*application.assets.list(""))
            } catch (e: IOException) {
                throw e
            }
        }

        return defaultAssetList
    }

    private fun isJSBundleFilePath(bundleName: String): Boolean {
        return bundleName.startsWith("/")
    }

    private val ASSETS_URL_PREFIX = "assets://"
    private fun getAssetUrl(bundleName: String): String? {
        return ASSETS_URL_PREFIX + bundleName
    }

    private fun assetExists(application: Application, assetName: String): Boolean {
        var ret = false
        val index = assetName.lastIndexOf('/')

        // If the assetName indicates it is placed in a subdir in assets,
        // check if it actually exists in that dir, else just check in the
        // default list
        if (index > 0) {
            var actualAssetName = assetName
            var prefix = ""
            // If the asset name is images/abc.png, we make prefix = images
            // and actualAssetName = abc.png
            prefix = assetName.substring(0, index)
            actualAssetName = assetName.substring(index + 1)
            try {
                ret = Arrays.asList(*application.assets.list(prefix)).contains(actualAssetName)
            } catch (e: IOException) {
                throw e
            }
        } else {
            ret = getDefaultAssetList(application)?.contains(assetName) == true
        }
        return ret
    }

    override fun fetch(bundle: JSBundle): JSBundle {

        /*

        // Noop if the bundle already has data
        if (bundle.Content != null)
            return bundle;

        if (bundle.Info == null)
            throw IllegalArgumentException("JSBundle.Info must be available")

        if (bundle.Info.Id != null) {
            // Noop, if Info.Id is populated and a named asset is available
            if(assetExists(application, bundle.Info.Id))
                return bundle;


            // Look for the bundle in the asset cache
            val bundleFullyQualifiedPath: String = OfficeAssetsManagerUtil.getAssetCacheDirectory() + "/" + bundle.Info.Id
            if (FileUtils.fileExists(bundleFullyQualifiedPath)) {
                return JSBundle(null , JSBundleInfo(null, bundleFullyQualifiedPath, null));
            }

            // Try to extract the bundle to asset cache
            OfficeAssetsManagerUtil.extractAssetFromAsset(bundle.Info.Id)
            if (FileUtils.fileExists(bundleFullyQualifiedPath)) {
                return JSBundle(null , JSBundleInfo(null, bundleFullyQualifiedPath, null));
            }
        }

        if (bundle.Info.FileName != null && FileUtils.fileExists(bundle.Info.FileName)) {
            return JSBundle(null , JSBundleInfo(null, bundle.Info.FileName, null));
        }
*/
        throw RuntimeException("Unable to fetch the bundle: Info.Id: ${bundle.Info?.Id}  :  Info.FileName: ${bundle.Info?.FileName}")
    }
}