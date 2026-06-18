#include "SerialPort.h"
#include "Utils/Utility.h"
#include "Utils/Config.h"

#include <QDebug>
#include <QDateTime>

#include "CommManager.h"

SerialPort::SerialPort(QObject *parent)
    : QObject(parent)
{
    m_Serial = new QSerialPort();

    connect(m_Serial, SIGNAL(readyRead()), this, SLOT(recvData()));
    connect(m_Serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(serialPortError(QSerialPort::SerialPortError)));

}

SerialPort::~SerialPort() {

}

bool SerialPort::startConnect(QString port_name, int baudrate_index)
{
    if (!m_Serial->isOpen()) {

        m_Serial->setPortName(port_name);
        m_Serial->open(QIODevice::ReadWrite);

        if (!m_Serial->isOpen()) {
            return false;
        }

        switch (baudrate_index) {
        case 0:
            m_Serial->setBaudRate(115200);
            break;
        case 1:
            m_Serial->setBaudRate(500000);
            break;
        case 2:
            m_Serial->setBaudRate(1000000);
            break;
        case 3:
            m_Serial->setBaudRate(2000000);
            break;
        default:
            m_Serial->setBaudRate(115200);
            break;
        }

        m_Serial->setDataBits(QSerialPort::Data8);
        m_Serial->setParity(QSerialPort::NoParity);
        m_Serial->setStopBits(QSerialPort::OneStop);
        m_Serial->setFlowControl(QSerialPort::NoFlowControl);
    }

    m_SerialPortName = port_name;
    return true;
}

void SerialPort::disconnect()
{
    if (m_Serial->isOpen()) {
        m_Serial->flush();
        m_Serial->close();
    }
}

bool SerialPort::isConnected()
{
    return m_Serial->isOpen();
}

QVariantMap SerialPort::getSerialPorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    QVariantMap ports_map;

    foreach(const QSerialPortInfo & info, ports)
    {
        QString port_name = info.portName();
        QString display_name = info.portName() + " " + info.description();

        ports_map.insert(port_name, display_name);
    }

    return ports_map;
}

QString SerialPort::getSerialPortName()
{
    return m_SerialPortName;
}


void SerialPort::writeData(QByteArray data)
{
    if (m_Serial->isOpen()) {
        m_Serial->write(data);
    }
}

void SerialPort::recvData()
{
    while (m_Serial->bytesAvailable() > 0) {
        emit dataRx(m_Serial->readAll());
    }
}

void SerialPort::serialPortError(QSerialPort::SerialPortError error)
{
    QString message = "";

    switch (error) {
    case QSerialPort::NoError:
        break;
    default:
        message = QString(tr("Serial port error: %1")).arg(m_Serial->errorString());
        break;
    }

    if (!message.isEmpty()) {
        emit serialPortErrorMessage(message);

        if (m_Serial->isOpen()) {
            m_Serial->close();
        }
    }
}
