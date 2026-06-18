#pragma once

#include <QObject>

#include <QVariantMap>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    Q_INVOKABLE bool startConnect(QString name, int baudrate_index);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE bool isConnected();


    Q_INVOKABLE QVariantMap getSerialPorts();
    Q_INVOKABLE QString getSerialPortName();

signals:
    void serialPortErrorMessage(QString message);
    void serialPortInfo(QString message);

    void dataRx(QByteArray data);

public slots:
    void writeData(QByteArray data);

private slots:
    void recvData();
    void serialPortError(QSerialPort::SerialPortError error);

private:
    //串口
    QSerialPort* m_Serial;
    QString m_SerialPortName;
};

