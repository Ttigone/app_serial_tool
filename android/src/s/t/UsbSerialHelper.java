package s.t;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class UsbSerialHelper implements SerialInputOutputManager.Listener {

    private static final String TAG = "UsbSerialHelper";
    private static final String ACTION_USB_PERMISSION = "s.t.USB_PERMISSION";

    private static UsbSerialHelper instance;

    private Context context;
    private UsbManager usb_manager;
    private UsbSerialPort serial_port;
    private SerialInputOutputManager io_manager;
    private boolean port_open = false;

    private UsbSerialHelper() {}

    public static UsbSerialHelper getInstance() {
        if (instance == null) {
            instance = new UsbSerialHelper();
        }
        return instance;
    }

    public static int getStatusBarHeight(Context ctx) {
        int result = 0;
        int resourceId = ctx.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = ctx.getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    public static int getNavigationBarHeight(Context ctx) {
        int result = 0;
        int resourceId = ctx.getResources().getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = ctx.getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    public static String[] getSafeAreaInsets(Context ctx) {
        int[] insets = new int[4]; // top, left, right, bottom

        // Status bar height (top inset)
        int resourceId = ctx.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            insets[0] = ctx.getResources().getDimensionPixelSize(resourceId);
        }

        // Navigation bar height (bottom inset)
        resourceId = ctx.getResources().getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            insets[3] = ctx.getResources().getDimensionPixelSize(resourceId);
        }

        String[] result = new String[4];
        for (int i = 0; i < 4; i++) {
            result[i] = String.valueOf(insets[i]);
        }
        return result;
    }

    public static void initContext(Context ctx) {
        getInstance().context = ctx.getApplicationContext();
        getInstance().usb_manager = (UsbManager) ctx.getSystemService(Context.USB_SERVICE);
    }

    public static String[] enumerateDevices(Context ctx) {
        UsbManager mgr = (UsbManager) ctx.getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> drivers = UsbSerialProber.getDefaultProber().findAllDrivers(mgr);

        ArrayList<String> result = new ArrayList<>();
        for (UsbSerialDriver driver : drivers) {
            UsbDevice device = driver.getDevice();
            result.add(device.getDeviceName());
            result.add(String.valueOf(device.getVendorId()));
            result.add(String.valueOf(device.getProductId()));
            result.add(driver.getClass().getSimpleName());
        }
        return result.toArray(new String[0]);
    }

    public static void requestPermission(Context ctx, int device_index) {
        UsbManager mgr = (UsbManager) ctx.getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> drivers = UsbSerialProber.getDefaultProber().findAllDrivers(mgr);

        if (device_index < 0 || device_index >= drivers.size()) return;

        UsbDevice device = drivers.get(device_index).getDevice();
        PendingIntent flags = PendingIntent.getBroadcast(ctx, 0,
                new Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE);
        mgr.requestPermission(device, flags);
    }

    public static boolean openDevice(Context ctx, int device_index,
                                      int baud_rate, int data_bits,
                                      int stop_bits, int parity, int flow_control) {
        UsbSerialHelper helper = getInstance();
        helper.context = ctx.getApplicationContext();
        helper.usb_manager = (UsbManager) ctx.getSystemService(Context.USB_SERVICE);

        try {
            List<UsbSerialDriver> drivers = UsbSerialProber.getDefaultProber()
                    .findAllDrivers(helper.usb_manager);

            if (device_index < 0 || device_index >= drivers.size()) {
                Log.e(TAG, "Invalid device index: " + device_index);
                return false;
            }

            UsbSerialDriver driver = drivers.get(device_index);
            UsbDevice device = driver.getDevice();

            if (!helper.usb_manager.hasPermission(device)) {
                Log.e(TAG, "No USB permission for device");
                return false;
            }

            UsbDeviceConnection connection = helper.usb_manager.openDevice(device);
            if (connection == null) {
                Log.e(TAG, "Failed to open USB device connection");
                return false;
            }

            helper.serial_port = driver.getPorts().get(0);
            helper.serial_port.open(connection);
            helper.serial_port.setParameters(baud_rate, data_bits, stop_bits, parity);

            if (flow_control == 1) {
                helper.serial_port.setRTS(true);
                helper.serial_port.setDTR(true);
            }

            helper.io_manager = new SerialInputOutputManager(helper.serial_port, helper);
            helper.io_manager.start();

            helper.port_open = true;
            Log.i(TAG, "Serial port opened successfully");
            return true;

        } catch (Exception e) {
            Log.e(TAG, "Failed to open device: " + e.getMessage());
            helper.port_open = false;
            return false;
        }
    }

    public static void closeDevice() {
        UsbSerialHelper helper = getInstance();
        try {
            if (helper.io_manager != null) {
                helper.io_manager.stop();
                helper.io_manager = null;
            }
            if (helper.serial_port != null) {
                helper.serial_port.close();
                helper.serial_port = null;
            }
        } catch (Exception e) {
            Log.e(TAG, "Error closing port: " + e.getMessage());
        }
        helper.port_open = false;
    }

    public static boolean setConfig(int baud_rate, int data_bits,
                                     int stop_bits, int parity, int flow_control) {
        UsbSerialHelper helper = getInstance();
        if (!helper.port_open || helper.serial_port == null) return false;

        try {
            helper.serial_port.setParameters(baud_rate, data_bits, stop_bits, parity);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "Failed to set config: " + e.getMessage());
            return false;
        }
    }

    public static int write(byte[] data) {
        UsbSerialHelper helper = getInstance();
        if (!helper.port_open || helper.serial_port == null) return -1;

        try {
            helper.serial_port.write(data, 1000);
            return data.length;
        } catch (Exception e) {
            Log.e(TAG, "Write error: " + e.getMessage());
            return -1;
        }
    }

    public static int read(byte[] buffer, int timeout_ms) {
        UsbSerialHelper helper = getInstance();
        if (!helper.port_open || helper.serial_port == null) return -1;

        try {
            return helper.serial_port.read(buffer, timeout_ms);
        } catch (Exception e) {
            Log.e(TAG, "Read error: " + e.getMessage());
            return -1;
        }
    }

    @Override
    public void onNewData(byte[] data) {
        nativeOnDataReceived(data, data.length);
    }

    @Override
    public void onRunError(Exception e) {
        Log.e(TAG, "IO Manager error: " + e.getMessage());
        nativeOnError(e.getMessage());
    }

    private static native void nativeOnDataReceived(byte[] data, int length);
    private static native void nativeOnError(String message);
}
