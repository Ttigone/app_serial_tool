#pragma once

#include "src/core/serial_interface.h"

#include <memory>

namespace serial {

class platform_serial_factory {
public:
    static std::unique_ptr<serial_interface> create();
};

} // namespace serial
