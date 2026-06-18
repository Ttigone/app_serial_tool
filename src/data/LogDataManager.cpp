#include "LogDataManager.h"
#include "RealTimeDataManager.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include "Utils/Config.h"

LogDataField::LogDataField() {
  field_name = "";
  field_type = FIELD_UINT16;
}

QObject* LogDataManager::qmlSingleton(QQmlEngine* engine,
                                      QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static LogDataManager* _singleton = nullptr;
  if (!_singleton) _singleton = new LogDataManager;
  return _singleton;
}

LogDataManager* LogDataManager::singleton() {
  return qobject_cast<LogDataManager*>(qmlSingleton(nullptr, nullptr));
}

LogDataManager::LogDataManager() { initWave(); }

LogDataManager::~LogDataManager() {}

int LogDataManager::parseLogFile(QString file_path) {
  int ret = 0;
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

  // 解析.dat文件
  QFile wave_file(file_path);
  if (!wave_file.open(QIODevice::ReadOnly)) {
    return 1;
  }

  if (parseLogHeader(wave_file) == 0) {
    ret = parseLogData(wave_file);
  } else {
    ret = 1;
  }

  wave_file.close();
  return ret;
}

QSharedPointer<QCPGraphDataContainer> LogDataManager::getWaveData(
    QString wave_id) {
  QSharedPointer<QCPGraphDataContainer> data_point = NULL;

  WaveData* wave_data = NULL;
  if (m_DataMap.contains(wave_id)) {
    wave_data = m_DataMap[wave_id];
    data_point = wave_data->m_DataBufferPointer;
  }
  return data_point;
}

QVariantMap LogDataManager::getFieldList() { return m_LogDataFiledList; }

int LogDataManager::parseLogHeader(QFile& file_handle) {
  QString line_str;
  QStringList line_list;

  line_str = file_handle.readLine();  // blackbox version

  line_str = file_handle.readLine();  // field_name

  if (line_str.contains("field_name") == false) {
    return 1;
  }
  line_list = line_str.remove("\n").split(":");
  QString field_names = line_list[1];
  QStringList field_name_list = field_names.split(",");

  line_str = file_handle.readLine();  // field_type

  if (line_str.contains("field_type") == false) {
    return 1;
  }
  line_list = line_str.remove("\n").split(":");
  QString field_types = line_list[1];
  QStringList field_type_list = field_types.split(",");
  if (field_type_list.size() != field_name_list.size()) {
    return 1;
  }

  line_str = file_handle.readLine();  // field_scale

  if (line_str.contains("field_scale") == false) {
    return 1;
  }
  line_list = line_str.remove("\n").split(":");
  QString field_scale = line_list[1];
  QStringList field_scale_list = field_scale.split(",");
  if (field_scale_list.size() != field_name_list.size()) {
    return 1;
  }

  m_LogDataField.clear();
  m_LogDataFiledList.clear();

  m_LogFiledSize = 2;
  // 所有 名称
  for (int i = 0; i < field_name_list.size(); i++) {
    QString data_key_str = QString("%1").arg(i, 2, 10, QLatin1Char('0'));

    LogDataField field_info;
    field_info.field_name = field_name_list[i];
    field_info.field_type = field_type_list[i].toInt();
    field_info.field_scale = field_scale_list[i].toInt();

    m_LogDataField.append(field_info);

    QVariantMap item_map;
    item_map.insert("fieldID", data_key_str);
    item_map.insert("fieldName", field_info.field_name);
    item_map.insert("fieldChecked", false);

    m_LogDataFiledList.insert(data_key_str, item_map);
    // m_LogDataFiledList.insert(QString::number(i), item_map);

    switch (field_info.field_type) {
      case FIELD_INT8:
      case FIELD_UINT8:
        m_LogFiledSize += 1;
        break;
      case FIELD_INT16:
      case FIELD_UINT16:
        m_LogFiledSize += 2;
        break;
      case FIELD_INT32:
      case FIELD_UINT32:
        m_LogFiledSize += 4;
        break;
      default:
        break;
    }
  }

  initWave();
  return 0;
}

int LogDataManager::parseLogData(QFile& file_handle) {
  int file_size = file_handle.size();
  if ((file_size - 4096) % m_LogFiledSize != 0) {
    return 2;
  }

  QMap<QString, WaveData*>::iterator iter;

  for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++) {
    WaveData* data = iter.value();
    data->clear();
  }

  int data_count = (file_size - 4096) / m_LogFiledSize;

  double data_value;

  file_handle.seek(4096);

  for (int i = 0; i < data_count; i++) {
    CustomByteArray data_array = file_handle.read(m_LogFiledSize);

    quint8 head = data_array.popFrontUint8();

    if (head != 'D') {
      continue;
    }

    // file_index
    data_array.popFrontUint8();

    // time_ms
    //        quint32 time_ms = data_array.popFrontUint32();
    //        time_x = time_ms/1000.0f;
    //        m_DataMap[QString::number(0)]->appendPoint(QCPGraphData(time_x,
    //        time_x));

    for (int j = 0; j < m_LogDataField.size(); j++) {
      LogDataField field_info = m_LogDataField[j];
      QString data_key_str = QString("%1").arg(j, 2, 10, QLatin1Char('0'));

      switch (field_info.field_type) {
        case FIELD_INT8: {
          data_value = data_array.popFrontUint8() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        case FIELD_UINT8: {
          data_value = data_array.popFrontInt8() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        case FIELD_INT16: {
          data_value = data_array.popFrontInt16() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        case FIELD_UINT16: {
          data_value = data_array.popFrontUint16() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        case FIELD_INT32: {
          data_value = data_array.popFrontInt32() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        case FIELD_UINT32: {
          data_value = data_array.popFrontUint32() / field_info.field_scale;
          m_DataMap[data_key_str]->appendPoint(QCPGraphData(i, data_value));
          break;
        }
        default:
          break;
      }
    }
  }

  return 0;
}

void LogDataManager::initWave() {
  QMap<QString, WaveData*>::iterator iter;
  for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++) {
    WaveData* data = iter.value();
    delete data;
  }
  m_DataMap.clear();

  for (int i = 0; i < m_LogDataField.size(); i++) {
    QString data_key_str = QString("%1").arg(i, 2, 10, QLatin1Char('0'));

    addWave(data_key_str);
  }
}

void LogDataManager::addWave(QString data_id) {
  if (!m_DataMap.contains(data_id)) {
    WaveData* data = new WaveData();
    m_DataMap[data_id] = data;
  }
}

void LogDataManager::saveCsvFile(QString file_path) {
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

  QFile file(file_path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QTextStream write_stream(&file);

  int data_index = 0;

  QString header_str = "";
  for (int i = 0; i < m_LogDataField.size(); i++) {
    data_index++;

    LogDataField field_info = m_LogDataField[i];
    header_str += field_info.field_name;
    if (data_index <= (m_LogDataField.size() - 1)) {
      header_str += ',';
    }
  }

  write_stream << header_str << '\n';

  QMap<QString, WaveData*>::const_iterator iter = m_DataMap.constBegin();

  int data_count = iter.value()->m_DataBufferPointer->size();

  QCPDataContainer<QCPGraphData>::const_iterator data_iter;

  for (int i = 0; i < data_count; i++) {
    QString data_info = "";
    data_index = 0;

    for (iter = m_DataMap.constBegin(); iter != m_DataMap.constEnd(); ++iter) {
      data_index++;
      data_iter = iter.value()->m_DataBufferPointer->at(i);
      data_info += QString::number(data_iter->mainValue(), 'f', 1);

      if (data_index <= (m_DataMap.size() - 1)) {
        data_info += ',';
      }
    }

    write_stream << data_info << '\n';
  }

  file.close();
}
