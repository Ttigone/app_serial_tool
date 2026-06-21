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
import android.app.Activity;

import androidx.core.content.FileProvider;
import androidx.core.app.ShareCompat;

public class QShareUtils
{
    // reference Authority as defined in AndroidManifest.xml
    private static String AUTHORITY="sinemotion.tool.fileprovider";

    protected QShareUtils()
    {
    }

    // Qt 6: QtNative.activity() is package-private, use reflection to access it
    private static Activity activity() {
        try {
            Class<?> qtNativeClass = Class.forName("org.qtproject.qt.android.QtNative");
            java.lang.reflect.Method activityMethod = qtNativeClass.getDeclaredMethod("activity");
            activityMethod.setAccessible(true);
            return (Activity) activityMethod.invoke(null);
        } catch (Exception e) {
            Log.e("QShareUtils", "Failed to get Qt activity: " + e.getMessage());
            return null;
        }
    }

    public static boolean checkMimeTypeView(String mimeType) {
        if (activity() == null)
            return false;
        Intent myIntent = new Intent();
        myIntent.setAction(Intent.ACTION_VIEW);
        File fileToShare = new File("");
        Uri uri = Uri.fromFile(fileToShare);
        myIntent.setDataAndType(uri, mimeType);

        if (myIntent.resolveActivity(activity().getPackageManager()) != null) {
            Log.d("ekkescorner checkMime ", "YEP - we can go on and View");
            return true;
        } else {
            Log.d("ekkescorner checkMime", "sorry - no App available to View");
        }
        return false;
    }

    public static boolean checkMimeTypeEdit(String mimeType) {
        if (activity() == null)
            return false;
        Intent myIntent = new Intent();
        myIntent.setAction(Intent.ACTION_EDIT);
        File fileToShare = new File("");
        Uri uri = Uri.fromFile(fileToShare);
        myIntent.setDataAndType(uri, mimeType);

        if (myIntent.resolveActivity(activity().getPackageManager()) != null) {
            Log.d("ekkescorner checkMime ", "YEP - we can go on and Edit");
            return true;
        } else {
            Log.d("ekkescorner checkMime", "sorry - no App available to Edit");
        }
        return false;
    }

    public static boolean share(String text, String url) {
        if (activity() == null)
            return false;
        Intent sendIntent = new Intent();
        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_TEXT, text + " " + url);
        sendIntent.setType("text/plain");

        if (sendIntent.resolveActivity(activity().getPackageManager()) != null) {
            activity().startActivity(sendIntent);
            return true;
        } else {
            Log.d("ekkescorner share", "Intent not resolved");
        }
        return false;
    }

    public static boolean createCustomChooserAndStartActivity(Intent theIntent, String title, int requestId, Uri uri) {
        final Context context = activity();
        final PackageManager packageManager = context.getPackageManager();
        final boolean isLowerOrEqualsKitKat = Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT;

        ResolveInfo defaultAppInfo = packageManager.resolveActivity(theIntent, PackageManager.MATCH_DEFAULT_ONLY);
        if(defaultAppInfo == null) {
            Log.d("ekkescorner", title+" PackageManager cannot resolve Activity");
            return false;
        }

        List<ResolveInfo> appInfoList = packageManager.queryIntentActivities(theIntent, PackageManager.MATCH_DEFAULT_ONLY);
        if (appInfoList.isEmpty()) {
            Log.d("ekkescorner", title+" appInfoList.isEmpty");
            return false;
        }
        Log.d("ekkescorner", title+" appInfoList: "+appInfoList.size());

        Collections.sort(appInfoList, new Comparator<ResolveInfo>() {
            @Override
            public int compare(ResolveInfo first, ResolveInfo second) {
                String firstName = first.loadLabel(packageManager).toString();
                String secondName = second.loadLabel(packageManager).toString();
                return firstName.compareToIgnoreCase(secondName);
            }
        });

        List<Intent> targetedIntents = new ArrayList<Intent>();
        for (ResolveInfo appInfo : appInfoList) {
            String targetPackageName = appInfo.activityInfo.packageName;
            if (targetPackageName.equals(context.getPackageName())) {
                continue;
            }

            Intent targetedIntent = new Intent(theIntent);
            targetedIntent.setPackage(targetPackageName);
            targetedIntents.add(targetedIntent);

            if(isLowerOrEqualsKitKat) {
                Log.d("ekkescorner", "legacy support grantUriPermission");
                context.grantUriPermission(targetPackageName, uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            }
        }

        if (targetedIntents.isEmpty()) {
            Log.d("ekkescorner", title+" targetedIntents.isEmpty");
            return false;
        }

        Intent chooserIntent = Intent.createChooser(targetedIntents.remove(targetedIntents.size() - 1), title);
        if (targetedIntents.isEmpty()) {
            Log.d("ekkescorner", title+" only one Intent left for Chooser");
        } else {
            chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS, targetedIntents.toArray(new Parcelable[] {}));
        }
        if (chooserIntent.resolveActivity(activity().getPackageManager()) != null) {
            if(requestId > 0) {
                activity().startActivityForResult(chooserIntent, requestId);
            } else {
                activity().startActivity(chooserIntent);
            }
            return true;
        }
        Log.d("ekkescorner", title+" Chooser Intent not resolved. Should never happen");
        return false;
    }

    public static void revokeFilePermissions(String filePath) {
        final Context context = activity();
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT) {
            File file = new File(filePath);
            Uri uri = FileProvider.getUriForFile(context, AUTHORITY, file);
            context.revokeUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        }
    }

    public static boolean sendFile(String filePath, String title, String mimeType, int requestId) {
        if (activity() == null)
            return false;

        Intent sendIntent = ShareCompat.IntentBuilder.from(activity()).getIntent();
        sendIntent.setAction(Intent.ACTION_SEND);

        File imageFileToShare = new File(filePath);

        Uri uri;
        try {
            uri = FileProvider.getUriForFile(activity(), AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("ekkescorner sendFile - cannot be shared: ", filePath);
            return false;
        }

        Log.d("ekkescorner sendFile", uri.toString());
        sendIntent.putExtra(Intent.EXTRA_STREAM, uri);

        if(mimeType == null || mimeType.isEmpty()) {
            mimeType = activity().getContentResolver().getType(uri);
            Log.d("ekkescorner sendFile guessed mimeType:", mimeType);
        }  else {
            Log.d("ekkescorner sendFile w mimeType:", mimeType);
        }

        sendIntent.setType(mimeType);

        sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        sendIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        return createCustomChooserAndStartActivity(sendIntent, title, requestId, uri);
    }

    public static boolean viewFile(String filePath, String title, String mimeType, int requestId) {
        if (activity() == null)
            return false;

        Intent viewIntent = ShareCompat.IntentBuilder.from(activity()).getIntent();
        viewIntent.setAction(Intent.ACTION_VIEW);

        File imageFileToShare = new File(filePath);

        Uri uri;
        try {
            uri = FileProvider.getUriForFile(activity(), AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("ekkescorner viewFile - cannot be shared: ", filePath);
            return false;
        }

        Log.d("ekkescorner viewFile from file path: ", filePath);
        Log.d("ekkescorner viewFile to content URI: ", uri.toString());

        if(mimeType == null || mimeType.isEmpty()) {
            mimeType = activity().getContentResolver().getType(uri);
            Log.d("ekkescorner viewFile guessed mimeType:", mimeType);
        } else {
            Log.d("ekkescorner viewFile w mimeType:", mimeType);
        }

        viewIntent.setDataAndType(uri, mimeType);

        viewIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        viewIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        return createCustomChooserAndStartActivity(viewIntent, title, requestId, uri);
    }

    public static boolean editFile(String filePath, String title, String mimeType, int requestId) {
        if (activity() == null)
            return false;

        Intent editIntent = ShareCompat.IntentBuilder.from(activity()).getIntent();
        editIntent.setAction(Intent.ACTION_EDIT);

        File imageFileToShare = new File(filePath);

        Uri uri;
        try {
            uri = FileProvider.getUriForFile(activity(), AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("ekkescorner editFile - cannot be shared: ", filePath);
            return false;
        }
        Log.d("ekkescorner editFile", uri.toString());

        if(mimeType == null || mimeType.isEmpty()) {
            mimeType = activity().getContentResolver().getType(uri);
            Log.d("ekkescorner editFile guessed mimeType:", mimeType);
        } else {
            Log.d("ekkescorner editFile w mimeType:", mimeType);
        }

        editIntent.setDataAndType(uri, mimeType);

        editIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        editIntent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        return createCustomChooserAndStartActivity(editIntent, title, requestId, uri);
    }

    public static String getContentName(ContentResolver cR, Uri uri) {
      Cursor cursor = cR.query(uri, null, null, null, null);
      cursor.moveToFirst();
      int nameIndex = cursor
          .getColumnIndex(MediaStore.MediaColumns.DISPLAY_NAME);
      if (nameIndex >= 0) {
        return cursor.getString(nameIndex);
      } else {
        return null;
      }
    }

    public static String createFile(ContentResolver cR, Uri uri, String fileLocation) {
        String filePath = null;
        try {
                InputStream iStream = cR.openInputStream(uri);
                if (iStream != null) {
                    String name = getContentName(cR, uri);
                    if (name != null) {
                        filePath = fileLocation + "/" + name;
                        Log.d("ekkescorner - create File", filePath);
                        File f = new File(filePath);
                        FileOutputStream tmp = new FileOutputStream(f);
                        Log.d("ekkescorner - create File", "new FileOutputStream");

                        byte[] buffer = new byte[1024];
                        while (iStream.read(buffer) > 0) {
                            tmp.write(buffer);
                        }
                        tmp.close();
                        iStream.close();
                        return filePath;
                    }
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                return filePath;
            } catch (IOException e) {
                e.printStackTrace();
                return filePath;
            } catch (Exception e) {
                e.printStackTrace();
                return filePath;
            }
        return filePath;
    }

}
