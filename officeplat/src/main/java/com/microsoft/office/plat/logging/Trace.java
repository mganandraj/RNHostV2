/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 */
package com.microsoft.office.plat.logging;

import java.lang.Throwable;

import com.microsoft.office.plat.SystemProperties;
import com.microsoft.office.plat.annotation.*;
// import com.microsoft.office.plat.preference.PreferencesUtils;
import com.microsoft.office.plat.AppPackageInfo;

import android.content.Context;
import android.util.Log;

/**
 * The class implements the file diagnostic logging and performance logging.
 */
@KeepClassAndMembers
public class Trace
{
    /**
     * Local Cached value indicating isDebugVersion
     */
    private static Boolean isDebugVersion = null;

    /**
     * Indicates if the trace is enabled or not.
     */
    private static boolean traceEnabled = false;

    /**
     * Default level for logging. This can be updated using System property
     * log.tag.OfficeApp. For eg.
     * adb shell setprop log.tag.OfficeApp DEBUG
     * overridden by System.setProperty
     */
    private static int logLevel = Log.WARN;

    /**
     * Initializes the trace component.
     */
    public static void initialize(Context context, boolean traceEnabled)
    {
        updateLevelWithSystemProp(context, traceEnabled);
        Trace.setTracingEnabled(traceEnabled);
    }

    /**
     * This method updates loglevel from Shared preference - LOG_LEVEL.
     * This method should be used to set early boot Trace level.
     * @param context
     */
    public static void updateLogLevelFromSharedPreference(Context context)
    {
        // logLevel = PreferencesUtils.getInteger(context,"LOG_LEVEL",Log.WARN);
    }

    /**
     * This method updates the logLevel with the system property log.tag.OfficeApp
     * This method should be removed at ship time (WI 3301324)
     * @param traceEnabled
     */
    private static void updateLevelWithSystemProp(Context context, boolean traceEnabled)
    {
        String logProperty = "log.tag.OfficeApp";
        if (isDebugVersion() && traceEnabled)
        {
            logLevel = Log.VERBOSE;
        }

        // override everything with app/process setting
        updateLogLevel(SystemProperties.getSystemLocalProp(logProperty));
    }

    /**
     */
    private static void updateLogLevel(String systemLogProperty)
    {
        if ("VERBOSE".equalsIgnoreCase(systemLogProperty))
        {
            logLevel = Log.VERBOSE;
        }
        else if ("DEBUG".equalsIgnoreCase(systemLogProperty))
        {
            logLevel = Log.DEBUG;
        }
        else if ("INFO".equalsIgnoreCase(systemLogProperty))
        {
            logLevel = Log.INFO;
        }
        else if ("WARNING".equalsIgnoreCase(systemLogProperty))
        {
            logLevel = Log.WARN;
        }
        else if ("ERROR".equalsIgnoreCase(systemLogProperty))
        {
            logLevel = Log.ERROR;
        }
        else
        {
            logLevel = Log.ASSERT + 1;
        }
    }

    /**
     * Sets the tracing enable state.
     *
     * @param value
     *            the new tracing enable state.
     */
    private static void setTracingEnabled(boolean value)
    {
        Trace.traceEnabled = value;
        Trace.setTracingLevelNative(logLevel);
    }

    /**
     * Send a INFO log message.
     *
     * @param tag
     *            Used to identify the source of a log message. It usually identifies the class or activity where the
     *            log call occurs.
     * @param message
     *            The message you would like logged.
     */
    public static void i(String tag, String message)
    {
        Trace.log(Log.INFO, tag, message);
    }

    /**
     * Send a DEBUG log message.
     *
     * @param tag
     *            Used to identify the source of a log message. It usually identifies the class or activity where the
     *            log call occurs.
     * @param message
     *            The message you would like logged.
     */
    public static void d(String tag, String message)
    {
        Trace.log(Log.DEBUG, tag, message);
    }

    /**
     * Send a VERBOSE log message.
     *
     * @param tag
     *            Used to identify the source of a log message. It usually identifies the class or activity where the
     *            log call occurs.
     * @param message
     *            The message you would like logged.
     */
    public static void v(String tag, String message)
    {
        Trace.log(Log.VERBOSE, tag, message);
    }

    /**
     * Send a WARN log message.
     *
     * @param tag
     *            Used to identify the source of a log message. It usually identifies the class or activity where the
     *            log call occurs.
     * @param message
     *            The message you would like logged.
     */
    public static void w(String tag, String message)
    {
        Trace.log(Log.WARN, tag, message);
    }

    /**
     * Send a ERROR log message.
     *
     * @param tag
     *            Used to identify the source of a log message. It usually identifies the class or activity where the
     *            log call occurs.
     * @param message
     *            The message you would like logged.
     */
    public static void e(String tag, String message)
    {
        Trace.log(Log.ERROR, tag, message);
    }

    public static void e(String tag, String message, Throwable throwable)
    {
        Trace.log(Log.ERROR, tag, message, throwable);
    }


    /**
     * Get a loggable stack trace from a throwable without messages
     *
     * @param exception
     *        An exception to log
     * @return
     *        the string of the exception stack trace without messages
     */
    private static String getStackTraceWithoutMessages(Throwable exception)
    {
        StringBuffer sb = new StringBuffer();
        StackTraceElement[] frames = null;

        if( exception == null)
        {
            return "";
        }

        sb.append(exception.getClass().getName()+"\n");

        frames = exception.getStackTrace();
        if( frames != null && frames.length > 0 )
        {
            for (StackTraceElement frame : frames)
            {
                sb.append("\t"+"at ");
                sb.append(frame.toString()).append("\n");
            }
        }

        while( (exception = exception.getCause()) != null )
        {
            sb.append("Caused By: ");
            sb.append(exception.getClass().getName()+"\n");
            frames = exception.getStackTrace();
            if( frames != null && frames.length > 0 )
            {
                for ( StackTraceElement frame : frames )
                {
                    sb.append("\t"+"at ");
                    sb.append(frame.toString()).append("\n");
                }
            }
        }

        return sb.toString();
    }


    /**
     * Get a loggable stack trace from a throwable
     *
     * @param e
     *        An exception to log
     * @return
     *        the string of the exception stack trace
     */
    public static String getStackTraceString(Throwable e)
    {
        if(AppPackageInfo.isReleaseApk())
        {
            return getStackTraceWithoutMessages(e);
        }
        else
        {
            return Log.getStackTraceString(e);
        }
    }

    /**
     * To tell if the application is debug version
     * @return
     *      true if the version is debug, otherwise false.
     */
    public static boolean isDebugVersion()
    {
        if (isDebugVersion == null)
        {
            isDebugVersion = Boolean.valueOf(Trace.isDebugVersionNative());
        }
        return isDebugVersion.booleanValue();
    }

    /**
     * Checks to see whether or not a log for the specified tag is loggable at the specified level.
     *
     * @param traceLevel
     *            The trace level to check.
     * @return
     *            Whether or not that this is allowed to be logged.
     */
    public static boolean isLoggable(int traceLevel)
    {
        return traceLevel >= logLevel;
    }

    /**
     * Logs a message to both logcat
     *
     * @param priority
     *            the logging priority
     * @param component
     *            the logging component
     * @param message
     *            the logging message
     */
    private static void log(int priority, String component, String message)
    {
        if (isLoggable(priority))
        {
            if(message == null)
            {
                message = "";
            }
            Log.println(priority, component, message);
        }
    }

    /**
     * Logs a message to both logcat.
     *
     * @param priority
     *            the logging priority
     * @param component
     *            the logging component
     * @param message
     *            the logging message
     * @param throwable
     *            the Throwables to be logged
     */
    private static void log(int priority, String component, String message, Throwable throwable)
    {
        if (isLoggable(priority))
        {
            if(message == null)
            {
                message = "";
            }
            String fullMessage = String.format("%s \n %s", message, Log.getStackTraceString(throwable));
            Log.println(priority, component, fullMessage);
        }
    }

    /**
     * Native call to infer debug version.
     * @return If native code is debug build
     */
    // private static native boolean isDebugVersionNative();
    // private static native void setTracingLevelNative(int logLevel);
    private static boolean isDebugVersionNative() { return true;}
    private static void setTracingLevelNative(int logLevel) {}
}