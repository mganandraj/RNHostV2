/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 */
package com.microsoft.office.plat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.ConcurrentHashMap;

public class SystemProperties
{
    /**
     * On-Demand cache for storing the read properties. Cache avoids JNI calls everytime
     */
    private static final ConcurrentHashMap<String, String> propCache = new ConcurrentHashMap<String, String>();

    /**
     * Get the value for the given key from the Global system properties.
     * Since this API depends on plat.so's code, use this API only if plat.so is loaded.
     *
     * @return an empty string if the key isn't found else the value.
     */
    public static String getSystemGlobalPropEx(String key)
    {
        String propertyValue = "";

        if (key == null)
        {
            return propertyValue;
        }

        if (propCache.containsKey(key))
        {
            return propCache.get(key);
        }

        propertyValue = getSystemGlobalPropNative(key);
        propCache.put(key, propertyValue);

        return propertyValue;
    }

    /**
     * Get the value for the given key from the Global system properties.
     * Prefer to use getSystemGlobalPropEx if it is sure that plat.so loaded before calling this function.
     * getSystemGlobalPropEx is better in performace than getSystemGlobalProp.
     *
     * @return an empty string if the key isn't found else the value.
     */
    public static String getSystemGlobalProp(String key)
    {
        String propertyValue = null;
        BufferedReader reader = null;
        try
        {
            Process process = Runtime.getRuntime().exec("getprop " + key);
            reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            propertyValue = reader.readLine();
            if(propertyValue != null)
                propertyValue = propertyValue.trim();
        }
        catch (IOException ex)
        {
            // do nothing
        }
        finally
        {
            if (reader != null)
            {
                try
                {
                    reader.close();
                }
                catch (IOException e)
                {
                    // do nothing
                }
            }
        }

        return propertyValue == null ? "" : propertyValue;
    }

    /**
     * Get the value for the given key from the Local system properties.
     *
     *
     * @return an empty string if the key isn't found else the value.
     */
    public static String getSystemLocalProp(String key)
    {
        String propertyValue = "";
        try
        {
            propertyValue = System.getProperty(key);
        }
        catch (SecurityException | NullPointerException | IllegalArgumentException e)
        {
            // DO nothing.
        }
        return propertyValue;
    }

    /**
     * Get the value for the given key from the Local system properties.
     *
     * @return the previous value of the system property,
     *             or empty string if it did not have one.
     */
    public static String setSystemLocalProp(String key, String value)
    {
        return System.setProperty(key, value);
    }

    /**
     * Native function to get read the system property.
     *
     * @return the value of given system property
     *             or empty string if it did not have one.
     */
    public static native String getSystemGlobalPropNative(String key);
}
