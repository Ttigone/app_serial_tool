#include "serial_manager.h"
#include "src/platform/platform_serial_factory.h"

#include <QByteArray>
#include <QVariantMap>

namespace serial {

serial_manager& serial_manager::instance() {
    static serial_manager mgr;
    return mgr;
}

serial_manager::serial_manager() {
    port_ = platform_serial_factory::create();
    if (port_) {
        port_->set_read_callback([this](const uint8_t* data, int size) {
            on_data_received(data, size);
        });
        port_->set_error_callback([this](const std::string& error_msg) {
            on_error_occurred(error_msg);
        });
    }
}

serial_manager::~serial_manager() = default;

QVariantList serial_manager::enumerate_devices() {
    QVariantList result;
    if (!port_) return result;

    auto devices = port_->enumerate_devices();
    for (const auto& dev : devices) {
        QVariantMap map;
        map["device_name"] = QString::fromStdString(dev.device_name);
        map["vendor_id"] = QString::fromStdString(dev.vendor_id);
        map["product_id"] = QString::fromStdString(dev.product_id);
        map["driver_name"] = QString::fromStdString(dev.driver_name);
        map["device_id"] = dev.device_id;
        result.append(map);
    }
    return result;
}

bool serial_manager::open_port(int device_id, int baud, int bits,
                               int stop, int par, int flow) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!port_) return false;

    port_config config;
    config.baud = static_cast<baud_rate>(baud);
    config.bits = static_cast<data_bits>(bits);
    config.stop = static_cast<stop_bits>(stop);
    config.par = static_cast<parity>(par);
    config.flow = static_cast<flow_control>(flow);

    bool ok = port_->open(device_id, config);
    update_state();
    return ok;
}

bool serial_manager::close_port() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!port_) return false;

    bool ok = port_->close();
    update_state();
    return ok;
}

bool serial_manager::write_data(const QByteArray& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!port_ || !port_->is_open()) return false;

    int written = port_->write(reinterpret_cast<const uint8_t*>(data.constData()),
                               data.size());
    return written == data.size();
}

bool serial_manager::set_baud_rate(int baud) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!port_ || !port_->is_open()) return false;

    auto config = port_->current_config();
    config.baud = static_cast<baud_rate>(baud);
    return port_->set_config(config);
}

bool serial_manager::is_open() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return port_ && port_->is_open();
}

int serial_manager::port_state_value() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(current_state_);
}

void serial_manager::on_data_received(const uint8_t* data, int size) {
    QByteArray arr(reinterpret_cast<const char*>(data), size);
    emit data_received(arr);
}

void serial_manager::on_error_occurred(const std::string& error_msg) {
    emit error_occurred(QString::fromStdString(error_msg));
}

void serial_manager::update_state() {
    auto new_state = port_ ? port_->state() : port_state::closed;
    if (current_state_ != new_state) {
        current_state_ = new_state;
        emit state_changed();
    }
}

} // namespace serial
