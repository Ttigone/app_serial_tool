#pragma once

#include "serial_interface.h"
#include "serial_types.h"

#include <memory>
#include <mutex>
#include <string>

#include <QObject>
#include <QVariantList>

namespace serial {

class serial_manager : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool is_open READ is_open NOTIFY state_changed)
    Q_PROPERTY(int port_state READ port_state_value NOTIFY state_changed)

public:
    static serial_manager& instance();

    // device enumeration
    Q_INVOKABLE QVariantList enumerate_devices();

    // port lifecycle
    Q_INVOKABLE bool open_port(int device_id, int baud_rate, int data_bits,
                               int stop_bits, int parity, int flow_control);
    Q_INVOKABLE bool close_port();

    // data transfer
    Q_INVOKABLE bool write_data(const QByteArray& data);

    // configuration
    Q_INVOKABLE bool set_baud_rate(int baud_rate);

    bool is_open() const;
    int port_state_value() const;

signals:
    void state_changed();
    void data_received(const QByteArray& data);
    void error_occurred(const QString& message);

private:
    serial_manager();
    ~serial_manager() override;

    serial_manager(const serial_manager&) = delete;
    serial_manager& operator=(const serial_manager&) = delete;

    void on_data_received(const uint8_t* data, int size);
    void on_error_occurred(const std::string& error_msg);

    void update_state();

    std::unique_ptr<serial_interface> port_;
    mutable std::mutex mutex_;
    port_state current_state_ = port_state::closed;
};

} // namespace serial
