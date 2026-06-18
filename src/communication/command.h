#pragma once
#include <QObject>
#include "DataPacket.h"

#include "Data/ParamManager.h"

struct DroneCanNodeStatus
{

public:
    DroneCanNodeStatus() {
        node_id = 0;
        node_type = 0;
        update_timestamp = 0;
        time_out = 3000;
    }

    quint8 node_id;
    quint8 node_type;   //0, normal, 1,droneCAN,
    qint64 update_timestamp;
    qint64 time_out;
};


struct EscLogFileInfo
{

public:
    EscLogFileInfo() {
        entry_index = 0;
        file_size = 0;
        path = "";
    }

    qint16 entry_index;
    qint16 error;
    qint16 entry_type;
    quint32 file_size;
    QString path;
};


class Command : public QObject
{
    Q_OBJECT
public:
    Command();
    ~Command();

    Q_INVOKABLE void parsePacket(CommPacktetData packet_info, QByteArray& data_field);

    //电调信息查询
    Q_INVOKABLE bool queryESCInfo(quint8 device_id);

    Q_INVOKABLE bool queryNodeInfo();
    Q_INVOKABLE bool setPWM(quint8 esc_id, quint16 pwm);
    Q_INVOKABLE bool setPWMCmdMsg(quint16 pwm);
    Q_INVOKABLE bool setPWMCmd2Msg(quint16 pwm);

    Q_INVOKABLE bool setSpeed(quint8 esc_id, qint32 speed);
    Q_INVOKABLE bool setSpeedCommand2(quint8 esc_id, qint32 speed);

    Q_INVOKABLE bool queryRealtimeData(quint8 esc_id);

    Q_INVOKABLE QString setAllParam();
    Q_INVOKABLE QString getAllParam();

    Q_INVOKABLE int setParam(quint16 param_id);
    Q_INVOKABLE int getParam(quint16 param_id);
    Q_INVOKABLE int writeFlash();

    Q_INVOKABLE int unlockConfig(QString passwd);
    Q_INVOKABLE int resetConfig();
    Q_INVOKABLE int clearHistoryState(QString passwd);

    Q_INVOKABLE int startHallCali();
    Q_INVOKABLE int getHallCaliResult();

    Q_INVOKABLE int setPassword(QString new_passwd);


    Q_INVOKABLE bool encoderCmd(quint8 encoder_cmd);
    Q_INVOKABLE bool encoderEnableMechAngle(quint8 enable);
    Q_INVOKABLE bool encoderSetMechAngle(qint16 mech_angle);

    Q_INVOKABLE bool encoderSetMechAngleUpload(quint16 upload_hz);

    Q_INVOKABLE bool queryHistoryErrorCode(quint16 index, quint16 count);

    Q_INVOKABLE bool scanEscActive(quint8 start_id, quint8 end_id);

    Q_INVOKABLE QVariantMap getNodeActiveList();

    Q_INVOKABLE bool readESCLogList();

    Q_INVOKABLE QVariantMap getESCLogList();

    Q_INVOKABLE bool getDirectoryEntryInfo(quint16 entry_index);

    Q_INVOKABLE bool getFileInfo(QString path);

    Q_INVOKABLE bool readFileData(quint32 offset, QString path);

    Q_INVOKABLE void downloadLogFile(QString log_path, quint32 log_size, QString save_file_name);

    Q_INVOKABLE int agingTestAllocateESC(quint8 esc_id);

    Q_INVOKABLE int agingTestStart(quint8 esc_id);

    Q_INVOKABLE int agingTestStop(quint8 esc_id);

    Q_INVOKABLE int agingTestPoint();

signals:
    void sendData(QByteArray & data);
    void sendCmdFinished(quint8 cmd);

    void realtimeDataReceived(EscRealtimeDataParam realtime_data);

    void activeNodeListChange();

    void downloadFileProcess(int progeress);
    void downloadFileMessage(QString message);

public slots:
    void updateNodeStaus();

private:
#ifdef Android_Platform
    bool sendCmd(QByteArray& cmd, quint16 cmd_id, quint16 time_out = 40);
#else
    bool sendCmd(QByteArray& cmd, quint16 cmd_id, quint16 time_out = 20);
#endif
    void parseESCInfo(QByteArray& data);

    void parseRealtimeData(QByteArray& data, quint64 data_signature);

    void parseSetConfig(QByteArray& data);
    void parseGetConfig(QByteArray& data);
    void parseSaveConfig(QByteArray& data);

    void parsePrivateCmd(QByteArray& data);

    void parseEncoderCmd(QByteArray& data);
    void parseEncoderEnableMechAngle(QByteArray& data);
    void parseEncoderSetMechAngle(QByteArray& data);
    void parseEncoderSetMechAngleUpload(QByteArray& data);

    void parseEncodetMechAngle(QByteArray& data);

    void parseHistoryErrorCode(QByteArray& data);

    void parseDynamicNodeID(QByteArray& data);

    void sendDynamicNodeIDResponse(quint8 node_id, QByteArray uniqueid);

    quint8 getDynamicNodeID(quint8 preferred_node_id);

    void parseDroneCanNodeStatus(quint8 esc_id, QByteArray& data);

    void parseFileGetInfo(QByteArray& data);

    void parseFileGetDirectoryEntryInfo(QByteArray& data);

    void parseFileRead(QByteArray& data);

    void parseAgingTestData(QByteArray& data);

    void parseAgingTestCtrl(QByteArray& data);

    void parseAgingTestPoint(QByteArray& data);


    quint8 m_ESCID;

    quint8 m_DynamicPreferredNodeID;    //预分配节点ID
    QByteArray m_DynamicUniqueID;

    QMap<int, DroneCanNodeStatus> m_DroneCanNodeList;

    QTimer* m_UpdateNodeTimer;

    QVector<EscLogFileInfo> m_EscLogFileList;
};

