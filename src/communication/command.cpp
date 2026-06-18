#include "Command.h"
#include <QDebug>
#include "Utils/CustomByteArray.h"
#include "Utils/Utility.h"

#include "CommDefine.h"
#include "Data/RealTimeDataManager.h"
#include "Public/AppInterface.h"
#include "Utils/Config.h"
#include "Data/AgingTestDataManager.h"

Command::Command() {
  m_UpdateNodeTimer = new QTimer(this);
  m_UpdateNodeTimer->setTimerType(Qt::PreciseTimer);

  connect(m_UpdateNodeTimer, SIGNAL(timeout()), this, SLOT(updateNodeStaus()));

  m_UpdateNodeTimer->start(1000);
}

Command::~Command() {}

void Command::parsePacket(CommPacktetData packet_info, QByteArray& data_field) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  // 广播消息
  if (packet_info.transfer_type == 0) {
    switch (packet_info.data_type_id) {
      case COMM_NODE_STATUS: {
        parseDroneCanNodeStatus(packet_info.src_node_id, data_field);
      } break;
      case COMM_MECH_ANGLE_UPLOAD: {
        // 服务的消息的目标ID必须是PC
        if (packet_info.src_node_id != esc_id) return;

        parseEncodetMechAngle(data_field);
      } break;

      case COMM_AGING_TEST_DATA: {
        parseAgingTestData(data_field);
      } break;
    }
  }
  // 匿名消息
  else if (packet_info.transfer_type == 1) {
    // 服务的消息的目标
    if (packet_info.src_node_id == DEVICE_PC_NODE) return;

    switch (packet_info.data_type_id) {
      case COMM_DYNAMIC_NODE_ID: {
        parseDynamicNodeID(data_field);
        break;
      } break;
    }
  }
  // 服务消息
  else if (packet_info.transfer_type == 2) {
    // 服务的消息的目标ID必须是PC
    if (packet_info.dst_node_id != DEVICE_PC_NODE) return;

    switch (packet_info.data_type_id) {
      case COMM_ESC_INFO: {
        parseESCInfo(data_field);
      } break;
      case COMM_SET_PWM2: {
        parseRealtimeData(data_field, SINEMOTION_SETPWM2_SIGNATURE);
      } break;

      case COMM_REALTIME_DATA: {
        parseRealtimeData(data_field, SINEMOTION_REALTIMEDATA_SIGNATURE);
      } break;
      case COMM_SET_SPEED: {
        parseRealtimeData(data_field, SINEMOTION_SETSPEED_SIGNATURE);
      } break;
      case COMM_SET_CONFIG: {
        parseSetConfig(data_field);
      } break;
      case COMM_GET_CONFIG: {
        parseGetConfig(data_field);
      } break;
      case COMM_SAVE_CONFIG: {
        parseSaveConfig(data_field);
      } break;
      case COMM_ENCODER_CMD: {
        parseEncoderCmd(data_field);
      } break;
      case COMM_HISTORY_ERROR_CMD: {
        parseHistoryErrorCode(data_field);
      } break;

      case COMM_SEND_PRIVATE_CMD: {
        parsePrivateCmd(data_field);
      } break;
      case COMM_ENABLE_MECH_ANGLE: {
        parseEncoderEnableMechAngle(data_field);
      } break;
      case COMM_SET_MECH_ANGLE: {
        parseEncoderSetMechAngle(data_field);
      } break;
      case COMM_SET_MECH_ANGLE_UPLOAD: {
        parseEncoderSetMechAngleUpload(data_field);
      } break;

      case COMM_FILE_GETINFO: {
        parseFileGetInfo(data_field);
      } break;

      case COMM_FILE_GET_DIRECTORY_ENTRY_INFO: {
        parseFileGetDirectoryEntryInfo(data_field);
      } break;

      case COMM_FILE_READ: {
        parseFileRead(data_field);
      } break;

      case COMM_AGING_TEST_CTRL: {
        parseAgingTestCtrl(data_field);
      } break;

      case COMM_AGING_TEST_POINT: {
        parseAgingTestPoint(data_field);
      } break;

      default:
        break;
    }

    emit sendCmdFinished(packet_info.data_type_id);
  }
}

bool Command::queryESCInfo(quint8 device_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_ESC_INFO;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool Command::queryNodeInfo() {
  // quint8 esc_comm_id = ParamManager::singleton()->getEscCommID();

  CustomByteArray cmd;

  cmd.appendUint8(0xAA);
  cmd.appendUint8(0x55);
  cmd.appendUint8(2);  // 数据长度

  // cmd.AppendUint8(ESC_QUERY_VERSION);    //功能码,查询版本
  cmd.appendUint8(DEVICE_ESC_NODE);  // 电调地址

  // quint8 check_sum = Utility::calcEscCheckSum(cmd);
  // cmd.AppendUint8(check_sum);

  // QByteArray ble_data = Utility::packBleData(cmd, DEVICE_ESC);
  // bool ret = sendEscCmd(ble_data);
  return false;
}

bool Command::setPWM(quint8 esc_id, quint16 pwm) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_PWM2;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(pwm);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);
  return true;
}

bool Command::setPWMCmdMsg(quint16 pwm) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.data_type_id = COMM_PWM_CMD_MSG;
  packet.transfer_type = MESSAGE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint8(DEVICE_ESC_NODE);
  data_field.appendUint16(pwm);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);
  return true;
}

bool Command::setPWMCmd2Msg(quint16 pwm) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.data_type_id = COMM_PWM_CMD2_MSG;
  packet.transfer_type = MESSAGE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint8(DEVICE_ESC_NODE);
  data_field.appendUint16(pwm);
  data_field.appendUint8(DEVICE_ESC_NODE + 1);
  data_field.appendUint16(pwm);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);
  return true;
}

bool Command::setSpeed(quint8 esc_id, qint32 speed) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_SPEED;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendInt32(speed);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);
  return true;
}

bool Command::setSpeedCommand2(quint8 esc_id, qint32 speed) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SPEED_COMMAND2;
  packet.transfer_type = MESSAGE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  data_field.appendUint8(esc_id);
  data_field.appendInt16(speed & 0xFFFF);
  data_field.appendInt8((speed >> 16) & 0xFF);

  for (int i = 0; i < 19; i++) {
    data_field.appendInt32(0);
  }

  quint16 data_crc = Utility::calcSignatureCrc16(
      SINEMOTION_SPEEDCOMMAND2_SIGNATURE, data_field);
  data_field.push_front((data_crc & 0xff00) >> 8);
  data_field.push_front(data_crc & 0x00ff);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);
  return true;
}

bool Command::queryRealtimeData(quint8 esc_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_REALTIME_DATA;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  emit sendData(cmd);

  // bool ret = sendCmd(cmd, packet.data_type_id);
  return true;
}

QString Command::setAllParam() {
  QString result_info;
  QStringList result_list;

  QList<int> param_id_list = ParamManager::singleton()->getParamList();

  for (int i = 0; i < param_id_list.size(); i++) {
    int param_id = param_id_list[i];
    Parameter* param = ParamManager::singleton()->getParamPtr(param_id);

    if (param->m_Attr == ParamAttrType::ATTR_RO) {
      continue;
    }

    int result = setParam(param_id_list[i]);

    if (result != 0) {
      if (result_list.count() > 8) {
        if (result_list.count() == 8) {
          result_list.append(QString("......"));
        }
      } else {
        switch (result) {
          case -1:
            result_list.append(QString(
                tr("%1 Write Comm Error").arg(param->getParamDisplayName())));
            break;
          case 1:
            result_list.append(QString(
                tr("%1 ESC Not Support").arg(param->getParamDisplayName())));
            break;
          case 2:
            result_list.append(QString(
                tr("%1 Data Type Error").arg(param->getParamDisplayName())));
            break;
          case 3:
            result_list.append(QString(
                tr("%1 Data Out of Range").arg(param->getParamDisplayName())));
            break;
          case 4:
            result_list.append(QString(tr("Esc Config Locked")));
            break;
          case 5:
            result_list.append(QString(tr("%1 Need To Stop Motor First")
                                           .arg(param->getParamDisplayName())));
            break;
          case 6:
            result_list.append(
                QString(tr("%1 Readonly").arg(param->getParamDisplayName())));
            break;
          case 7:
            // result_list.append(QString(tr("%1 Need To Restart to Take
            // Effect").arg(param->getParamDisplayName())));
            break;
          default:
            result_list.append(QString(
                tr("%1 Data Type Error").arg(param->getParamDisplayName())));
            break;
        }
      }

      if (result == -1 || result == 4 || result == 5) {
        break;
      }
    }
  }
  result_info = result_list.join("\n");

  return result_info;
}

QString Command::getAllParam() {
  QString result_info;
  QStringList result_list;
  QList<int> param_id_list = ParamManager::singleton()->getParamList();

  for (int i = 0; i < param_id_list.size(); i++) {
    int param_id = param_id_list[i];
    Parameter* param = ParamManager::singleton()->getParamPtr(param_id);

    int result = getParam(param_id_list[i]);

    if (result != 0) {
      if (result_list.count() > 5) {
        if (result_list.count() == 5) {
          result_list.append(QString("......"));
        }
      } else {
        switch (result) {
          case -1:
            result_list.append(QString(
                tr("%1 Read Comm Error").arg(param->getParamDisplayName())));
            break;
          case 1:
            result_list.append(QString(
                tr("%1 ESC Not Support").arg(param->getParamDisplayName())));
            break;
          case 2:
            result_list.append(QString(
                tr("%1 Data Type Error").arg(param->getParamDisplayName())));
            break;
          case 4:
            result_list.append(QString(tr("Esc Config Locked")));
            break;
          default:
            result_list.append(QString(
                tr("%1 Data Type Error").arg(param->getParamDisplayName())));
            break;
        }
      }

      if (result == -1 || result == 4) {
        break;
      }
    }
  }

  result_info = result_list.join("\n");
  return result_info;
}

int Command::setParam(quint16 param_id) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_CONFIG;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(param_id);
  ParamManager::singleton()->getParamByte(param_id, data_field);

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc =
        Utility::calcSignatureCrc16(SINEMOTION_SETCONFIG_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->set_config_id = param_id;
  config_info->set_config_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->set_config_result;
}

int Command::getParam(quint16 param_id) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_GET_CONFIG;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(param_id);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->get_config_id = param_id;
  config_info->get_config_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->get_config_result;
}

int Command::writeFlash() {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SAVE_CONFIG;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->save_config_result = -1;
  sendCmd(cmd, packet.data_type_id);

  return config_info->save_config_result;
}

int Command::unlockConfig(QString passwd) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SEND_PRIVATE_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(PRIVATE_CMD_UNLOCK_CONFIG);
  ;

  QByteArray passwd_array = passwd.toLatin1();

  for (int i = 0; i < 10; i++) {
    if (i < passwd_array.size()) {
      data_field.appendUint8(passwd_array[i]);
    } else {
      data_field.appendUint8(0);
    }
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc = Utility::calcSignatureCrc16(
        SINEMOTION_SENDPRIVATECMD_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->private_cmd_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->private_cmd_result;
}

int Command::resetConfig() {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SEND_PRIVATE_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(PRIVATE_CMD_RESET_CONFIG);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->private_cmd_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->private_cmd_result;
}

int Command::clearHistoryState(QString passwd) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SEND_PRIVATE_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(PRIVATE_CMD_CLEAR_HISTORY_STATE);
  ;

  QByteArray passwd_array = passwd.toLatin1();

  for (int i = 0; i < 10; i++) {
    if (i < passwd_array.size()) {
      data_field.appendUint8(passwd_array[i]);
    } else {
      data_field.appendUint8(0);
    }
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc = Utility::calcSignatureCrc16(
        SINEMOTION_SENDPRIVATECMD_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->private_cmd_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->private_cmd_result;
}

int Command::startHallCali() {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  // 校准前清数值
  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  encoder_info->cali_state = 0;
  encoder_info->open_loop_angle = 0;
  encoder_info->elec_angle = 0;
  encoder_info->hall_cw_offset = 0;
  encoder_info->hall_ccw_offset = 0;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SEND_PRIVATE_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(PRIVATE_CMD_SENSOR_CALIB);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->private_cmd_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->private_cmd_result;
}

int Command::getHallCaliResult() {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SEND_PRIVATE_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(PRIVATE_CMD_SENSOR_CALIB_STATUS);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->private_cmd_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->private_cmd_result;
}

int Command::setPassword(QString new_passwd) {
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_CONFIG;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(0xff0);

  QByteArray passwd_array = new_passwd.toLatin1();

  for (int i = 0; i < 10; i++) {
    if (i < passwd_array.size()) {
      data_field.appendUint8(passwd_array[i]);
    } else {
      data_field.appendUint8(0);
    }
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc =
        Utility::calcSignatureCrc16(SINEMOTION_SETCONFIG_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  config_info->set_config_id = 0xff0;
  config_info->set_config_result = -1;

  sendCmd(cmd, packet.data_type_id);

  return config_info->set_config_result;
}

bool Command::encoderCmd(quint8 encoder_cmd) {
  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  quint8 esc_id = ParamManager::singleton()->getEscID();

  encoder_info->cmd_result = -1;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_ENCODER_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint8(encoder_cmd);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id, 3);

  return ret;
}

bool Command::encoderEnableMechAngle(quint8 enable) {
  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  quint8 esc_id = ParamManager::singleton()->getEscID();

  encoder_info->enable_mech_angle_flag = -1;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_ENABLE_MECH_ANGLE;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint8(enable);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool Command::encoderSetMechAngle(qint16 mech_angle) {
  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  quint8 esc_id = ParamManager::singleton()->getEscID();

  encoder_info->set_mech_angle_flag = -1;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_MECH_ANGLE;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(mech_angle);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  // bool ret = sendCmd(cmd, packet.data_type_id);

  emit sendData(cmd);
  return true;
}

bool Command::encoderSetMechAngleUpload(quint16 upload_hz) {
  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  quint8 esc_id = ParamManager::singleton()->getEscID();

  encoder_info->set_mech_angle_upload_flag = -1;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_SET_MECH_ANGLE_UPLOAD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(upload_hz);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool Command::queryHistoryErrorCode(quint16 index, quint16 count) {
  bool ret = false;
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_HISTORY_ERROR_CMD;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  EscInfoParam* esc_info = ParamManager::singleton()->getEscInfoParamPtr();
  esc_info->history_error_code = "";

  for (int i = 0; i < count; i++) {
    CustomByteArray data_field;
    data_field.clear();
    data_field.appendUint16(index + i);

    packet.data_len = data_field.length();

    QByteArray cmd = Utility::encodePacket(packet, data_field);
    ret = sendCmd(cmd, packet.data_type_id);
    if (ret == false) {
      break;
    }
  }

  return ret;
}

bool Command::scanEscActive(quint8 start_id, quint8 end_id) {
  // 删除normal节点
  QMap<int, DroneCanNodeStatus>::iterator iter;
  for (iter = m_DroneCanNodeList.begin(); iter != m_DroneCanNodeList.end();) {
    DroneCanNodeStatus node_status = iter.value();
    if (node_status.node_type == 0) {
      iter = m_DroneCanNodeList.erase(iter);
      continue;
    }
    iter++;
  }

  for (int i = start_id; i <= end_id; i++) {
    if (m_DroneCanNodeList.contains(i)) {
      continue;
    }

    if (queryESCInfo(i)) {
      DroneCanNodeStatus active_node;
      active_node.node_id = i;
      active_node.node_type = 0;  // normal

      m_DroneCanNodeList.insert(i, active_node);
    }
  }

  emit activeNodeListChange();

  return true;
}

QVariantMap Command::getNodeActiveList() {
  QVariantMap esc_map;

  QMap<int, DroneCanNodeStatus>::iterator iter;
  for (iter = m_DroneCanNodeList.begin(); iter != m_DroneCanNodeList.end();
       iter++) {
    quint8 node_id = iter.key();
    QString device_id = QString::number(node_id);
    QString display_name = QString("Node ID(%1)").arg(node_id, 2);
    esc_map.insert(device_id, display_name);
  }

  return esc_map;
}

bool Command::readESCLogList() {
  m_EscLogFileList.clear();

  bool ret = true;
  for (int i = 0; i < 128; i++) {
    if (getDirectoryEntryInfo(i)) {
      LogFileInfoParam* log_file_info =
          ParamManager::singleton()->getLogFileInfoParamPtr();
      if (log_file_info->file_error == 0) {
        if (getFileInfo(log_file_info->file_path)) {
          if (log_file_info->file_error == 0) {
            EscLogFileInfo log_info;
            log_info.entry_index = i;
            log_info.error = log_file_info->file_error;
            log_info.entry_type = log_file_info->entry_type;
            log_info.file_size = log_file_info->file_size;
            log_info.path = log_file_info->file_path;

            m_EscLogFileList.append(log_info);
          } else {
            break;
          }
        } else {
          break;
        }
      } else {
        break;
      }
    } else {
      break;
    }
  }

  return ret;
}

QVariantMap Command::getESCLogList() {
  QVariantMap log_map;

  QMap<int, EscLogFileInfo>::iterator iter;
  for (int i = 0; i < m_EscLogFileList.size(); i++) {
    EscLogFileInfo log_item = m_EscLogFileList[i];

    QVariantMap item_map;
    item_map.insert("entry_index", QString::number(log_item.entry_index));
    item_map.insert("error", QString::number(log_item.error));
    item_map.insert("entry_type", QString::number(log_item.entry_type));
    item_map.insert("file_size", QString::number(log_item.file_size));
    item_map.insert("file_path", log_item.path);

    log_map.insert(QString::number(log_item.entry_index), item_map);
  }

  return log_map;
}

bool Command::getDirectoryEntryInfo(quint16 entry_index) {
  bool ret = false;
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_FILE_GET_DIRECTORY_ENTRY_INFO;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();
  log_file_info->file_error = -1;
  log_file_info->entry_type = 0;
  log_file_info->file_path = "";

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint32(entry_index);

  // path
  data_field.appendUint8('.');
  data_field.appendUint8('/');

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ret = sendCmd(cmd, packet.data_type_id);

  return ret;
}

bool Command::getFileInfo(QString path) {
  bool ret = false;
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_FILE_GETINFO;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();
  log_file_info->file_error = -1;
  log_file_info->entry_type = 0;

  CustomByteArray data_field;
  data_field.clear();

  QByteArray path_array = path.toLatin1();

  for (int i = 0; i < path_array.size(); i++) {
    data_field.appendUint8(path_array[i]);
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_FILE_GETINFO_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ret = sendCmd(cmd, packet.data_type_id);

  return ret;
}

bool Command::readFileData(quint32 offset, QString path) {
  bool ret = false;
  quint8 esc_id = ParamManager::singleton()->getEscID();

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = esc_id;
  packet.data_type_id = COMM_FILE_READ;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();
  log_file_info->file_error = -1;
  log_file_info->entry_type = 0;
  log_file_info->file_path = "";

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint32(offset);
  data_field.appendUint8(0);

  QByteArray path_array = path.toLatin1();

  for (int i = 0; i < path_array.size(); i++) {
    data_field.appendUint8(path_array[i]);
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_FILE_READ_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  ret = sendCmd(cmd, packet.data_type_id);

  return ret;
}

void Command::downloadLogFile(QString log_path, quint32 log_size,
                              QString save_file_name) {
  QDir folder;

  QString dir = Config::singleton()->getRootPath();

  QDateTime current_date_time = QDateTime::currentDateTime();

  QString current_date = current_date_time.toString("yyyy-MM-dd");

  dir += "/EscLog/" + current_date + "/";

  // 创建文件夹
  if (!folder.exists(dir)) {
    folder.mkpath(dir);
  }

  QString log_file_path = dir + save_file_name;
  QFile log_file(log_file_path);
  if (!log_file.open(QIODevice::ReadWrite)) {
    return;
  }

  emit downloadFileMessage(tr("Create File ") + log_file_path);

  quint32 download_offset = 0;

  int progress = 0;

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();

  emit downloadFileMessage(tr("Start Downloading Log File..."));

  while (download_offset < log_size) {
    qDebug() << "fuck";
    // 循环读取
    QElapsedTimer timer;
    timer.start();
    qInfo() << "Downloading Log File Offset:" << download_offset;
    if (readFileData(download_offset, log_path)) {
      qInfo() << "Read File Data Time:" << timer.elapsed() << "ms";
      timer.restart();
      qInfo() << "Read File Data Size:" << log_file_info->read_data_size;
      log_file.write((const char*)log_file_info->file_data,
                     log_file_info->read_data_size);
      download_offset += log_file_info->read_data_size;

      progress = (int)(download_offset * 100 / log_size);

      emit downloadFileProcess(progress);
    } else {
      emit downloadFileMessage(
          tr("<font color=#e81123>Open firmware file error...</font>"));
      break;
    }
  }
  if (progress == 100) {
    emit downloadFileMessage(
        tr("<font color=#0066B4>Download Log Success...</font>"));
  }

  log_file.close();
}

bool Command::sendCmd(QByteArray& cmd, quint16 cmd_id, quint16 timeout) {
  bool ret = false;
  quint16 used_cmd = cmd_id;
  int retry_cnt = 0;

  auto conn = connect(this, &Command::sendCmdFinished,
                      [&ret, &used_cmd](quint8 recv_cmd) {
                        if (recv_cmd == used_cmd) {
                          ret = true;
                        }
                      });

  while (retry_cnt++ < 5) {
    emit sendData(cmd);

    for (int i = 0; i < timeout; i++) {
      Utility::DelayMs(5);
      if (ret) {
        break;
      }
    }

    if (ret) {
      break;
    }
  }

  disconnect(conn);
  return ret;
}

void Command::parseESCInfo(QByteArray& data) {
  CustomByteArray data_array = data;
  quint16 data_crc = data_array.popFrontUint16();

  /*  串口不支持data_crc校验
  quint16 calc_data_crc =
  Utility::calcSignatureCrc16(SINEMOTION_ESCINFO_SIGNATURE, data_array);

  if(data_crc != calc_data_crc)
  {
      qWarning() << "Data Error:Data Crc" << data_crc << "Calc Crc" <<
  calc_data_crc; qDebug()<< Utility::ByteArray2HexStr(data); return;
  }
  */

  EscInfoParam* esc_info = ParamManager::singleton()->getEscInfoParamPtr();

  quint16 hw_id = data_array.popFrontUint16();
  quint16 bootloader_version = data_array.popFrontUint16();
  quint16 app_version = data_array.popFrontUint16();
  quint16 motor_id = data_array.popFrontUint16();
  quint16 param_version = data_array.popFrontUint16();

  esc_info->hardware_id = QString("0x%1").arg(QString::number(hw_id, 16));
  esc_info->bootloader_version =
      QString("V%1.%2(0x%3)")
          .arg((bootloader_version >> 8) & 0xff)
          .arg(bootloader_version & 0xff, 2, 10, QLatin1Char('0'))
          .arg(QString::number(bootloader_version, 16));
  esc_info->app_version = QString("V%1.%2(0x%3)")
                              .arg((app_version >> 8) & 0xff)
                              .arg(app_version & 0xff, 2, 10, QLatin1Char('0'))
                              .arg(QString::number(app_version, 16));
  esc_info->motor_id = QString("%1").arg(motor_id);  // resv1

  esc_info->param_version =
      QString("V%1.%2(0x%3)")
          .arg((param_version >> 8) & 0xff)
          .arg(param_version & 0xff, 2, 10, QLatin1Char('0'))
          .arg(QString::number(param_version, 16));  // resv2

  esc_info->sn_code = data_array;
}

void Command::parseRealtimeData(QByteArray& data, quint64 data_signature) {
  CustomByteArray data_array = data;

  if (data.length() != 35) {
    qWarning() << "data.length() != 35";
    qDebug() << Utility::ByteArray2HexStr(data);
    return;
  }

  quint16 data_crc = data_array.popFrontUint16();
  quint16 calc_data_crc =
      Utility::calcSignatureCrc16(data_signature, data_array);

  if (data_crc != calc_data_crc) {
    qWarning() << "Data Error:Data Crc" << data_crc << "Calc Crc"
               << calc_data_crc;
    qDebug() << Utility::ByteArray2HexStr(data);
    return;
  }

  EscRealtimeDataParam* realtime_data =
      ParamManager::singleton()->getEscRealtimeDataParam();

  realtime_data->recv_pwm = data_array.popFrontUint16();
  realtime_data->comm_pwm = data_array.popFrontUint16();

  realtime_data->speed = data_array.popFrontInt32();

  realtime_data->current = data_array.popFrontInt16();
  realtime_data->bus_current = data_array.popFrontInt16();

  realtime_data->voltage = data_array.popFrontUint16();
  realtime_data->v_modulation = data_array.popFrontUint16();

  realtime_data->mos_temp = data_array.popFrontInt16();
  realtime_data->cap_temp = data_array.popFrontInt16();
  realtime_data->mcu_temp = data_array.popFrontInt16();

  realtime_data->running_error = data_array.popFrontUint16();
  realtime_data->selfcheck_error = data_array.popFrontUint16();

  if (data.length() == 35) {
    realtime_data->motor_temp = data_array.popFrontInt16();
    realtime_data->time_10ms = data_array.popFrontUint16();
  }

  realtime_data->speed_km_h = AppInterface::singleton()->getSpeedKmH() * 10;

  RealTimeDataManager::singleton()->addDataPoint();

  emit realtimeDataReceived(*realtime_data);
}

void Command::parseSetConfig(QByteArray& data) {
  CustomByteArray data_array = data;

  quint16 param_id = data_array.popFrontUint16();
  quint8 valid_flag = data_array.popFrontUint8();

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();

  if (config_info->set_config_id == param_id) {
    config_info->set_config_result = valid_flag;
  } else {
    config_info->set_config_result = 1;
  }
}

void Command::parseGetConfig(QByteArray& data) {
  CustomByteArray data_array = data;
  if (data.length() > 7) {
    // 去除最开始2位的crc
    data_array = data.mid(2);
  }
  quint16 param_id = data_array.popFrontUint16();
  quint8 valid_flag = data_array.popFrontUint8();

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();
  if (config_info->get_config_id == param_id) {
    if (valid_flag == 0)
      ParamManager::singleton()->setParamByte(param_id, data_array);

    config_info->get_config_result = valid_flag;
  } else {
    config_info->get_config_result = 1;  // 不支持
  }
}

void Command::parseSaveConfig(QByteArray& data) {
  CustomByteArray data_array = data;
  quint8 state = data_array.popFrontUint8();

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();

  config_info->save_config_result = state;
}

void Command::parsePrivateCmd(QByteArray& data) {
  CustomByteArray data_array = data;

  if (data.length() > 7) {
    // 去除最开始2位的crc
    data_array = data.mid(2);
  }

  ConfigInfoParam* config_info =
      ParamManager::singleton()->getConfigInfoParam();

  uint16_t recv_cmd_id = data_array.popFrontUint16();
  quint8 valid_flag = data_array.popFrontUint8();

  switch (recv_cmd_id) {
    case PRIVATE_CMD_UNLOCK_CONFIG:
      break;
    case PRIVATE_CMD_RESET_CONFIG:
      break;
    case PRIVATE_CMD_SENSOR_CALIB:
      break;
    case PRIVATE_CMD_SENSOR_CALIB_STATUS:
      EncoderInfoParam* encoder_info =
          ParamManager::singleton()->getEncoderInfoParamPtr();
      encoder_info->cali_state = data_array.popFrontUint8();
      encoder_info->open_loop_angle = data_array.popFrontInt32();
      encoder_info->elec_angle = data_array.popFrontInt32();
      encoder_info->hall_cw_offset = data_array.popFrontInt32();
      encoder_info->hall_ccw_offset = data_array.popFrontInt32();

      RealTimeDataManager::singleton()->addEncoderPoint();
      break;
  }

  config_info->private_cmd_result = valid_flag;
}

void Command::parseEncoderCmd(QByteArray& data) {
  CustomByteArray data_array = data;

  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();

  quint8 encoder_cmd = data_array.popFrontUint8();
  switch (encoder_cmd) {
    case ENCODER_CMD_CALI:
      encoder_info->cmd_result = data_array.popFrontUint8();
      break;
    case ENCODER_CMD_QUERY_CALI_RESULT:
      encoder_info->cmd_result = data_array.popFrontUint8();
      break;
    case ENCODER_CMD_SET_TARGET_POS:
      encoder_info->cmd_result = data_array.popFrontUint8();
      break;
  }
}

void Command::parseEncoderEnableMechAngle(QByteArray& data) {
  CustomByteArray data_array = data;

  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();

  encoder_info->enable_mech_angle_flag = data_array.popFrontUint8();
}

void Command::parseEncoderSetMechAngle(QByteArray& data) {
  CustomByteArray data_array = data;

  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  encoder_info->set_mech_angle_flag = data_array.popFrontUint8();
}

void Command::parseEncoderSetMechAngleUpload(QByteArray& data) {
  CustomByteArray data_array = data;

  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  encoder_info->set_mech_angle_upload_flag = data_array.popFrontUint8();
}

void Command::parseEncodetMechAngle(QByteArray& data) {
  CustomByteArray data_array = data;

  EncoderInfoParam* encoder_info =
      ParamManager::singleton()->getEncoderInfoParamPtr();
  encoder_info->mech_angle = data_array.popFrontInt16();

  RealTimeDataManager::singleton()->addEncoderPoint();
}

void Command::parseHistoryErrorCode(QByteArray& data) {
  CustomByteArray data_array = data;

  EscInfoParam* esc_info = ParamManager::singleton()->getEscInfoParamPtr();

  data_array.popFrontUint8();  // valid_flag

  qint32 error_code = data_array.popFrontUint16();
  if (error_code == 65535) {
    error_code = -1;
  }

  esc_info->history_error_code +=
      QString("%1 ").arg(error_code, 5, 10, QLatin1Char(' ')).toUpper();
}

void Command::parseDynamicNodeID(QByteArray& data) {
  CustomByteArray data_array = data;
  quint8 first_byte = data_array.popFrontUint8();

  bool first_part_of_unique_id = first_byte & 0x01;
  quint8 node_id = 0;
  if (first_part_of_unique_id) {
    m_DynamicPreferredNodeID = first_byte >> 1;
    if (m_DynamicPreferredNodeID == 0) {
      m_DynamicPreferredNodeID = 10;
    }

    m_DynamicUniqueID.clear();

    qDebug() << "m_DynamicUniqueID Clear";
  }

  m_DynamicUniqueID.append(data_array);

  qDebug() << "m_DynamicUniqueID :" << QString::fromUtf8(m_DynamicUniqueID);

  if (m_DynamicUniqueID.size() == 16) {
    quint8 dynamic_node_id = getDynamicNodeID(m_DynamicPreferredNodeID);
    sendDynamicNodeIDResponse(dynamic_node_id, m_DynamicUniqueID);
  } else {
    sendDynamicNodeIDResponse(node_id, m_DynamicUniqueID);
  }
}

void Command::sendDynamicNodeIDResponse(quint8 node_id, QByteArray unique_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.data_type_id = COMM_DYNAMIC_NODE_ID;
  packet.transfer_type = ANONYMOUS_MESSAGE_TRANFER;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  quint8 first_byte = (node_id << 1) & 0xFE;

  data_field.appendUint8(first_byte);

  for (int i = 0; i < unique_id.size(); i++) {
    data_field.appendUint8(unique_id[i]);
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc =
        Utility::calcSignatureCrc16(SINEMOTION_DYNAMIC_NODE_ID, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  sendData(cmd);
  return;
}

quint8 Command::getDynamicNodeID(quint8 preferred_node_id) {
  quint8 node_id = preferred_node_id;
  if (m_DroneCanNodeList.contains(preferred_node_id)) {
    for (int i = 32; i < 127; i++) {
      if (m_DroneCanNodeList.contains(i) == false) {
        node_id = i;
        break;
      }
    }
  }

  return node_id;
}

void Command::parseDroneCanNodeStatus(quint8 esc_id, QByteArray& data) {
  // 不存在该节点
  if (m_DroneCanNodeList.contains(esc_id) == false) {
    DroneCanNodeStatus active_node;
    active_node.node_id = esc_id;
    active_node.node_type = 1;  // droneCAN
    active_node.update_timestamp = QDateTime::currentMSecsSinceEpoch();

    m_DroneCanNodeList.insert(esc_id, active_node);

    emit activeNodeListChange();
    return;
  }

  m_DroneCanNodeList[esc_id].update_timestamp =
      QDateTime::currentMSecsSinceEpoch();
}

void Command::updateNodeStaus() {
  qint64 time_now = QDateTime::currentMSecsSinceEpoch();

  QMap<int, DroneCanNodeStatus>::iterator iter;
  for (iter = m_DroneCanNodeList.begin(); iter != m_DroneCanNodeList.end();) {
    DroneCanNodeStatus node_status = iter.value();
    if (node_status.node_type == 1) {
      if (time_now > (node_status.update_timestamp + node_status.time_out)) {
        iter = m_DroneCanNodeList.erase(iter);

        emit activeNodeListChange();

        continue;
      }
    }
    iter++;
  }
}

void Command::parseFileGetInfo(QByteArray& data) {
  CustomByteArray data_array = data;

  if (data.length() > 7) {
    quint16 data_crc = data_array.popFrontUint16();
    quint16 calc_data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_FILE_GETINFO_SIGNATURE, data_array);
    if (data_crc != calc_data_crc) {
      qWarning() << "parseFileGetDirectoryEntryInfo:Data Crc" << data_crc
                 << "Calc Crc" << calc_data_crc;
      qDebug() << Utility::ByteArray2HexStr(data);
      return;
    }
  }

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();

  // FileSize
  log_file_info->file_size = data_array.popFrontUint32();
  data_array.popFrontUint8();  // uint40

  // Error
  log_file_info->file_error = data_array.popFrontInt16();
  // Entry Type
  log_file_info->entry_type = data_array.popFrontUint8();
}

void Command::parseFileGetDirectoryEntryInfo(QByteArray& data) {
  CustomByteArray data_array = data;

  if (data.length() > 7) {
    quint16 data_crc = data_array.popFrontUint16();
    quint16 calc_data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_FILE_GETDIRECTORYENTRYINFO_SIGNATURE, data_array);
    if (data_crc != calc_data_crc) {
      qWarning() << "parseFileGetDirectoryEntryInfo:Data Crc" << data_crc
                 << "Calc Crc" << calc_data_crc;
      qDebug() << Utility::ByteArray2HexStr(data);
      return;
    }
  }

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();

  // Error
  log_file_info->file_error = data_array.popFrontInt16();
  // Entry Type
  log_file_info->entry_type = data_array.popFrontUint8();
  // Path
  log_file_info->file_path = data_array;
}

void Command::parseFileRead(QByteArray& data) {
  CustomByteArray data_array = data;

  if (data.length() > 7) {
    quint16 data_crc = data_array.popFrontUint16();
    quint16 calc_data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_FILE_READ_SIGNATURE, data_array);
    if (data_crc != calc_data_crc) {
      qWarning() << "parseFileRead:Data Crc" << data_crc << "Calc Crc"
                 << calc_data_crc;
      qDebug() << Utility::ByteArray2HexStr(data);
      return;
    }
  }

  LogFileInfoParam* log_file_info =
      ParamManager::singleton()->getLogFileInfoParamPtr();
  // Error
  log_file_info->file_error = data_array.popFrontInt16();

  // file data
  log_file_info->read_data_size = data_array.size();
  memcpy(log_file_info->file_data, data_array.data(), data_array.size());
}

void Command::parseAgingTestData(QByteArray& data) {
  CustomByteArray data_array = data;

  if (data.length() != 58) {
    qDebug() << Utility::ByteArray2HexStr(data);

    qWarning() << "parseAgingTestData length error";
    return;
  }

  quint16 data_crc = data_array.popFrontUint16();
  quint16 calc_data_crc = Utility::calcSignatureCrc16(
      UAVCAN_PROTOCOL_AGING_TEST_DATA_SIGNATURE, data_array);
  if (data_crc != calc_data_crc) {
    qWarning() << "parseAgingTestData:Data Crc" << data_crc << "Calc Crc"
               << calc_data_crc;
    qDebug() << Utility::ByteArray2HexStr(data);
    return;
  }

  quint8 node_id = data_array.popFrontUint8();           // 节点ID
  quint16 test_count = data_array.popFrontUint16();      // 当前测试点
  quint8 test_point_index = data_array.popFrontUint8();  // 当前测试点索引
  quint32 test_point_time_ms =
      data_array.popFrontUint32();  // 当前测试点执行时间ms

  EscAgingTestDataParam* aging_test_data = NULL;
  if (node_id == 32) {
    aging_test_data = ParamManager::singleton()->getEscAgingTestDataParamPtr(0);
  } else if (node_id == 33) {
    aging_test_data = ParamManager::singleton()->getEscAgingTestDataParamPtr(1);
  } else if (node_id == 34) {
    aging_test_data = ParamManager::singleton()->getEscAgingTestDataParamPtr(2);
  } else if (node_id == 35) {
    aging_test_data = ParamManager::singleton()->getEscAgingTestDataParamPtr(3);
  } else {
    qWarning() << "parseAgingTestData: unknown node_id :" << node_id;
    return;
  }

  aging_test_data->adc1 = data_array.popFrontUint16();
  aging_test_data->adc2 = data_array.popFrontUint16();

  aging_test_data->recv_pwm = data_array.popFrontUint16() / 10.0f;
  aging_test_data->comm_pwm = data_array.popFrontUint16() / 10.0f;

  aging_test_data->phase_voltage = data_array.popFrontInt16() / 10.0f;

  aging_test_data->current_res = data_array.popFrontInt16() / 10.0f;
  aging_test_data->current_acs = data_array.popFrontInt16() / 10.0f;
  aging_test_data->bus_current = data_array.popFrontInt16() / 10.0f;

  aging_test_data->voltage = data_array.popFrontUint16() / 10.0f;
  aging_test_data->v_modulation = data_array.popFrontUint16() / 10.0f;

  aging_test_data->mos_temp = data_array.popFrontInt16() / 10.0f;
  aging_test_data->mos2_temp = data_array.popFrontInt16() / 10.0f;

  aging_test_data->cap_temp = data_array.popFrontInt16() / 10.0f;
  aging_test_data->mcu_temp = data_array.popFrontInt16() / 10.0f;

  aging_test_data->running_error = data_array.popFrontUint16();
  aging_test_data->selfcheck_error = data_array.popFrontUint16();
  aging_test_data->selfcheck_value = data_array.popFrontInt16();
  aging_test_data->selfcheck_phase = data_array.popFrontInt16();

  aging_test_data->motor_temp = data_array.popFrontInt16() / 10.0f;

  aging_test_data->time_10ms = data_array.popFrontUint16();

  aging_test_data->encoder_angle = data_array.popFrontUint16();
  aging_test_data->encoder_temp = data_array.popFrontInt16() / 10.0f;

  aging_test_data->v5_V = data_array.popFrontInt16() / 10.0f;
  aging_test_data->v15_V = data_array.popFrontInt16() / 10.0f;

  RealTimeDataManager::singleton()->addAgingTestPoint(
      node_id, test_count, test_point_index, test_point_time_ms,
      *aging_test_data);
}

int Command::agingTestAllocateESC(quint8 esc_id) {
  int retry_cnt = 0;

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = DEVICE_AGING_TEST_NODE;
  packet.data_type_id = COMM_AGING_TEST_CTRL;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data1_field;
  data1_field.clear();
  data1_field.appendUint8(1);       // cmd_id
  data1_field.appendUint8(1);       // opocode
  data1_field.appendUint8(esc_id);  // esc_id
  data1_field.appendUint16(0);      // resv_0
  data1_field.appendUint16(0);      // resv_1

  packet.data_len = data1_field.length();

  QByteArray cmd_1 = Utility::encodePacket(packet, data1_field);

  if (sendCmd(cmd_1, packet.data_type_id) == false) {
    return 1;  // 指令通信故障
  }

  CustomByteArray data2_field;
  data2_field.clear();
  data2_field.appendUint8(1);       // cmd_id
  data2_field.appendUint8(0);       // opocode
  data2_field.appendUint8(esc_id);  // esc_id
  data2_field.appendUint16(0);      // resv_0
  data2_field.appendUint16(0);      // resv_1

  packet.data_len = data2_field.length();

  QByteArray cmd_2 = Utility::encodePacket(packet, data2_field);

  AgingTestCmdParam* test_cmd =
      ParamManager::singleton()->getAgingTestCmdParamPrt();
  test_cmd->test_ctrl_result = -1;
  test_cmd->test_ctrl_flag = 0;

  while (retry_cnt++ < 5) {
    if (sendCmd(cmd_2, packet.data_type_id) == false) {
      return 1;  // 指令通信故障
    }

    if (test_cmd->test_ctrl_result == 0) {
      return 0;  // 分配结束
    } else if (test_cmd->test_ctrl_result == 2) {
      return 2;  // 分配ID失败
    }

    Utility::DelayMs(500);
  }

  return 3;  // 分配ID超时
}

int Command::agingTestStart(quint8 esc_id_flag) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = DEVICE_AGING_TEST_NODE;
  packet.data_type_id = COMM_AGING_TEST_CTRL;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  quint16 pwm_acc_time =
      AgingTestDataManager::singleton()->getAgingPwmAccDecTimeMs();

  CustomByteArray data1_field;
  data1_field.clear();
  data1_field.appendUint8(2);              // cmd_id
  data1_field.appendUint8(1);              // opocode
  data1_field.appendUint8(esc_id_flag);    // esc_id
  data1_field.appendUint16(pwm_acc_time);  // resv_0
  data1_field.appendUint16(0);             // resv_1

  packet.data_len = data1_field.length();

  QByteArray cmd_1 = Utility::encodePacket(packet, data1_field);

  if (sendCmd(cmd_1, packet.data_type_id) == false) {
    return 1;  // 指令通信故障
  }

  return 0;
}

int Command::agingTestStop(quint8 esc_id_flag) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = DEVICE_AGING_TEST_NODE;
  packet.data_type_id = COMM_AGING_TEST_CTRL;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  quint16 pwm_acc_time =
      AgingTestDataManager::singleton()->getAgingPwmAccDecTimeMs();

  CustomByteArray data1_field;
  data1_field.clear();
  data1_field.appendUint8(3);              // cmd_id
  data1_field.appendUint8(1);              // opocode
  data1_field.appendUint8(esc_id_flag);    // esc_id
  data1_field.appendUint16(pwm_acc_time);  // resv_0
  data1_field.appendUint16(0);             // resv_1

  packet.data_len = data1_field.length();

  QByteArray cmd_1 = Utility::encodePacket(packet, data1_field);

  if (sendCmd(cmd_1, packet.data_type_id) == false) {
    return 1;  // 指令通信故障
  }

  return 0;
}

int Command::agingTestPoint() {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = DEVICE_AGING_TEST_NODE;
  packet.data_type_id = COMM_AGING_TEST_POINT;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  QVector<AgingTestCasePoint> test_point_list =
      AgingTestDataManager::singleton()->getAgingTestPoint();

  quint8 test_point_num = test_point_list.size();

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint8(test_point_num);  // cmd_id

  for (int i = 0; i < test_point_num; i++) {
    AgingTestCasePoint point_item = test_point_list[i];
    quint16 send_pwm = point_item.send_pwm * 10;
    quint16 time_100ms = point_item.time * 10;
    quint32 data = send_pwm | (time_100ms << 16);
    data_field.appendUint32(data);  //
  }

  // 添加签名CRC,到包头
  if (data_field.length() > 7) {
    quint16 data_crc = Utility::calcSignatureCrc16(
        UAVCAN_PROTOCOL_AGINGTESTPOINT_SIGNATURE, data_field);
    data_field.push_front((data_crc & 0xff00) >> 8);
    data_field.push_front(data_crc & 0x00ff);
  }

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  if (sendCmd(cmd, packet.data_type_id) == false) {
    return 1;  // 指令通信故障
  }

  return 0;
}

void Command::parseAgingTestCtrl(QByteArray& data) {
  CustomByteArray data_array = data;

  data_array.popFrontUint8();  // cmd_id

  AgingTestCmdParam* test_cmd =
      ParamManager::singleton()->getAgingTestCmdParamPrt();
  test_cmd->test_ctrl_result = data_array.popFrontUint8();
}

void Command::parseAgingTestPoint(QByteArray& data) {
  CustomByteArray data_array = data;

  AgingTestCmdParam* test_cmd =
      ParamManager::singleton()->getAgingTestCmdParamPrt();
  test_cmd->test_point_restult = data_array.popFrontUint8();
}
