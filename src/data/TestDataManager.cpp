#include "TestDataManager.h"
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

TestDataDesc::TestDataDesc() {
  date = "";
  time = "";
  sn_code = "";

  desc_path = "";
  data_path = "";
}

QObject* TestDataManager::qmlSingleton(QQmlEngine* engine,
                                       QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static TestDataManager* _singleton = nullptr;
  if (!_singleton) _singleton = new TestDataManager;
  return _singleton;
}

TestDataManager* TestDataManager::singleton() {
  return qobject_cast<TestDataManager*>(qmlSingleton(nullptr, nullptr));
}

TestDataManager::TestDataManager() {
  initWave();
  m_DataSaveFlag = false;
}

TestDataManager::~TestDataManager() {}

void TestDataManager::setDataSaveFlag(bool flag) {
  m_DataSaveFlag = flag;
  m_DataSaveIndex = 0;
  m_BufferTimeLength =
      60 * Config::singleton()->getStorageDuration();  // 缓存10min
}

void TestDataManager::saveData(QString esc_sn, QString log_time,
                               bool last_time) {
  if (m_DataSaveFlag) {
    return;
  }

  QSharedPointer<QCPGraphDataContainer> data =
      RealTimeDataManager::singleton()->getData("recv_pwm");

  bool found_range;
  QCPRange range = data->keyRange(found_range);
  if (!found_range) {
    return;
  }

  float end_save_time = 0;

  float start_time = range.lower;
  float end_time = range.upper;
  float time_length = end_time - start_time;

  if (last_time) {
    end_save_time = end_time + 1;
  } else {
    // 每次存储最后60s之前的的缓存数据
    if (Utility::FloatGreater(time_length, m_BufferTimeLength + 60)) {
      end_save_time = end_time - 60;
    } else {
      return;  // 不存储
    }
  }

  // 计算data_size
  int data_count = 0;
  QCPDataContainer<QCPGraphData>::iterator iter;
  for (iter = data->begin(); iter != data->end(); iter++) {
    double key = iter->mainKey();
    if (key < end_save_time) {
      data_count++;
    } else {
      break;
    }
  }

  int data_size = data_count * sizeof(double);

  QString esc_sn_str = esc_sn.remove(QRegExp("\\s"));
  if (esc_sn_str.isEmpty()) {
    esc_sn_str = "Unknown";
  }

  QDir folder;

  QString dir = Config::singleton()->getTestDataPath();

  QDateTime current_date_time = QDateTime::currentDateTime();

  QString current_date = current_date_time.toString("yyyy-MM-dd");

  dir += "/" + current_date + "/" + esc_sn_str;

  // 创建文件夹
  if (!folder.exists(dir)) {
    folder.mkpath(dir);
  }

  QString wave_file_name =
      QString("%1_%2.dat").arg(log_time).arg(m_DataSaveIndex);
  QString wave_file_path = QString("%1/%2").arg(dir).arg(wave_file_name);

  QFile wave_file(wave_file_path);
  if (!wave_file.open(QIODevice::ReadWrite)) {
    return;
  }

  m_DataSaveIndex++;

  int offset = 64 * 128;

  saveWaveTitle(wave_file, "time", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "time_10ms", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "recv_pwm", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "comm_pwm", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "speed", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "current", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "bus_current", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "voltage", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "v_modulation", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "mos_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "cap_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "mcu_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "motor_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "running_error", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "selfcheck_error", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "speed_km_h", offset, data_size);

  wave_file.seek(64 * 128);
  // 保存时间轴x
  data = RealTimeDataManager::singleton()->getData("recv_pwm");
  saveWaveTime(wave_file, data, end_save_time);

  data = RealTimeDataManager::singleton()->getData("time_10ms");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("recv_pwm");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("comm_pwm");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("speed");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("current");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("bus_current");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("voltage");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("v_modulation");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("mos_temp");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("cap_temp");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("mcu_temp");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("motor_temp");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("running_error");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("selfcheck_error");
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getData("speed_km_h");
  saveWaveData(wave_file, data, end_save_time, last_time);

  wave_file.close();
}

void TestDataManager::clearHistoryData() {
  QString dir_path = Config::singleton()->getTestDataPath();

  QDir dir(dir_path);

  dir.removeRecursively();
}

QVariantMap TestDataManager::getTestDataDesc() {
  QString test_data_dir = Config::singleton()->getTestDataPath();
  QVector<TestDataDesc> test_data_desc;

  QDir root_dir(test_data_dir);

  QFileInfoList date_list =
      root_dir.entryInfoList(root_dir.filter() | QDir::NoDotAndDotDot,
                             root_dir.sorting() | QDir::Time);

  for (int i = 0; i < date_list.size(); i++) {
    QString date_str = date_list[i].fileName();
    QString date_path = test_data_dir + "/" + date_str;

    scanTestDataDesc(date_path, test_data_desc);
  }

  QVariantMap test_data_map;

  for (int i = 0; i < test_data_desc.size(); i++) {
    TestDataDesc item_desc = test_data_desc[i];

    QVariantMap item_map;
    item_map.insert("time", item_desc.date + " " + item_desc.time);
    item_map.insert("sn_code", item_desc.sn_code);
    item_map.insert("desc_path", item_desc.desc_path);
    item_map.insert("data_path", item_desc.data_path);

    test_data_map.insert(QString::number(i), item_map);
  }

  return test_data_map;
}

QString TestDataManager::getLastDirPath() {
  QString dir_path = Config::singleton()->getTestDataPath();

  QDir root_dir(dir_path);

  QString date_path = dir_path;
  QFileInfoList date_list =
      root_dir.entryInfoList(root_dir.filter() | QDir::NoDotAndDotDot,
                             root_dir.sorting() | QDir::Time);

  for (int i = 0; i < date_list.size(); i++) {
    QString date_str = date_list[i].fileName();
    date_path = dir_path + "/" + date_str;

    break;
  }

  return date_path;
}

QVariantMap TestDataManager::getDirTestDataDesc(QString dir_path) {
#ifdef Android_Platform
  if (dir_path.startsWith("file:/")) {
    dir_path.remove(0, 6);
  }
#else
  if (dir_path.startsWith("file:///")) {
    dir_path.remove(0, 8);
  } else if (dir_path.startsWith("file:")) {
    dir_path.remove(0, 5);
  }
#endif

  QVector<TestDataDesc> test_data_desc;

  scanTestDataDesc(dir_path, test_data_desc);

  QVariantMap test_data_map;

  for (int i = 0; i < test_data_desc.size(); i++) {
    TestDataDesc item_desc = test_data_desc[i];

    QVariantMap item_map;
    item_map.insert("time", item_desc.date + " " + item_desc.time);
    item_map.insert("sn_code", item_desc.sn_code);
    item_map.insert("desc_path", item_desc.desc_path);
    item_map.insert("data_path", item_desc.data_path);

    test_data_map.insert(QString::number(i), item_map);
  }

  return test_data_map;
}

void TestDataManager::parseData(QString desc_path, QString data_path) {
  if (desc_path != data_path) {
    QFile desc_file(desc_path);
    if (!desc_file.open(QIODevice::ReadOnly)) {
      return;
    }

    QFile wave_file(data_path);
    if (!wave_file.open(QIODevice::ReadOnly)) {
      return;
    }

    QString desc_str = desc_file.readAll();
    desc_file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document =
        QJsonDocument::fromJson(desc_str.toUtf8(), &parseJsonErr);
    if ((parseJsonErr.error == QJsonParseError::NoError)) {
      if (document.isObject()) {
        QJsonObject obj = document.object();

        if (obj.contains("desc")) {
          QJsonArray desc_array = obj.take("desc").toArray();

          for (int i = 0; i < desc_array.size(); ++i) {
            QJsonValue item = desc_array.at(i);

            QJsonObject item_obj = item.toObject();
            QString name = item_obj.take("name").toString();
            int offset = item_obj.take("offset").toInt();
            int size = item_obj.take("size").toInt();
            parseDataFile(name, offset, size, wave_file);
          }

          syncWave();
        }
      }
    }
    wave_file.close();
  } else {
    // 解析.dat文件
    QFile wave_file(data_path);
    if (!wave_file.open(QIODevice::ReadOnly)) {
      return;
    }

    quint8 wave_info_buffer[64];

    for (int i = 0; i < 128; i++) {
      wave_file.seek(i * 64);
      wave_file.read((char*)wave_info_buffer, 64);
      QByteArray head_byte((char*)wave_info_buffer, 64);
      CustomByteArray wave_info_array(head_byte);
      quint16 flag = wave_info_array.popFrontUint16();

      if (flag == 0x0100) {
        int offset = wave_info_array.popFrontInt32();
        int size = wave_info_array.popFrontInt32();
        QString name(head_byte.mid(24));

        parseDataFile(name, offset, size, wave_file);
      } else {
        break;
      }
    }
    syncWave();

    wave_file.close();
  }
}

QSharedPointer<QCPGraphDataContainer> TestDataManager::getWaveData(
    QString wave_id) {
  QSharedPointer<QCPGraphDataContainer> data_point = NULL;

  WaveData* wave_data = NULL;
  if (m_DataMap.contains(wave_id)) {
    wave_data = m_DataMap[wave_id];
    data_point = wave_data->m_DataBufferPointer;
  }
  return data_point;
}

void TestDataManager::saveCsvFile(QString file_path) {
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

  QString header = QString(
      "Time(s), Time_10ms, CommPwm, RecvPwm, Speed(rpm), Current, BusCurrent, "
      "V Modulation, Voltage, MosTemp, CapTemp, McuTemp, "
      "MotorTemp,SelfCheckError, RunningError, Speed(km_h)");

  write_stream << header << '\n';

  for (int i = 0; i < m_TimeData.size(); i++) {
    QString data_info = "";
    data_info += QString::number(m_TimeData[i], 'f', 3);
    data_info += "," + QString::number(m_Time10msData[i], 'f', 0);
    data_info += "," + QString::number(m_CommPWMData[i], 'f', 1);
    data_info += "," + QString::number(m_RecvPWMData[i], 'f', 1);
    data_info += "," + QString::number(m_SpeedData[i], 'f', 1);
    data_info += "," + QString::number(m_CurrentData[i], 'f', 1);
    data_info += "," + QString::number(m_BusCurrentData[i], 'f', 1);
    data_info += "," + QString::number(m_VModulationData[i], 'f', 1);
    data_info += "," + QString::number(m_VoltageData[i], 'f', 1);
    data_info += "," + QString::number(m_MosTempData[i], 'f', 1);
    data_info += "," + QString::number(m_CapTempData[i], 'f', 1);
    data_info += "," + QString::number(m_McuTempData[i], 'f', 1);
    data_info += "," + QString::number(m_MotorTempData[i], 'f', 1);

    data_info += "," + QString::number(m_SelfcheckErrorData[i], 'f', 0);
    data_info += "," + QString::number(m_RunningErrorData[i], 'f', 0);
    data_info += "," + QString::number(m_SpeedKmHData[i], 'f', 1);

    write_stream << data_info << '\n';
  }

  file.close();
}

void TestDataManager::saveTestSummaryCsvFile(QString file_path) {
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

  QString header = QString("SN, TestTime");

  write_stream << header << '\n';

  QString dir_path = Config::singleton()->getTestDataPath();
  QVector<QString> test_summay;

  scanTestSummaryInfo(dir_path, test_summay);

  for (int i = 0; i < test_summay.size(); i++) {
    write_stream << test_summay[i] << '\n';
  }
}

void TestDataManager::saveWaveTitle(QFile& file_handle, QString wave_title,
                                    int wave_offset, int wave_size) {
  CustomByteArray info_array;
  QByteArray wave_title_array = wave_title.toLatin1();

  // 前24个字节是信息,后40个字节是字段名称
  info_array.clear();
  info_array.appendUint16(0x0100);
  info_array.appendInt32(wave_offset);
  info_array.appendInt32(wave_size);
  info_array.appendUint16(0);
  info_array.appendUint16(0);
  info_array.appendUint16(0);
  info_array.appendUint16(0);
  info_array.appendUint16(0);
  info_array.appendUint16(0);
  info_array.appendUint16(0);

  for (int i = 0; i < 40; i++) {
    if (i < wave_title_array.size()) {
      info_array.appendUint8(wave_title_array[i]);
    } else {
      info_array.appendUint8(0);
    }
  }

  file_handle.write(info_array);
}

void TestDataManager::saveWaveTime(QFile& file_handle,
                                   QSharedPointer<QCPGraphDataContainer> data,
                                   double end_time) {
  QDataStream data_stream(&file_handle);
  data_stream.setByteOrder(QDataStream::LittleEndian);

  QCPDataContainer<QCPGraphData>::iterator iter;
  for (iter = data->begin(); iter != data->end(); iter++) {
    double key = iter->mainKey();
    if (key < end_time) {
      data_stream << key;
    } else {
      break;
    }
  }
}

void TestDataManager::saveWaveData(QFile& file_handle,
                                   QSharedPointer<QCPGraphDataContainer> data,
                                   double end_time, bool last_time) {
  QDataStream data_stream(&file_handle);
  data_stream.setByteOrder(QDataStream::LittleEndian);

  QCPDataContainer<QCPGraphData>::iterator iter;
  for (iter = data->begin(); iter != data->end(); iter++) {
    double time_s = iter->mainKey();
    double value = iter->mainValue();

    if (time_s < end_time) {
      data_stream << value;
    } else {
      break;
    }
  }

  if (last_time == false) {
    // 移除数据
    data->removeBefore(end_time);
  }
}

void TestDataManager::scanTestDataDesc(QString dir_path,
                                       QVector<TestDataDesc>& test_data_desc) {
  QStringList dir_path_split = dir_path.split("/");
  QString date_name_str = dir_path_split.constLast();

  QDir date_dir(dir_path);
  QFileInfoList test_data_list =
      date_dir.entryInfoList(date_dir.filter() | QDir::NoDotAndDotDot,
                             date_dir.sorting() | QDir::Time);
  for (int j = 0; j < test_data_list.size(); j++) {
    QString sn_code_str = test_data_list[j].fileName();
    QString item_path = dir_path + "/" + sn_code_str;

    QDir item_dir(item_path);
    QStringList name_filters;
    name_filters << "*.json";
    name_filters << "*.dat";

    QFileInfoList item_list = item_dir.entryInfoList(
        name_filters, item_dir.filter() | QDir::NoDotAndDotDot,
        item_dir.sorting() | QDir::Time);

    for (int z = 0; z < item_list.size(); z++) {
      QString base_name = item_list[z].baseName();
      QString file_suffix = item_list[z].suffix();

      TestDataDesc item_desc;
      item_desc.date = date_name_str;
      item_desc.time = base_name;
      item_desc.sn_code = sn_code_str;

      if (file_suffix == "json") {
        item_desc.desc_path = item_path + "/" + base_name + ".json";
        item_desc.data_path = item_path + "/" + base_name + ".bin";
      } else {
        item_desc.desc_path = item_path + "/" + base_name + ".dat";
        item_desc.data_path = item_path + "/" + base_name + ".dat";
      }

      test_data_desc.append(item_desc);
    }
  }
}

void TestDataManager::scanTestSummaryInfo(QString dir_path,
                                          QVector<QString>& test_summary) {
  QDir root_dir(dir_path);

  QFileInfoList date_list =
      root_dir.entryInfoList(root_dir.filter() | QDir::NoDotAndDotDot,
                             root_dir.sorting() | QDir::Time);

  for (int i = 0; i < date_list.size(); i++) {
    QString date_str = date_list[i].fileName();
    QString date_path = dir_path + "/" + date_str;

    date_str.replace('-', '_');
    QDir date_dir(date_path);
    QFileInfoList test_data_list =
        date_dir.entryInfoList(date_dir.filter() | QDir::NoDotAndDotDot,
                               date_dir.sorting() | QDir::Time);
    for (int j = 0; j < test_data_list.size(); j++) {
      QString sn_code_str = test_data_list[j].fileName();

      QString summay_info = sn_code_str + "," + date_str;
      test_summary.append(summay_info);
    }
  }
}

void TestDataManager::parseDataFile(QString name, int offset, int size,
                                    QFile& file_handle) {
  int data_count = size / sizeof(double);

  file_handle.seek(offset);
  QDataStream data_stream(&file_handle);
  data_stream.setByteOrder(QDataStream::LittleEndian);

  if (name == "time") {
    parseWaveData(m_TimeData, data_stream, data_count);
  } else if (name == "time_10ms") {
    parseWaveData(m_Time10msData, data_stream, data_count);
  } else if (name == "recv_pwm") {
    parseWaveData(m_RecvPWMData, data_stream, data_count);
  } else if (name == "comm_pwm") {
    parseWaveData(m_CommPWMData, data_stream, data_count);
  } else if (name == "speed") {
    parseWaveData(m_SpeedData, data_stream, data_count);
  } else if (name == "current") {
    parseWaveData(m_CurrentData, data_stream, data_count);
  } else if (name == "bus_current") {
    parseWaveData(m_BusCurrentData, data_stream, data_count);
  } else if (name == "voltage") {
    parseWaveData(m_VoltageData, data_stream, data_count);
  } else if (name == "v_modulation") {
    parseWaveData(m_VModulationData, data_stream, data_count);
  } else if (name == "mos_temp") {
    parseWaveData(m_MosTempData, data_stream, data_count);
  } else if (name == "cap_temp") {
    parseWaveData(m_CapTempData, data_stream, data_count);
  } else if (name == "mcu_temp") {
    parseWaveData(m_McuTempData, data_stream, data_count);
  } else if (name == "motor_temp") {
    parseWaveData(m_MotorTempData, data_stream, data_count);
  } else if (name == "running_error") {
    parseWaveData(m_RunningErrorData, data_stream, data_count);
  } else if (name == "selfcheck_error") {
    parseWaveData(m_SelfcheckErrorData, data_stream, data_count);
  } else if (name == "speed_km_h") {
    parseWaveData(m_SpeedKmHData, data_stream, data_count);
  }
}

void TestDataManager::parseWaveData(QVector<double>& data,
                                    QDataStream& data_stream, int size) {
  data.clear();
  for (int i = 0; i < size; i++) {
    double value;
    data_stream >> value;
    data.append(value);
  }
}

void TestDataManager::initWave() {
  addWave("time_10ms");

  addWave("recv_pwm");
  addWave("comm_pwm");

  addWave("speed");

  addWave("current");
  addWave("bus_current");

  addWave("voltage");
  addWave("v_modulation");

  addWave("mos_temp");
  addWave("cap_temp");
  addWave("mcu_temp");
  addWave("motor_temp");

  addWave("running_error");
  addWave("selfcheck_error");
  addWave("speed_km_h");
}

void TestDataManager::addWave(QString line_name) {
  if (!m_DataMap.contains(line_name)) {
    WaveData* data = new WaveData();
    m_DataMap[line_name] = data;
  }
}

void TestDataManager::syncWave() {
  QMap<QString, WaveData*>::iterator iter;

  for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++) {
    QString name = iter.key();
    WaveData* data = iter.value();
    data->clear();

    if (name == "time_10ms") {
      syncWaveData(m_TimeData, m_Time10msData, data);
    } else if (name == "recv_pwm") {
      syncWaveData(m_TimeData, m_RecvPWMData, data);
    } else if (name == "comm_pwm") {
      syncWaveData(m_TimeData, m_CommPWMData, data);
    } else if (name == "speed") {
      syncWaveData(m_TimeData, m_SpeedData, data);
    } else if (name == "current") {
      syncWaveData(m_TimeData, m_CurrentData, data);
    } else if (name == "bus_current") {
      syncWaveData(m_TimeData, m_BusCurrentData, data);
    } else if (name == "voltage") {
      syncWaveData(m_TimeData, m_VoltageData, data);
    } else if (name == "v_modulation") {
      syncWaveData(m_TimeData, m_VModulationData, data);
    } else if (name == "mos_temp") {
      syncWaveData(m_TimeData, m_MosTempData, data);
    } else if (name == "cap_temp") {
      syncWaveData(m_TimeData, m_CapTempData, data);
    } else if (name == "mcu_temp") {
      syncWaveData(m_TimeData, m_McuTempData, data);
    } else if (name == "motor_temp") {
      syncWaveData(m_TimeData, m_MotorTempData, data);
    } else if (name == "running_error") {
      syncWaveData(m_TimeData, m_RunningErrorData, data);
    } else if (name == "selfcheck_error") {
      syncWaveData(m_TimeData, m_SelfcheckErrorData, data);
    } else if (name == "speed_km_h") {
      syncWaveData(m_TimeData, m_SpeedKmHData, data);
    }
  }
}

void TestDataManager::syncWaveData(QVector<double>& time_x,
                                   QVector<double>& value_y,
                                   WaveData* wave_data) {
  if (value_y.size() != time_x.size()) {
    value_y.fill(-1, time_x.size());
  }

  for (int i = 0; i < time_x.size(); i++) {
    QCPGraphData point(time_x[i], value_y[i]);
    wave_data->appendPoint(point);
  }
}
