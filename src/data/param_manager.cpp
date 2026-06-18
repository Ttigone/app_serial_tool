#include "ParamManager.h"
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QtMath>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include "Utils/Config.h"
#include "Utils/Utility.h"
#include "Utils/Linguist.h"
#include "Comm/CommManager.h"
#include <QApplication>
#include <QDateTime>

QObject* ParamManager::qmlSingleton(QQmlEngine* engine,
                                    QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static ParamManager* _singleton = nullptr;
  if (!_singleton) _singleton = new ParamManager;
  return _singleton;
}

ParamManager* ParamManager::singleton() {
  return qobject_cast<ParamManager*>(qmlSingleton(nullptr, nullptr));
}

ParamManager::ParamManager() {
  qRegisterMetaType<EscInfoParam>();
  qRegisterMetaType<EncoderInfoParam>();

  qRegisterMetaType<EscAgingTestDataParam>();

  initParam();

#ifdef Android_Platform
  QString dir = "assets:/param_config/";
#else
  QString dir = qApp->applicationDirPath();
  dir += "/assets/param_config/";
#endif
  QString config_param_xml = dir + "motor_param.xml";
  loadXml(config_param_xml);
}

ParamManager::~ParamManager() {}

bool ParamManager::loadXml(QString fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QXmlStreamReader stream(&file);
  QString group_name = "";
  Parameter param;

  while (!stream.atEnd()) {
    if (stream.isStartElement()) {
      QString node_name = stream.name().toString();

      if (node_name == "group") {
        QXmlStreamAttributes attrs = stream.attributes();

        group_name = attrs.value("name").toString();
      }

      if (node_name == "parameter") {
        QXmlStreamAttributes attrs = stream.attributes();

        bool flag;

        param.m_GroupName = group_name;
        param.m_ParamName = attrs.value("name").toString();
        param.m_ParamID = attrs.value("addr").toInt(&flag, 16);
        param.m_ValueData = attrs.value("default").toInt();

        QString display_type = attrs.value("display_type").toString();
        if (display_type == "int") {
          param.m_DisplayType = DisplayDataType::DISPLAY_TYPE_INT;
        } else if (display_type == "float") {
          param.m_DisplayType = DisplayDataType::DISPLAY_TYPE_FLOAT;
        } else if (display_type == "enum") {
          param.m_DisplayType = DisplayDataType::DISPLAY_TYPE_ENUM;
        } else if (display_type == "string") {
          param.m_DisplayType = DisplayDataType::DISPLAY_TYPE_STRING;
        } else if (display_type == "int_array") {
          param.m_DisplayType = DisplayDataType::DISPLAY_TYPE_INT_ARRAY;
        }

        QString data_type = attrs.value("data_type").toString();
        if (data_type == "uint16") {
          param.m_DataType = ParamDataType::DATA_UINT16;
        } else if (data_type == "int16") {
          param.m_DataType = ParamDataType::DATA_INT16;
        } else if (data_type == "int32") {
          param.m_DataType = ParamDataType::DATA_INT32;
        } else if (data_type == "uint32") {
          param.m_DataType = ParamDataType::DATA_UINT32;
        } else if (data_type == "int8_array") {
          param.m_DataType = ParamDataType::DATA_INT8_ARRAY;
        } else if (data_type == "int16_array") {
          param.m_DataType = ParamDataType::DATA_INT16_ARRAY;
        }

        QString attr = attrs.value("attr").toString();
        if (attr == "ro") {
          param.m_Attr = ParamAttrType::ATTR_RO;

        } else {
          param.m_Attr = ParamAttrType::ATTR_RW;
        }
      }

      if (node_name == "display_name_en") {
        param.m_DisplayName = stream.readElementText();
      }

      if (node_name == "display_name_cn") {
        param.m_DisplayNameCN = stream.readElementText();
      }

      if (node_name == "short_desc_en") {
        param.m_ShortDescInfo = stream.readElementText();
      }

      if (node_name == "short_desc_cn") {
        param.m_ShortDescInfoCN = stream.readElementText();
      }

      if (node_name == "decimal") {
        param.m_Decimals = stream.readElementText().toInt();
        param.m_Scale = qPow(10, param.m_Decimals);
      }

      if (node_name == "min") {
        param.m_MinValue = stream.readElementText().toInt();
      }

      if (node_name == "max") {
        param.m_MaxValue = stream.readElementText().toInt();
      }

      if (node_name == "unit") {
        param.m_Suffix = stream.readElementText();
      }

      if (node_name == "enum_item_en") {
        param.m_EnumItemList.append(stream.readElementText());
      }

      if (node_name == "enum_item_cn") {
        param.m_EnumItemCNList.append(stream.readElementText());
      }

      if (node_name == "dim") {
        param.m_ValueArrayDim = stream.readElementText().toInt();
      }
    } else if (stream.isEndElement()) {
      QString node_name = stream.name().toString();
      if (node_name == "group") {
      }

      if (node_name == "parameter") {
        if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_ENUM) {
          param.m_MinValue = 0;
          param.m_MaxValue = param.m_EnumItemList.size() - 1;
        }

        m_ParamMap.insert(param.m_ParamID, param);

        param.reset();
      }
    }

    stream.readNext();
  }

  file.close();

  return true;
}

void ParamManager::initParam() {
  m_EscDeviceID = 0x20;
  m_EscRealtimeDataParam = new EscRealtimeDataParam();
  m_EscInfoParam = new EscInfoParam();
  m_ConfigInfoParam = new ConfigInfoParam();

  m_EncoderInfoParam = new EncoderInfoParam();

  m_LogFileInfoParam = new LogFileInfoParam();

  m_AgingTestCmdParam = new AgingTestCmdParam();

  for (int i = 0; i < 4; i++) {
    EscAgingTestDataParam* esc_aging_test_param = new EscAgingTestDataParam();

    m_EscAgingTestDataParamList.append(esc_aging_test_param);
  }

  m_TestCaseFlag = false;

  m_TestCaseFileName = "";
}

EscRealtimeDataParam* ParamManager::getEscRealtimeDataParam() {
  return m_EscRealtimeDataParam;
}

EscInfoParam ParamManager::getEscInfoParam() { return *m_EscInfoParam; }

EscInfoParam* ParamManager::getEscInfoParamPtr() { return m_EscInfoParam; }

ConfigInfoParam* ParamManager::getConfigInfoParam() {
  return m_ConfigInfoParam;
}

EncoderInfoParam ParamManager::getEncoderInfoParam() {
  return *m_EncoderInfoParam;
}

EncoderInfoParam* ParamManager::getEncoderInfoParamPtr() {
  return m_EncoderInfoParam;
}

LogFileInfoParam* ParamManager::getLogFileInfoParamPtr() {
  return m_LogFileInfoParam;
}

AgingTestCmdParam* ParamManager::getAgingTestCmdParamPrt() {
  return m_AgingTestCmdParam;
}

EscAgingTestDataParam* ParamManager::getEscAgingTestDataParamPtr(
    int esc_index) {
  return m_EscAgingTestDataParamList[esc_index];
}

EscAgingTestDataParam ParamManager::getEscAgingTestDataParam(int esc_index) {
  return *m_EscAgingTestDataParamList[esc_index];
}

quint8 ParamManager::getEscID() { return m_EscDeviceID; }

void ParamManager::setEscID(quint8 esc_id) { m_EscDeviceID = esc_id; }

void ParamManager::setEscList(QVector<quint8> esc_list) {
  m_EscDeviceIDList = esc_list;
}

void ParamManager::initParamMap() { m_ParamMap.clear(); }

QList<int> ParamManager::getParamList() { return m_ParamMap.keys(); }

Parameter* ParamManager::getParamPtr(int param_id) {
  if (m_ParamMap.contains(param_id)) {
    return &(m_ParamMap[param_id]);
  }
  return NULL;
}

Parameter* ParamManager::getParamPtr(QString param_id) {
  QMap<int, Parameter>::iterator iter = m_ParamMap.begin();

  for (; iter != m_ParamMap.end(); iter++) {
    Parameter param = iter.value();
    if (param.m_ParamName == param_id) {
      return &iter.value();
    }
  }

  return NULL;
}

QVariantMap ParamManager::getParam(int param_id) {
  QVariantMap param_data;

  if (m_ParamMap.contains(param_id)) {
    Parameter param = m_ParamMap[param_id];

    if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
      param_data.insert("param_name", param.m_DisplayNameCN);
    } else {
      param_data.insert("param_name", param.m_DisplayName);
    }

    param_data.insert("cur_value", param.m_ValueData);
    param_data.insert("min_value", param.m_MinValue);
    param_data.insert("max_value", param.m_MaxValue);
    param_data.insert("param_scale", param.m_Scale);
    param_data.insert("param_decimals", param.m_Decimals);
    param_data.insert("suffix", param.m_Suffix);

    if (param.m_Attr == ParamAttrType::ATTR_RW) {
      param_data.insert("rw_attr", "rw");
    } else {
      param_data.insert("rw_attr", "ro");
    }

    QVariantMap enum_map;

    for (int i = 0; i < param.m_EnumItemList.size(); i++) {
      if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
        enum_map.insert(QString::number(i), param.m_EnumItemCNList[i]);
      } else {
        enum_map.insert(QString::number(i), param.m_EnumItemList[i]);
      }
    }

    param_data.insert("enum_list", enum_map);

    if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_STRING) {
      QString value_str = "";
      for (int i = 0; i < param.m_ValueArrayDim; i++) {
        if (param.m_ValueArray[i] == 0) break;
        value_str.append(param.m_ValueArray[i]);
      }

      param_data.insert("array_value", value_str);
    } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_INT_ARRAY) {
      QString value_str = "";
      for (int i = 0; i < param.m_ValueArrayDim; i++) {
        value_str.append(QString::number(param.m_ValueArray[i]));

        if (i != (param.m_ValueArrayDim - 1)) value_str.append(",");
      }

      param_data.insert("array_value", value_str);
    } else {
      param_data.insert("array_value", "");
    }
  }

  return param_data;
}

QVariantMap ParamManager::getParam(QString param_name) {
  QVariantMap param_data;

  QMap<int, Parameter>::iterator iter = m_ParamMap.begin();

  for (; iter != m_ParamMap.end(); iter++) {
    Parameter param = iter.value();
    if (param.m_ParamName == param_name) {
      if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
        param_data.insert("param_name", param.m_DisplayNameCN);
      } else {
        param_data.insert("param_name", param.m_DisplayName);
      }

      param_data.insert("cur_value", param.m_ValueData);
      param_data.insert("min_value", param.m_MinValue);
      param_data.insert("max_value", param.m_MaxValue);
      param_data.insert("param_scale", param.m_Scale);
      param_data.insert("param_decimals", param.m_Decimals);
      param_data.insert("suffix", param.m_Suffix);

      if (param.m_Attr == ParamAttrType::ATTR_RW) {
        param_data.insert("rw_attr", "rw");
      } else {
        param_data.insert("rw_attr", "ro");
      }

      QVariantMap enum_map;

      for (int i = 0; i < param.m_EnumItemList.size(); i++) {
        if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
          enum_map.insert(QString::number(i), param.m_EnumItemCNList[i]);
        } else {
          enum_map.insert(QString::number(i), param.m_EnumItemList[i]);
        }
      }

      param_data.insert("enum_list", enum_map);

      if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_STRING) {
        QString value_str = "";
        for (int i = 0; i < param.m_ValueArrayDim; i++) {
          value_str.append(param.m_ValueArray[i]);
        }

        param_data.insert("array_value", value_str);
      } else if (param.m_DisplayType ==
                 DisplayDataType::DISPLAY_TYPE_INT_ARRAY) {
        QString value_str = "";
        for (int i = 0; i < param.m_ValueArrayDim; i++) {
          value_str.append(QString::number(param.m_ValueArray[i]));

          if (i != (param.m_ValueArrayDim - 1)) value_str.append(",");
        }
        param_data.insert("array_value", value_str);
      } else {
        param_data.insert("array_value", "");
      }

      break;
    }
  }

  return param_data;
}

QVariantMap ParamManager::getParamOverview(int param_id) {
  QVariantMap param_data;

  if (m_ParamMap.contains(param_id)) {
    Parameter param = m_ParamMap[param_id];

    QString param_name = param.m_DisplayName;

    if (param.m_Suffix != "") {
      param_name += "(" + param.m_Suffix + ")";
    }

    param_data.insert("param_name", param_name);

    if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_INT) {
      param_data.insert("value", QString::number(param.m_ValueData));
    } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_FLOAT) {
      double value = (double)param.m_ValueData / param.m_Scale;
      param_data.insert("value", QString::number(value, 'f', param.m_Decimals));
    } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_ENUM) {
      param_data.insert("value", param.m_EnumItemList[param.m_ValueData]);
    }
  }

  return param_data;
}

QVariantMap ParamManager::getGroupParams(QString group_name) {
  QMap<int, Parameter>::iterator iter = m_ParamMap.begin();

  QVariantMap param_list;

  for (; iter != m_ParamMap.end(); iter++) {
    QVariantMap param_data;
    Parameter param = iter.value();

    if (param.m_GroupName == group_name) {
      param_data.insert("param_id", param.m_ParamID);

      if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_INT) {
        param_data.insert("edit_type", "int");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_FLOAT) {
        param_data.insert("edit_type", "float");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_ENUM) {
        param_data.insert("edit_type", "enum");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_STRING) {
        param_data.insert("edit_type", "string");

      } else if (param.m_DisplayType ==
                 DisplayDataType::DISPLAY_TYPE_INT_ARRAY) {
        param_data.insert("edit_type", "int_array");
      }

      if (param.m_Attr == ParamAttrType::ATTR_RW) {
        param_data.insert("rw_attr", "rw");
      } else {
        param_data.insert("rw_attr", "ro");
      }

      param_list.insert(QString::number(param.m_ParamID), param_data);
    }
  }

  return param_list;
}

QVariantMap ParamManager::getGroupParams2(QString group_name) {
  QMap<int, Parameter>::iterator iter = m_ParamMap.begin();

  QVariantMap param_list;

  for (; iter != m_ParamMap.end(); iter++) {
    QVariantMap param_data;
    Parameter param = iter.value();

    if (param.m_GroupName == group_name) {
      param_data.insert("param_id", param.m_ParamID);

      if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
        param_data.insert("param_name", param.m_DisplayNameCN);
      } else {
        param_data.insert("param_name", param.m_DisplayName);
      }

      param_data.insert("suffix", param.m_Suffix);

      if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_INT) {
        param_data.insert("edit_type", "int");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_FLOAT) {
        param_data.insert("edit_type", "float");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_ENUM) {
        param_data.insert("edit_type", "enum");
      } else if (param.m_DisplayType == DisplayDataType::DISPLAY_TYPE_STRING) {
        param_data.insert("edit_type", "string");

      } else if (param.m_DisplayType ==
                 DisplayDataType::DISPLAY_TYPE_INT_ARRAY) {
        param_data.insert("edit_type", "int_array");
      }

      if (param.m_Attr == ParamAttrType::ATTR_RW) {
        param_data.insert("rw_attr", "rw");
      } else {
        param_data.insert("rw_attr", "ro");
      }

      param_list.insert(QString::number(param.m_ParamID), param_data);
    }
  }

  return param_list;
}

void ParamManager::setParamByte(int param_id, CustomByteArray& data_byte) {
  Parameter* param = getParamPtr(param_id);

  if (param == NULL) {
    return;
  }

  switch (param->m_DataType) {
    case DATA_UINT16:
      param->setValue(data_byte.popFrontUint16());
      break;
    case DATA_INT16:
      param->setValue(data_byte.popFrontInt16());
      break;
    case DATA_INT32:
      param->setValue(data_byte.popFrontInt32());
      break;
    case DATA_UINT32:
      param->setValue(data_byte.popFrontUint32());
      break;
    case DATA_INT8_ARRAY:
      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        param->setArrayValue(i, data_byte.popFrontInt8());
      }
      break;
    case DATA_INT16_ARRAY:
      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        param->setArrayValue(i, data_byte.popFrontInt16());
      }
      break;
  }

  emit paramChanged(param_id);
}

void ParamManager::getParamByte(int param_id, CustomByteArray& data_byte) {
  Parameter* param = getParamPtr(param_id);

  if (param == NULL) {
    return;
  }

  switch (param->m_DataType) {
    case DATA_UINT16:
      data_byte.appendUint16(param->m_ValueData);
      break;
    case DATA_INT16:
      data_byte.appendInt16(param->m_ValueData);
      break;
    case DATA_INT32:
      data_byte.appendInt32(param->m_ValueData);
      break;
    case DATA_UINT32:
      data_byte.appendUint32(param->m_ValueData);
      break;
    case DATA_INT8_ARRAY:
      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        data_byte.appendInt8(param->m_ValueArray[i]);
      }
      break;
    case DATA_INT16_ARRAY:
      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        data_byte.appendInt16(param->m_ValueArray[i]);
      }
      break;
    default:
      break;
  }
}

void ParamManager::setParamValueString(int param_id, QString value_str) {
  Parameter* param = getParamPtr(param_id);

  if (param == NULL) {
    return;
  }

  switch (param->m_DisplayType) {
    case DISPLAY_TYPE_INT:
    case DISPLAY_TYPE_FLOAT:
    case DISPLAY_TYPE_ENUM: {
      int value = value_str.toInt();
      param->setValue(value);
    } break;
    case DISPLAY_TYPE_STRING: {
      QByteArray data_array = value_str.toLatin1();
      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        if (i < data_array.size()) {
          param->setArrayValue(i, data_array[i]);
        } else {
          param->setArrayValue(i, 0);
        }
      }
    } break;
    case DISPLAY_TYPE_INT_ARRAY: {
      QStringList data_arry_list = value_str.split(",");

      for (int i = 0; i < param->m_ValueArrayDim; i++) {
        if (i < data_arry_list.size()) {
          param->setArrayValue(i, data_arry_list[i].toInt());
        } else {
          param->setArrayValue(i, 0);
        }
      }
    } break;
    default:
      break;
  }

  emit paramChanged(param_id);
}

void ParamManager::setParamValue(int param_id, int value) {
  Parameter* param = getParamPtr(param_id);

  if (param == NULL) {
    return;
  }

  param->setValue(value);
}

void ParamManager::emitParamChanged(int param_id) {
  emit paramChanged(param_id);
}

int ParamManager::enableTestCase(QString file_path) {
  m_TestCaseFlag = false;
  m_TestCasePoint.clear();

  if (file_path == "") {
    return 0;
  }

#ifdef Android_Platform
  if (file_path.startsWith("file:/")) {
    file_path.remove(0, 6);
  }
#else
  if (file_path.startsWith("file:///")) {
    file_path.remove(0, 8);
  } else if (file_path.startsWith("file:")) {
    file_path.remove(0, 5);
  }
#endif

  QString dir = QDir::currentPath();
  QFile file(file_path);
  if (!file.exists()) {
    return 0;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return 0;
  }

  QString test_case_text = file.readAll();
  QJsonParseError parseJsonErr;
  QJsonDocument document =
      QJsonDocument::fromJson(test_case_text.toUtf8(), &parseJsonErr);
  if ((parseJsonErr.error == QJsonParseError::NoError)) {
    QJsonObject test_case_point_list = document.object();

    for (QJsonObject::Iterator iter = test_case_point_list.begin();
         iter != test_case_point_list.end(); iter++) {
      TestCasePoint point_value;

      QJsonValue value = iter.value();
      QJsonObject test_case_point = value.toObject();

      if (test_case_point.contains("time")) {
        QJsonValue tmp_value = test_case_point.take("time");
        point_value.time = tmp_value.toInt();
      }

      if (test_case_point.contains("ppm_acc_time")) {
        QJsonValue tmp_value = test_case_point.take("ppm_acc_time");
        point_value.ppm_acc_time = tmp_value.toInt();
      }

      if (test_case_point.contains("send_ppm")) {
        QJsonValue tmp_value = test_case_point.take("send_ppm");
        point_value.send_ppm = tmp_value.toInt();
      }

      if (test_case_point.contains("recv_ppm_max")) {
        QJsonValue tmp_value = test_case_point.take("recv_ppm_max");
        point_value.recv_ppm_max = tmp_value.toInt();
      }

      if (test_case_point.contains("recv_ppm_min")) {
        QJsonValue tmp_value = test_case_point.take("recv_ppm_min");
        point_value.recv_ppm_min = tmp_value.toInt();
      }

      if (test_case_point.contains("speed_max")) {
        QJsonValue tmp_value = test_case_point.take("speed_max");
        point_value.speed_max = tmp_value.toInt();
      }

      if (test_case_point.contains("speed_min")) {
        QJsonValue tmp_value = test_case_point.take("speed_min");
        point_value.speed_min = tmp_value.toInt();
      }

      if (test_case_point.contains("current_max")) {
        QJsonValue tmp_value = test_case_point.take("current_max");
        point_value.current_max = tmp_value.toInt();
      }

      if (test_case_point.contains("current_min")) {
        QJsonValue tmp_value = test_case_point.take("current_min");
        point_value.current_min = tmp_value.toInt();
      }

      if (test_case_point.contains("voltage_max")) {
        QJsonValue tmp_value = test_case_point.take("voltage_max");
        point_value.voltage_max = tmp_value.toInt();
      }

      if (test_case_point.contains("voltage_min")) {
        QJsonValue tmp_value = test_case_point.take("voltage_min");
        point_value.voltage_min = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_cap_max")) {
        QJsonValue tmp_value = test_case_point.take("temp_cap_max");
        point_value.temp_cap_max = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_cap_min")) {
        QJsonValue tmp_value = test_case_point.take("temp_cap_min");
        point_value.temp_cap_min = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_mcu_max")) {
        QJsonValue tmp_value = test_case_point.take("temp_mcu_max");
        point_value.temp_mcu_max = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_mcu_min")) {
        QJsonValue tmp_value = test_case_point.take("temp_mcu_min");
        point_value.temp_mcu_min = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_mos_max")) {
        QJsonValue tmp_value = test_case_point.take("temp_mos_max");
        point_value.temp_mos_max = tmp_value.toInt();
      }

      if (test_case_point.contains("temp_mos_min")) {
        QJsonValue tmp_value = test_case_point.take("temp_mos_min");
        point_value.temp_mos_min = tmp_value.toInt();
      }

      m_TestCasePoint.append(point_value);
    }
  }

  file.close();

  QFileInfo file_info(file_path);
  m_TestCaseFileName = file_info.fileName();
  m_TestCaseFlag = true;
  return m_TestCasePoint.size();
}

QVariantMap ParamManager::getTestCasePoint(int index) {
  QVariantMap test_case_point;

  if (index < m_TestCasePoint.size()) {
    TestCasePoint point = m_TestCasePoint[index];

    test_case_point.insert("time", point.time);
    test_case_point.insert("acc_time", point.ppm_acc_time);
    test_case_point.insert("send_pwm", point.send_ppm);
    test_case_point.insert("recv_pwm_min", point.recv_ppm_min);
    test_case_point.insert("recv_pwm_max", point.recv_ppm_max);
    test_case_point.insert("speed_min", point.speed_min);
    test_case_point.insert("speed_max", point.speed_max);
    test_case_point.insert("current_min", point.current_min);
    test_case_point.insert("current_max", point.current_max);
    test_case_point.insert("voltage_min", point.voltage_min);
    test_case_point.insert("voltage_max", point.voltage_max);
    test_case_point.insert("mos_temp_min", point.temp_mos_min);
    test_case_point.insert("mos_temp_max", point.temp_mos_max);
    test_case_point.insert("cap_temp_min", point.temp_cap_min);
    test_case_point.insert("cap_temp_max", point.temp_cap_max);
    test_case_point.insert("mcu_temp_min", point.temp_mcu_min);
    test_case_point.insert("mcu_temp_max", point.temp_mcu_max);
  }

  return test_case_point;
}

bool ParamManager::getTestCaseFlag() { return m_TestCaseFlag; }

QString ParamManager::getTestCaseFileName() { return m_TestCaseFileName; }

void ParamManager::resseTestCase() {
  m_TestIndex = 0;
  m_TestTimeStart = QDateTime::currentMSecsSinceEpoch();
  m_TestTimeCurrent = QDateTime::currentMSecsSinceEpoch();

  m_TestCurrentTimeUse = 0;
  m_ErrorInfo = "";
  m_ErrorFlag = 0;
  m_TestSendPPM = 9600;
}

int ParamManager::processTestCase(int esc_id, int time_interval) {
  int ret = 0;
  TestCasePoint test_case = m_TestCasePoint[m_TestIndex];

  m_TestTimeCurrent = QDateTime::currentMSecsSinceEpoch();

  m_TestCurrentTimeUse = m_TestTimeCurrent - m_TestTimeStart;

  if (m_TestCasePoint.size() >= 2) {
    if (m_TestIndex == 0) {
      m_TestStartSendPPM = test_case.send_ppm;
      m_TestEndSendPPM = test_case.send_ppm;
      m_TestSendPPM = m_TestStartSendPPM;
    } else {
      TestCasePoint last_test_case = m_TestCasePoint[m_TestIndex - 1];
      m_TestStartSendPPM = last_test_case.send_ppm;
      m_TestEndSendPPM = test_case.send_ppm;
    }
  } else {
    m_TestStartSendPPM = 9600;
    m_TestEndSendPPM = test_case.send_ppm;
  }

  if (m_TestCurrentTimeUse <= test_case.ppm_acc_time) {
    m_TestSendPPM = (m_TestEndSendPPM - m_TestStartSendPPM) *
                        m_TestCurrentTimeUse / (float)test_case.ppm_acc_time +
                    m_TestStartSendPPM;
  } else {
    m_TestSendPPM = m_TestEndSendPPM;

    // 延迟400ms才开始判断
    if (m_TestCurrentTimeUse > (test_case.ppm_acc_time + 400)) {
      test_case.JudgeStatusParam(m_TestIndex, m_EscRealtimeDataParam,
                                 m_ErrorInfo, m_ErrorFlag);
    }
  }

  if (m_TestCurrentTimeUse >= test_case.time) {
    m_TestTimeStart = QDateTime::currentMSecsSinceEpoch();

    m_TestIndex++;
    if (m_TestIndex >= m_TestCasePoint.size()) {
      if (m_ErrorFlag == 0) {
        ret = 1;
      } else {
        m_ErrorInfo += QString(
            QObject::tr("If a fault occurs, please contact an engineer"));
        ret = 2;
      }
      return ret;
    }

    m_TestCurrentTimeUse = 0;
  }

  // 更新PWM
  CommManager::singleton()->command()->setPWM(esc_id, m_TestSendPPM);

  if (m_EscRealtimeDataParam->running_error & 0x0001) {
    m_ErrorInfo = QString(
        QObject::tr("Overcurrent fault, please contact the engineer\n"));

    ret = 2;
    return ret;
  }

  return ret;
}

void ParamManager::resetFreqTest() {
  m_TestTimeStart = QDateTime::currentMSecsSinceEpoch();
  m_TestTimeCurrent = QDateTime::currentMSecsSinceEpoch();

  m_TestCurrentTimeUse = 0;

  m_FreqScanTimeUse = 0;
  m_FreqScanFreq = Config::singleton()->getScanFreqStartFreq();
  m_FreqScanEndFreq = Config::singleton()->getScanFreqEndFreq();
  m_FreqScanStep = Config::singleton()->getScanFreqStep();
  m_FreqScanFreqCount = 0;
  m_FreqScanFreqMaxCount = Config::singleton()->getScanFreqCount();
  m_FreqScanMinPwm = Config::singleton()->getScanFreqMinPwm() * 10;
  m_FreqScanMaxPwm = Config::singleton()->getScanFreqMaxPwm() * 10;
  m_FreqScanMiddlePwm = (m_FreqScanMinPwm + m_FreqScanMaxPwm) / 2;
  m_FreqScanPwmRange = m_FreqScanMaxPwm - m_FreqScanMinPwm;
}

void ParamManager::processFreqTest(int esc_id, int time_interval,
                                   int wave_type) {
  if (m_TestCurrentTimeUse < 500) {
    m_TestCurrentTimeUse += time_interval;
    m_TestSendPPM = 9600;
  } else if (m_TestCurrentTimeUse < 1000) {
    m_TestCurrentTimeUse += time_interval;
    m_TestSendPPM = m_FreqScanMiddlePwm;

    m_TestTimeStart = QDateTime::currentMSecsSinceEpoch();
  } else {
    m_TestTimeCurrent = QDateTime::currentMSecsSinceEpoch();
    m_FreqScanTimeUse = m_TestTimeCurrent - m_TestTimeStart;

    if (wave_type == 0) {
      m_TestSendPPM = m_FreqScanPwmRange * 0.5 *
                          qSin(2 * 3.141592654 * m_FreqScanFreq *
                               m_FreqScanTimeUse / 1000.0) +
                      m_FreqScanMiddlePwm;
    } else {
      if (m_FreqScanTimeUse < 500.0 / m_FreqScanFreq) {
        m_TestSendPPM = m_FreqScanMinPwm;
      } else {
        m_TestSendPPM = m_FreqScanMaxPwm;
      }
    }

    if (m_FreqScanTimeUse > 1000.0 / m_FreqScanFreq) {
      m_TestTimeStart = QDateTime::currentMSecsSinceEpoch();

      m_FreqScanTimeUse = 0;
      m_FreqScanFreqCount++;
      if (m_FreqScanFreqCount >= m_FreqScanFreqMaxCount) {
        m_FreqScanFreqCount = 0;

        m_FreqScanFreq += m_FreqScanStep;

        if (Utility::FloatGreater(m_FreqScanFreq, m_FreqScanEndFreq)) {
          m_FreqScanFreq = Config::singleton()->getScanFreqStartFreq();
        }
      }
    }
  }

  CommManager::singleton()->command()->setPWM(esc_id, m_TestSendPPM);
}

QString ParamManager::getTestCaseErrorInfo() { return m_ErrorInfo; }

QString ParamManager::getSelfCheckErrorInfo(quint16 self_error) {
  return m_EscErrorInfo.getSelfCheckErrorInfo(self_error);
}

QString ParamManager::getRunningErrorInfo(quint16 running_error) {
  return m_EscErrorInfo.getRunningErrorInfo(running_error);
}
