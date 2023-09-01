package com.example.rnhostv2_2

import android.os.Bundle
import org.json.JSONArray
import org.json.JSONException
import org.json.JSONObject
import org.json.JSONTokener

// Note: This can throw JSONException on malformed inputs.
// Note: These are the valid JSON datatypes
// a string
// a number
// an object (JSON object)
// an array
// a boolean
// null
// We are assuming "Double" type for all numbers
fun fromJson(jsonStr: String): Bundle {
    val json: Any = try { JSONObject(jsonStr) } catch (e : JSONException) {JSONArray(jsonStr)}
    return if (json is JSONObject) {
        fromJson(json as JSONObject)
    } else {
        val bundle = Bundle()
        bundle.putSerializable("array", fromJson(json as JSONArray))
        return bundle
    }
}

fun processValue(key: String?, value: Any, sink: Any) {
    when (value) {
        null ->
            if (sink is Bundle)
                (sink as Bundle).putString(key, null)
            else
                (sink as ArrayList<Any>).add(null as Any)
        is Boolean ->
            if (sink is Bundle)
                (sink as Bundle).putBoolean(key, value)
            else
                (sink as ArrayList<Any>).add(value)
        is String ->
            if (sink is Bundle)
                (sink as Bundle).putString(key, value)
            else
                (sink as ArrayList<Any>).add(value)
        /* Should we specialize for Int ? */
        is Int ->
            if (sink is Bundle)
                (sink as Bundle).putInt(key, value.toInt())
            else
                (sink as ArrayList<Any>).add(value.toInt())
        is Number ->
            if (sink is Bundle)
                (sink as Bundle).putDouble(key, value.toDouble())
            else
                (sink as ArrayList<Any>).add(value.toDouble())
        is JSONObject -> {
            if (sink is Bundle){
                (sink as Bundle).putBundle(key, fromJson(value as JSONObject))
            }
            else
                (sink as ArrayList<Any>).add(fromJson(value as JSONObject))
        }
        is JSONArray -> {
            if (sink is Bundle){
                (sink as Bundle).putSerializable(key, fromJson(value as JSONArray))
            }
            else
                (sink as ArrayList<Any>).add(fromJson(value as JSONArray))
        }
        else -> System.err.println("unable to transform json to bundle $key")
    }
}

fun fromJson(jsonObject: JSONObject) : Bundle {
    val childBundle = Bundle()
    jsonObject.keys().forEach {
        processValue(it, jsonObject.get(it) as Object, childBundle)
    }
    return childBundle;
}

fun fromJson(jsonArray: JSONArray): ArrayList<Any> {
    val childList = ArrayList<Any>()
    for (i in 0 until jsonArray.length()) {
        val obj = jsonArray.get(i)
        processValue(null, obj as Any, childList)
    }
    return childList
}