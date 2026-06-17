#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace serial {

enum class baud_rate : uint32_t {
    br_1200 = 1200,
    br_2400 = 2400,
    br_4800 = 4800,
    br_9600 = 9600,
    br_19200 = 19200,
    br_38400 = 38400,
    br_57600 = 57600,
    br_115200 = 115200,
    br_230400 = 230400,
    br_460800 = 460800,
    br_921600 = 921600,
};

enum class data_bits : uint8_t {
    five = 5,
    six = 6,
    seven = 7,
    eight = 8,
};

enum class stop_bits : uint8_t {
    one = 1,
    two = 2,
};

enum class parity : uint8_t {
    none = 0,
    even = 1,
    odd = 2,
};

enum class flow_control : uint8_t {
    none = 0,
    hardware = 1,
    software = 2,
};

enum class port_state {
    closed,
    opening,
    open,
    closing,
    error,
};

struct port_config {
    baud_rate baud = baud_rate::br_9600;
    data_bits bits = data_bits::eight;
    stop_bits stop = stop_bits::one;
    parity par = parity::none;
    flow_control flow = flow_control::none;
};

struct device_info {
    std::string device_name;
    std::string vendor_id;
    std::string product_id;
    std::string driver_name;
    int device_id = -1;
};

} // namespace serial
