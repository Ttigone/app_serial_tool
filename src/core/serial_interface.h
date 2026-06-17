#pragma once

#include "serial_types.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace serial {

class serial_interface {
public:
    virtual ~serial_interface() = default;

    // device enumeration
    virtual std::vector<device_info> enumerate_devices() = 0;

    // port lifecycle
    virtual bool open(int device_id, const port_config& config) = 0;
    virtual bool close() = 0;
    virtual bool is_open() const = 0;
    virtual port_state state() const = 0;

    // configuration
    virtual bool set_config(const port_config& config) = 0;
    virtual port_config current_config() const = 0;

    // data transfer
    virtual int write(const uint8_t* data, int size) = 0;
    virtual int read(uint8_t* buffer, int max_size) = 0;

    // async read callback
    using read_callback = std::function<void(const uint8_t* data, int size)>;
    using error_callback = std::function<void(const std::string& error_msg)>;

    virtual void set_read_callback(read_callback cb) = 0;
    virtual void set_error_callback(error_callback cb) = 0;
};

} // namespace serial
