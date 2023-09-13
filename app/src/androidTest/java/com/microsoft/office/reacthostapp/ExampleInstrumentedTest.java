package com.microsoft.office.reacthostapp;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import androidx.test.platform.app.InstrumentationRegistry;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        assertEquals("com.microsoft.office.reacthost", appContext.getPackageName());
    }

    // This doesn't work as unit test ! May be because JSONObject is a platform API
    @Test
    public void jsonToBundle() {
        String json1 = "{\n" +
                "  \"Id\": 78912.3,\n" +
                "  \"Customer\": \"Jason Sweet\",\n" +
                "  \"Quantity\": 1,\n" +
                "  \"Price\": 18.00\n" +
                "}";

        String json2 = "[\"Apple\", 1, true,  {\"name\":\"Leo\", \"rating\":\"VIP\"}]";
        String json3 = "[\n" +
                "  34,\n" +
                "  {\n" +
                "    \"a\": [\n" +
                "      \"0.042062\",\n" +
                "      21000,\n" +
                "      \"21000.00000000\"\n" +
                "    ],\n" +
                "    \"b\": [\n" +
                "      \"0.041960\",\n" +
                "      63306,\n" +
                "      \"63306.95000000\"\n" +
                "    ],\n" +
                "    \"c\": [\n" +
                "      \"0.041960\",\n" +
                "      \"0.00000011\"\n" +
                "    ],\n" +
                "    \"v\": [\n" +
                "      \"2453051.00336514\",\n" +
                "      \"8208496.19778969\"\n" +
                "    ],\n" +
                "    \"p\": [\n" +
                "      \"0.041922\",\n" +
                "      \"0.042844\"\n" +
                "    ],\n" +
                "    \"t\": [\n" +
                "      606,\n" +
                "      1557\n" +
                "    ],\n" +
                "    \"l\": [\n" +
                "      \"0.041383\",\n" +
                "      \"0.041383\"\n" +
                "    ],\n" +
                "    \"h\": [\n" +
                "      \"0.042414\",\n" +
                "      \"0.044987\"\n" +
                "    ],\n" +
                "    \"o\": [\n" +
                "      \"0.042107\",\n" +
                "      \"0.044318\"\n" +
                "    ]\n" +
                "  },\n" +
                "  \"ticker\",\n" +
                "  \"ADA\\/EUR\"\n" +
                "]";

//        Bundle bundle1 = UtilsKt.fromJson(json1);
//        Log.i("ExampleInstrumentedTest:jsonToBundle", "bundle1: " + bundle1.toString());

//        Bundle bundle2 = UtilsKt.fromJson(json2);
//        Log.i("ExampleInstrumentedTest:jsonToBundle", "bundle2: " + bundle2.toString());

        Bundle bundle3 = UtilsKt.fromJson(json3);
        Log.i("ExampleInstrumentedTest:jsonToBundle", "bundle3: " + bundle3.toString());
    }
}