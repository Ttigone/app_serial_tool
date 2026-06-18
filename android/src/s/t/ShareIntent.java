// (c) 2017 Ekkehard Gentz (ekke)
// this project is based on ideas from
// http://blog.lasconic.com/share-on-ios-and-android-using-qml/
// see github project https://github.com/lasconic/ShareUtils-QML
// also inspired by:
// https://www.androidcode.ninja/android-share-intent-example/
// https://www.calligra.org/blogs/sharing-with-qt-on-android/
// https://stackoverflow.com/questions/7156932/open-file-in-another-app
// http://www.qtcentre.org/threads/58668-How-to-use-QAndroidJniObject-for-intent-setData
// https://stackoverflow.com/questions/5734678/custom-filtering-of-intent-chooser-based-on-installed-android-package-name
// see also /COPYRIGHT and /LICENSE

package org.sinemo.utils;

import org.qtproject.qt5.android.QtNative;
import org.qtproject.qt5.android.bindings.QtActivity;

import java.lang.String;
import android.content.Intent;
import java.io.File;
import android.net.Uri;
import android.util.Log;

import android.content.ContentResolver;
import android.database.Cursor;
import android.provider.MediaStore;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.FileOutputStream;

import java.util.List;
import android.content.pm.ResolveInfo;
import java.util.ArrayList;
import android.content.pm.PackageManager;
import java.util.Comparator;
import java.util.Collections;
import android.content.Context;
import android.os.Parcelable;

import android.os.Build;

import android.support.v4.content.FileProvider;
import android.support.v4.app.ShareCompat;



public class ShareIntent
{
    public static void shareFile(QtActivity activity, String file_name)
    {

       //String file_name = Environment.getExternalStorageDirectory()  + "/要分享的文件夹/文件.txt";
        //String external_dir = Environment.getExternalStorageDirectory();
        //Log.d("getExternalStorageDirectory: ", external_dir);

       final Uri  appUri ;
       final File new_file = new File(file_name);

       Intent share = new Intent(Intent.ACTION_SEND);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            try {
            appUri = FileProvider.getUriForFile(activity,"sinemotion.tool.fileprovider", new_file);
            } catch (IllegalArgumentException e) {
                Log.d("ekkescorner sendFile - cannot be shared: ", file_name);
                return;
            }

            share.putExtra(Intent.EXTRA_STREAM,appUri);
        }else
        {
            share.putExtra(Intent.EXTRA_STREAM, new_file);
       }
        share.setType("*/*");

        share.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        share.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        activity.startActivity(Intent.createChooser(share, "share"));
    }
}
