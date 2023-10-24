package com.microsoft.office.plat;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.text.TextUtils;

import com.microsoft.office.plat.annotation.KeepClassAndMembers;
import com.microsoft.office.plat.logging.Trace;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

@KeepClassAndMembers
public final class FileUtils
{
    private static final String LOG_TAG = "FileUtils";

    /**
     * @param filePath
     * @return true if file exists at the given path, false otherwise
     */
    public static boolean fileExists(String filePath)
    {
        if (!TextUtils.isEmpty(filePath))
        {
            File file = new File(filePath);
            return file.exists();
        }
        return false;
    }

    /**
     * Reads all lines from the file located at the input path.
     *
     * @param filePath path of the file.
     * @return Array of lines in the file.
     * @throws FileNotFoundException if file is not accessible
     */
    public static List<String> readAllLines(String filePath) throws FileNotFoundException
    {

        List<String> result = new ArrayList<>();
        Scanner sc = null;
        try
        {
            FileInputStream fis = new FileInputStream(filePath);
            sc = new Scanner(fis);
            while (sc.hasNextLine())
            {
                result.add(sc.nextLine());
            }
        }
        finally
        {
            if (sc != null)
            {
                sc.close();
            }
        }
        return result;
    }

    /**
     * Deletes all the files and subdirectories present in the directory @directory. if @deleteRootDirectory is set it also deletes the root directory
     *
     * @param directory           root directory
     * @param deleteRootDirectory specifies whether to delete the root directory as well.
     */
    public static void clearDirectoryContents(File directory, boolean deleteRootDirectory)
    {
        if (directory == null)
        {
            return;
        }
        File[] files = directory.listFiles();
        if (files != null)
        {
            for (File f : files)
            {
                if (f.isDirectory())
                {
                    clearDirectoryContents(f, true);
                }
                else
                {
                    f.delete();
                }
            }
        }
        if (deleteRootDirectory)
        {
            directory.delete();
        }
    }

    /**
     * Creates directory if missing.
     *
     * @param directoryPath input directory path.
     * @return returns true if either directory already exists or directory creation is successful, false otherwise
     */
    public static boolean createDirectoryIfMissing(String directoryPath)
    {
        File f = new File(directoryPath);
        if (f.exists())
        {
            if (f.isDirectory())
            {
                return true;
            }
            else
            {
                Trace.e(LOG_TAG, "File already exists, unable to create directory " + directoryPath);
                return false;
            }
        }
        else
        {
            return f.mkdirs();
        }
    }

    /**
     * This method tries to get the local file path if the contentUri is representing a local file in the file system.
     * Most of the file browser apps would give us such a content provider uri.
     *
     * @param context    - A valid context
     * @param contentUri - Input content uri
     * @return string which will be the local file path. null if the local file path cannot be determined.
     */
    public static String tryGetPathIfLocalFileProvider(Context context, Uri contentUri)
    {
        String filePath = null;

        filePath = tryGetPathFromCursor(context, contentUri);
        if(!TextUtils.isEmpty(filePath))
        {
            return filePath;
        }

        filePath = tryGetPathFromContentUriSegments(contentUri);
        if(!TextUtils.isEmpty(filePath))
        {
            return filePath;
        }

        // We really want to know when filePath is still empty for a local content provider, by adding a telemetry log here.
        // But, we don;t know a deterministic way to differentiate local content uri from non local ones such as email attachments/ cloud providers
        // etc.
        Trace.d(LOG_TAG, "FilePathProvider.tryGetPathIfLocalFileProvider - unable to find local file path");
        return filePath;
    }

    private static String tryGetPathFromCursor(Context context, Uri contentUri)
    {
        Cursor cursor = null;
        String filePath = null;

        try
        {
            cursor = context.getContentResolver().query(contentUri, new String[]{MediaStore.MediaColumns.DATA}, null, null, null);
        }
        catch (Exception e)
        {
            //There was an exception while querying the above projection from content-provider
            //It may happen if corresponding content provider is not email client and is not handling certain error cases
            //So continue with case when cursor=null;
            Trace.e(LOG_TAG, "FilePathProvider.tryGetPathIfLocalFileProvider exception in cr.query while trying to compute local path: ");
        }

        if (cursor != null && cursor.moveToFirst())
        {
            int dataColumnIndex = cursor.getColumnIndex(MediaStore.MediaColumns.DATA);
            if (-1 != dataColumnIndex)
            {
                String candidateFilePath = cursor.getString(dataColumnIndex);

                if(!TextUtils.isEmpty(candidateFilePath) && doesFileExistOnDisk(candidateFilePath))
                {
                    filePath = candidateFilePath;
                }
                else
                {
                    Trace.d(LOG_TAG, "FilePathProvider.tryGetPathIfLocalFileProvider DATA field found but it isn't an existing local file path." + candidateFilePath);
                }
            }

            cursor.close();
        }

        return filePath;
    }

    private static String tryGetPathFromContentUriSegments(Uri contentUri)
    {
        String filePath = null;
        ArrayList<String> filePathSegments = new ArrayList<String>(contentUri.getPathSegments());

        Trace.d(LOG_TAG, "FilePathProvider.tryGetPathIfLocalFileProvider, Trying from content url segments : " + filePathSegments.toString());

        // Try to get local file paths from content URIs which looks like "content://com.metago.astro.filecontent/file/storage/emulated/0/Documents/Test1.pptx"
        // The URI segments would be [file, storage, emulated, 0, Documents, Test1.pptx]

        final String fileUriScheme = "file";

        if (filePathSegments.size() > 1 && filePathSegments.get(0) != null && filePathSegments.get(0).equals(fileUriScheme))
        {
            Uri.Builder localUriBuilder = new Uri.Builder().scheme(fileUriScheme);
            for (int i = 1; i < filePathSegments.size(); i++)
            {
                localUriBuilder.appendPath(filePathSegments.get(i));
            }

            String candidateFilePath = localUriBuilder.build().getPath();
            Trace.d(LOG_TAG, "FilePathProvider.tryGetPathIfLocalFileProvider Converted URI : " + candidateFilePath);

            if (doesFileExistOnDisk(candidateFilePath))
            {
                filePath = candidateFilePath;
            }
        }

        return filePath;
    }

    private static boolean doesFileExistOnDisk(String filePath)
    {
        boolean exists = false;
        try
        {
            exists = new File(filePath).exists();
        }
        catch (Exception ex)
        {
            Trace.d(LOG_TAG, "FilePathProvider.doesFileExistOnDisk : Exception which checking for the existance of local file." + ex
                    .getMessage());
        }

        return exists;
    }
}
