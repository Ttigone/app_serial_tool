#include "android_serial_port.h"

#include <QCoreApplication>
#include <QJniObject>
#include <QJniEnvironment>

#include <chrono>

namespace serial {

static const char* k_java_class = "s/t/UsbSerialHelper";

android_serial_port::android_serial_port() = default;

android_serial_port::~android_serial_port() {
    close();
}

std::vector<device_info> android_serial_port::enumerate_devices() {
    std::vector<device_info> result;

    QJniObject devices = QJniObject::callStaticObjectMethod(
        k_java_class, "enumerateDevices",
        "(Landroid/content/Context;)[Ljava/lang/String;",
        QJniObject(QCoreApplication::instance()->property("androidContext").value<jobject>()).object()
    );

    if (!devices.isValid()) return result;

    QJniEnvironment env;
    jobjectArray arr = devices.object<jobjectArray>();
    if (!arr) return result;

    jsize count = env->GetArrayLength(arr);
    for (jsize i = 0; i < count; i += 4) {
        device_info info;

        auto get_string = [&](int idx) -> std::string {
            QJniObject str(env->GetObjectArrayElement(arr, idx));
            return str.isValid() ? str.toString().toStdString() : "";
        };

        info.device_name = get_string(i);
        info.vendor_id = get_string(i + 1);
        info.product_id = get_string(i + 2);
        info.driver_name = get_string(i + 3);
        info.device_id = i / 4;
        result.push_back(info);
    }

    return result;
}

bool android_serial_port::open(int device_id, const port_config& config) {
    if (state_ != port_state::closed) return false;
    state_ = port_state::opening;

    QJniObject context = QJniObject::fromLocalRef(
        QCoreApplication::instance()->property("androidContext").value<jobject>()
    );

    jboolean ok = QJniObject::callStaticMethod<jboolean>(
        k_java_class, "openDevice",
        "(Landroid/content/Context;IIIIII)Z",
        context.object(),
        device_id,
        static_cast<jint>(config.baud),
        static_cast<jint>(config.bits),
        static_cast<jint>(config.stop),
        static_cast<jint>(config.par),
        static_cast<jint>(config.flow)
    );

    if (ok) {
        config_ = config;
        state_ = port_state::open;

        reading_ = true;
        read_thread_ = std::thread(&android_serial_port::read_loop, this);
    } else {
        state_ = port_state::error;
        notify_error("Failed to open USB serial device");
    }

    return ok;
}

bool android_serial_port::close() {
    if (state_ == port_state::closed) return true;
    state_ = port_state::closing;

    reading_ = false;
    if (read_thread_.joinable()) {
        read_thread_.join();
    }

    QJniObject::callStaticMethod<void>(k_java_class, "closeDevice");

    state_ = port_state::closed;
    return true;
}

bool android_serial_port::is_open() const {
    return state_ == port_state::open;
}

port_state android_serial_port::state() const {
    return state_.load();
}

bool android_serial_port::set_config(const port_config& config) {
    if (!is_open()) return false;

    jboolean ok = QJniObject::callStaticMethod<jboolean>(
        k_java_class, "setConfig",
        "(IIIII)Z",
        static_cast<jint>(config.baud),
        static_cast<jint>(config.bits),
        static_cast<jint>(config.stop),
        static_cast<jint>(config.par),
        static_cast<jint>(config.flow)
    );

    if (ok) {
        config_ = config;
    }
    return ok;
}

port_config android_serial_port::current_config() const {
    return config_;
}

int android_serial_port::write(const uint8_t* data, int size) {
    if (!is_open()) return -1;

    QJniObject context = QJniObject::fromLocalRef(
        QCoreApplication::instance()->property("androidContext").value<jobject>()
    );

    QJniEnvironment env;
    jbyteArray jarr = env->NewByteArray(size);
    env->SetByteArrayRegion(jarr, 0, size, reinterpret_cast<const jbyte*>(data));

    jint written = QJniObject::callStaticMethod<jint>(
        k_java_class, "write",
        "([B)I",
        jarr
    );

    env->DeleteLocalRef(jarr);
    return written;
}

int android_serial_port::read(uint8_t* buffer, int max_size) {
    if (!is_open()) return -1;

    QJniEnvironment env;
    jbyteArray jarr = env->NewByteArray(max_size);

    jint n = QJniObject::callStaticMethod<jint>(
        k_java_class, "read",
        "([BI)I",
        jarr,
        100  // timeout ms
    );

    if (n > 0) {
        env->GetByteArrayRegion(jarr, 0, n, reinterpret_cast<jbyte*>(buffer));
    }

    env->DeleteLocalRef(jarr);
    return n;
}

void android_serial_port::set_read_callback(read_callback cb) {
    read_cb_ = std::move(cb);
}

void android_serial_port::set_error_callback(error_callback cb) {
    error_cb_ = std::move(cb);
}

void android_serial_port::read_loop() {
    uint8_t buffer[4096];
    while (reading_) {
        int n = read(buffer, sizeof(buffer));
        if (n > 0 && read_cb_) {
            read_cb_(buffer, n);
        } else if (n < 0) {
            notify_error("Read error occurred");
            break;
        }
    }
}

void android_serial_port::notify_error(const std::string& msg) {
    if (error_cb_) {
        error_cb_(msg);
    }
}

} // namespace serial
