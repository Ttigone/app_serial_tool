#include "desktop_serial_port.h"

namespace serial {

std::vector<device_info> desktop_serial_port::enumerate_devices() {
    // TODO: implement with QSerialPortInfo for desktop
    return {};
}

bool desktop_serial_port::open(int device_id, const port_config& config) {
    // TODO: implement with QSerialPort for desktop
    config_ = config;
    state_ = port_state::open;
    return true;
}

bool desktop_serial_port::close() {
    state_ = port_state::closed;
    return true;
}

bool desktop_serial_port::is_open() const {
    return state_ == port_state::open;
}

port_state desktop_serial_port::state() const {
    return state_.load();
}

bool desktop_serial_port::set_config(const port_config& config) {
    config_ = config;
    return true;
}

port_config desktop_serial_port::current_config() const {
    return config_;
}

int desktop_serial_port::write(const uint8_t* data, int size) {
    // TODO: implement
    return size;
}

int desktop_serial_port::read(uint8_t* buffer, int max_size) {
    // TODO: implement
    return 0;
}

void desktop_serial_port::set_read_callback(read_callback cb) {
    read_cb_ = std::move(cb);
}

void desktop_serial_port::set_error_callback(error_callback cb) {
    error_cb_ = std::move(cb);
}

} // namespace serial
