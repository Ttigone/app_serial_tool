#include "platform_serial_factory.h"

#ifdef Q_OS_ANDROID
#include "src/platform/android/android_serial_port.h"
#elif defined(Q_OS_MACOS)
// #include "src/platform/macos/macos_serial_port.h"  // TODO: macOS implementation
#include "src/platform/desktop/desktop_serial_port.h"
#else
#include "src/platform/desktop/desktop_serial_port.h"
#endif

namespace serial {

std::unique_ptr<serial_interface> platform_serial_factory::create() {
#ifdef Q_OS_ANDROID
    return std::make_unique<android_serial_port>();
#elif defined(Q_OS_MACOS)
    // return std::make_unique<macos_serial_port>();  // TODO
    return std::make_unique<desktop_serial_port>();
#else
    return std::make_unique<desktop_serial_port>();
#endif
}

} // namespace serial
