package com.microsoft.office.reacthost

import android.os.Bundle
import android.os.Parcel
import android.util.Base64
import com.microsoft.office.plat.logging.Trace
import org.json.JSONArray
import org.json.JSONException
import org.json.JSONObject

// Note: This can throw JSONException on malformed inputs.
// Note: These are the valid JSON datatypes
// a string
// a number
// an object (JSON object)
// an array
// a boolean
// null
// We are assuming "Double" type for all numbers
fun fromJson(jsonStr: String?): Bundle {
    if(jsonStr.isNullOrEmpty())
        return Bundle()
    val json: Any = try { JSONObject(jsonStr) } catch (e : JSONException) {JSONArray(jsonStr)}
    return if (json is JSONObject) {
        fromJson(json)
    } else {
        val bundle = Bundle()
        bundle.putSerializable("array", fromJson(json as JSONArray))
        return bundle
    }
}

fun processValue(key: String?, value: Any?, sink: ArrayList<Any?>) {
    when (value) {
        null, is Boolean, is String -> {
            sink.add(value)
        }
        is JSONObject -> {
            sink.add(fromJson(value))
        }
        is JSONArray -> {
            sink.add(fromJson(value))
        }
        is Int -> {
            sink.add(value.toInt())
        }
        is Number -> {
            sink.add(value.toDouble())
        }
        else -> {
            System.err.println("unable to transform json to bundle $key")
        }
    }
}

fun processValue(key: String?, value: Any?, sink: Bundle) {
    when (value) {
        null ->
                sink.putString(key, null)
        is Boolean ->
                sink.putBoolean(key, value)
        is String ->
                sink.putString(key, value)
        is Int ->
                sink.putInt(key, value.toInt())
        is Number ->
                sink.putDouble(key, value.toDouble())
        is JSONObject ->
                sink.putBundle(key, fromJson(value))
        is JSONArray ->
                sink.putSerializable(key, fromJson(value))
        else -> System.err.println("unable to transform json to bundle $key")
    }
}


fun fromJson(jsonObject: JSONObject) : Bundle {
    val childBundle = Bundle()
    jsonObject.keys().forEach {
        processValue(it, jsonObject.get(it) as Any, childBundle)
    }
    return childBundle
}

fun fromJson(jsonArray: JSONArray): ArrayList<Any?> {
    val childList = ArrayList<Any?>()
    for (i in 0 until jsonArray.length()) {
        val obj = jsonArray.get(i)
        processValue(null, obj as Any, childList)
    }
    return childList
}

fun serializeBundle(bundle: Bundle): String {
    val parcel = Parcel.obtain()
    bundle.writeToParcel(parcel, 0)
    return Base64.encodeToString(parcel.marshall(), 0)
}

fun deSerializeBundle(str: String): Bundle {
    val bytes = Base64.decode(str, 0)
    val parcel = Parcel.obtain()
    parcel.unmarshall(bytes, 0, bytes.size)
    parcel.setDataPosition(0);
    var bundle = Bundle()
    bundle.readFromParcel(parcel)
    return bundle
}