#pragma once

#include "src/core/serial_interface.h"
#include "src/core/serial_types.h"

#include <atomic>
#include <string>

namespace serial {

// stub implementation for desktop development/testing
// will be replaced with QSerialPort or platform-specific impl
class desktop_serial_port : public serial_interface {
public:
    desktop_serial_port() = default;
    ~desktop_serial_port() override = default;

    std::vector<device_info> enumerate_devices() override;
    bool open(int device_id, const port_config& config) override;
    bool close() override;
    bool is_open() const override;
    port_state state() const override;

    bool set_config(const port_config& config) override;
    port_config current_config() const override;

    int write(const uint8_t* data, int size) override;
    int read(uint8_t* buffer, int max_size) override;

    void set_read_callback(read_callback cb) override;
    void set_error_callback(error_callback cb) override;

private:
    std::atomic<port_state> state_{port_state::closed};
    port_config config_;
    read_callback read_cb_;
    error_callback error_cb_;
};

} // namespace serial
