import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 400
    height: 700
    visible: true
    title: qsTr("Serial Tool")

    color: "#1a1a2e"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // connection panel
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Connection")
            font.pixelSize: 14

            background: Rectangle {
                color: "#16213e"
                radius: 6
                border.color: "#0f3460"
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true

                    ComboBox {
                        id: device_combo
                        Layout.fillWidth: true
                        model: []
                        textRole: "device_name"
                        font.pixelSize: 13
                    }

                    Button {
                        text: qsTr("Refresh")
                        font.pixelSize: 12
                        onClicked: serial_manager.enumerate_devices()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    ComboBox {
                        id: baud_combo
                        Layout.fillWidth: true
                        model: [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
                        currentIndex: 7  // 115200
                        font.pixelSize: 13
                    }

                    ComboBox {
                        id: data_bits_combo
                        model: [5, 6, 7, 8]
                        currentIndex: 3  // 8
                        font.pixelSize: 13
                    }

                    ComboBox {
                        id: stop_bits_combo
                        model: [1, 2]
                        currentIndex: 0
                        font.pixelSize: 13
                    }

                    ComboBox {
                        id: parity_combo
                        model: ["None", "Even", "Odd"]
                        currentIndex: 0
                        font.pixelSize: 13
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        id: open_btn
                        Layout.fillWidth: true
                        text: qsTr("Open")
                        font.pixelSize: 14
                        enabled: !serial_manager.is_open

                        background: Rectangle {
                            color: open_btn.enabled ? "#0f3460" : "#333"
                            radius: 4
                        }

                        onClicked: {
                            var dev = device_combo.model[device_combo.currentIndex]
                            if (dev) {
                                serial_manager.open_port(
                                    dev.device_id,
                                    baud_combo.currentValue,
                                    data_bits_combo.currentValue,
                                    stop_bits_combo.currentValue,
                                    parity_combo.currentIndex,
                                    0  // no flow control
                                )
                            }
                        }
                    }

                    Button {
                        id: close_btn
                        Layout.fillWidth: true
                        text: qsTr("Close")
                        font.pixelSize: 14
                        enabled: serial_manager.is_open

                        background: Rectangle {
                            color: close_btn.enabled ? "#e94560" : "#333"
                            radius: 4
                        }

                        onClicked: serial_manager.close_port()
                    }
                }

                Label {
                    id: status_label
                    text: serial_manager.is_open ? qsTr("Connected") : qsTr("Disconnected")
                    color: serial_manager.is_open ? "#00ff88" : "#ff6b6b"
                    font.pixelSize: 12
                }
            }
        }

        // data display
        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Data")
            font.pixelSize: 14

            background: Rectangle {
                color: "#16213e"
                radius: 6
                border.color: "#0f3460"
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 6

                RowLayout {
                    Layout.fillWidth: true

                    CheckBox {
                        id: hex_display
                        text: qsTr("Hex")
                        font.pixelSize: 12
                    }

                    Button {
                        text: qsTr("Clear")
                        font.pixelSize: 12
                        onClicked: {
                            receive_area.text = ""
                            send_area.text = ""
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    TextArea {
                        id: receive_area
                        readOnly: true
                        font.family: "Consolas"
                        font.pixelSize: 13
                        color: "#00ff88"
                        wrapMode: TextArea.Wrap
                        selectByMouse: true

                        background: Rectangle {
                            color: "#0d1117"
                            radius: 4
                        }

                        Connections {
                            target: serial_manager
                            function onDataReceived(data) {
                                if (hex_display.checked) {
                                    var hex = ""
                                    for (var i = 0; i < data.length; i++) {
                                        hex += ("0" + data[i].charCodeAt(0).toString(16)).slice(-2).toUpperCase() + " "
                                    }
                                    receive_area.append(hex)
                                } else {
                                    receive_area.append(data)
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    CheckBox {
                        id: hex_send
                        text: qsTr("Hex Send")
                        font.pixelSize: 12
                    }

                    CheckBox {
                        id: auto_newline
                        text: qsTr("Auto \\r\\n")
                        font.pixelSize: 12
                        checked: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: send_area
                        Layout.fillWidth: true
                        font.family: "Consolas"
                        font.pixelSize: 13
                        placeholderText: qsTr("Enter data to send...")
                        color: "#e0e0e0"

                        background: Rectangle {
                            color: "#0d1117"
                            radius: 4
                            border.color: "#0f3460"
                        }

                        onAccepted: send_btn.clicked()
                    }

                    Button {
                        id: send_btn
                        text: qsTr("Send")
                        font.pixelSize: 14
                        enabled: serial_manager.is_open

                        background: Rectangle {
                            color: send_btn.enabled ? "#0f3460" : "#333"
                            radius: 4
                        }

                        onClicked: {
                            var text = send_area.text
                            if (text.length === 0) return

                            var data
                            if (hex_send.checked) {
                                // parse hex string like "48 65 6C 6C 6F"
                                var bytes = text.replace(/\s+/g, ' ').trim().split(' ')
                                var arr = new Uint8Array(bytes.length)
                                for (var i = 0; i < bytes.length; i++) {
                                    arr[i] = parseInt(bytes[i], 16)
                                }
                                data = String.fromCharCode.apply(null, arr)
                            } else {
                                data = text
                                if (auto_newline.checked) {
                                    data += "\r\n"
                                }
                            }

                            serial_manager.write_data(data)
                            send_area.text = ""
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: serial_manager
        function onErrorOccurred(message) {
            status_label.text = qsTr("Error: ") + message
            status_label.color = "#ff6b6b"
        }
    }

    Component.onCompleted: {
        // trigger initial device enumeration
        var devices = serial_manager.enumerate_devices()
        device_combo.model = devices
    }
}
