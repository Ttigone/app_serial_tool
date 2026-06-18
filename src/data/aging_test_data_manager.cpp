#include "AgingTestDataManager.h"
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

#include "Comm/CommManager.h"

AgingTestDataDesc::AgingTestDataDesc() {
  date = "";
  time = "";
  sn_code = "";

  data_path = "";
}

QObject* AgingTestDataManager::qmlSingleton(QQmlEngine* engine,
                                            QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static AgingTestDataManager* _singleton = nullptr;
  if (!_singleton) _singleton = new AgingTestDataManager;
  return _singleton;
}

AgingTestDataManager* AgingTestDataManager::singleton() {
  return qobject_cast<AgingTestDataManager*>(qmlSingleton(nullptr, nullptr));
}

AgingTestDataManager::AgingTestDataManager() {
  initWave();
  m_DataSaveFlag = false;
}

AgingTestDataManager::~AgingTestDataManager() {}

void AgingTestDataManager::setDataSaveFlag(bool flag) {
  m_DataSaveFlag = flag;
  m_DataSaveIndex = 0;
  m_BufferTimeLength =
      60 * Config::singleton()->getStorageDuration();  // 缓存10min
}

void AgingTestDataManager::saveData(quint8 esc_index, QString esc_sn,
                                    QString log_time, bool last_time) {
  if (m_DataSaveFlag) {
    return;
  }

  QSharedPointer<QCPGraphDataContainer> data =
      RealTimeDataManager::singleton()->getAgingTestData(
          QString("ESC%1#recv_pwm").arg(esc_index));

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

  QDir folder;

  QString dir = Config::singleton()->getAgingTestDataPath();

  QDateTime current_date_time = QDateTime::currentDateTime();

  QString current_date = current_date_time.toString("yyyyMMdd");

  dir += "/" + current_date;

  // 创建文件夹
  if (!folder.exists(dir)) {
    folder.mkpath(dir);
  }

  QString wave_file_name = QString("%1_%2_ESC%3_%4.aging_dat")
                               .arg(current_date)
                               .arg(log_time)
                               .arg(m_DataSaveIndex)
                               .arg(esc_sn);
  QString wave_file_path = QString("%1/%2").arg(dir).arg(wave_file_name);

  QFile wave_file(wave_file_path);
  if (!wave_file.open(QIODevice::ReadWrite)) {
    return;
  }

  m_DataSaveIndex++;

  int offset = 64 * 128;

  saveWaveTitle(wave_file, "time", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "ADC1", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "ADC2", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "recv_pwm", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "comm_pwm", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "phase_voltage", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "current_res", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "current_acs", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "bus_current", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "voltage", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "v_modulation", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "mos_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "mos2_temp", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "cap_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "mcu_temp", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "running_error", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "selfcheck_error", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "selfcheck_value", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "selfcheck_phase", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "motor_temp", offset, data_size);
  offset += data_size;

  saveWaveTitle(wave_file, "time_10ms", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "encoder_angle", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "encoder_temp", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "v5_V", offset, data_size);
  offset += data_size;
  saveWaveTitle(wave_file, "v15_V", offset, data_size);

  wave_file.seek(64 * 128);
  // 保存时间轴x
  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#ADC1").arg(esc_index));
  saveWaveTime(wave_file, data, end_save_time);

  // 保存数据
  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#ADC1").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#ADC2").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#recv_pwm").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#comm_pwm").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#phase_voltage").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#current_res").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#current_acs").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#bus_current").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#voltage").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#v_modulation").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#mos_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#mos2_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#cap_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#mcu_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#running_error").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#selfcheck_error").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#selfcheck_value").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#selfcheck_phase").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#motor_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#time_10ms").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#encoder_angle").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#encoder_temp").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#v5_V").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  data = RealTimeDataManager::singleton()->getAgingTestData(
      QString("ESC%1#v15_V").arg(esc_index));
  saveWaveData(wave_file, data, end_save_time, last_time);

  wave_file.close();
}

void AgingTestDataManager::clearHistoryData() {
  QString dir_path = Config::singleton()->getTestDataPath();

  QDir dir(dir_path);

  dir.removeRecursively();
}

QVariantMap AgingTestDataManager::getDirTestDataDesc(QString dir_path) {
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

  QVector<AgingTestDataDesc> test_data_desc;

  scanTestDataDesc(dir_path, test_data_desc);

  QVariantMap test_data_map;

  for (int i = 0; i < test_data_desc.size(); i++) {
    AgingTestDataDesc item_desc = test_data_desc[i];

    QVariantMap item_map;
    item_map.insert("time", item_desc.date + " " + item_desc.time);
    item_map.insert("sn_code", item_desc.sn_code);
    item_map.insert("data_path", item_desc.data_path);

    test_data_map.insert(QString::number(i), item_map);
  }

  return test_data_map;
}

void AgingTestDataManager::parseData(QString data_path) {
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

QSharedPointer<QCPGraphDataContainer> AgingTestDataManager::getWaveData(
    QString wave_id) {
  QSharedPointer<QCPGraphDataContainer> data_point = NULL;

  WaveData* wave_data = NULL;
  if (m_DataMap.contains(wave_id)) {
    wave_data = m_DataMap[wave_id];
    data_point = wave_data->m_DataBufferPointer;
  }
  return data_point;
}

void AgingTestDataManager::saveCsvFile(QString file_path) {
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
      "Time(s), ADC1, ADC2, recv_pwm, comm_pwm, phase_voltage, current_res,  "
      "current_acs, bus_current, voltage, v_modulation, mos_temp, mos2_temp, "
      "cap_temp, mcu_temp, running_error,selfcheck_error, selfcheck_value, "
      "selfcheck_phase, motor_temp, time_10ms, encoder_angle, encoder_temp, "
      "v5_V, v15_V");

  write_stream << header << '\n';

  for (int i = 0; i < m_TimeData.size(); i++) {
    QString data_info = "";
    data_info += QString::number(m_TimeData[i], 'f', 3);
    data_info += "," + QString::number(m_ADC1Data[i], 'f', 1);
    data_info += "," + QString::number(m_ADC2Data[i], 'f', 1);

    data_info += "," + QString::number(m_RecvPWMData[i], 'f', 1);
    data_info += "," + QString::number(m_CommPWMData[i], 'f', 1);

    data_info += "," + QString::number(m_PhaseVoltageData[i], 'f', 1);

    data_info += "," + QString::number(m_CurrentResData[i], 'f', 1);
    data_info += "," + QString::number(m_CurrentAcsData[i], 'f', 1);
    data_info += "," + QString::number(m_BusCurrentData[i], 'f', 1);

    data_info += "," + QString::number(m_VoltageData[i], 'f', 1);
    data_info += "," + QString::number(m_VModulationData[i], 'f', 1);

    data_info += "," + QString::number(m_MosTempData[i], 'f', 1);
    data_info += "," + QString::number(m_Mos2TempData[i], 'f', 1);

    data_info += "," + QString::number(m_CapTempData[i], 'f', 1);
    data_info += "," + QString::number(m_McuTempData[i], 'f', 1);

    data_info += "," + QString::number(m_RunningErrorData[i], 'f', 1);

    data_info += "," + QString::number(m_SelfcheckErrorData[i], 'f', 1);
    data_info += "," + QString::number(m_SelfcheckValueData[i], 'f', 1);
    data_info += "," + QString::number(m_SelfcheckPhaseData[i], 'f', 1);

    data_info += "," + QString::number(m_MotorTempData[i], 'f', 1);

    data_info += "," + QString::number(m_Time10msData[i], 'f', 1);
    data_info += "," + QString::number(m_EncoderAngleData[i], 'f', 1);
    data_info += "," + QString::number(m_EncoderTempData[i], 'f', 1);
    data_info += "," + QString::number(m_V5VData[i], 'f', 1);
    data_info += "," + QString::number(m_V15VData[i], 'f', 1);

    write_stream << data_info << '\n';
  }

  file.close();
}

void AgingTestDataManager::saveWaveTitle(QFile& file_handle, QString wave_title,
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

void AgingTestDataManager::saveWaveTime(
    QFile& file_handle, QSharedPointer<QCPGraphDataContainer> data,
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

void AgingTestDataManager::saveWaveData(
    QFile& file_handle, QSharedPointer<QCPGraphDataContainer> data,
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

void AgingTestDataManager::scanTestDataDesc(
    QString dir_path, QVector<AgingTestDataDesc>& test_data_desc) {
  QStringList dir_path_split = dir_path.split("/");

  QDir item_dir(dir_path);
  QStringList name_filters;
  name_filters << "*.aging_dat";

  QFileInfoList test_data_list = item_dir.entryInfoList(
      name_filters, item_dir.filter() | QDir::NoDotAndDotDot,
      item_dir.sorting() | QDir::Time);
  for (int j = 0; j < test_data_list.size(); j++) {
    QString base_name = test_data_list[j].baseName();

    QStringList item_info = base_name.split("_");

    QString data_path = test_data_list[j].absoluteFilePath();
    AgingTestDataDesc item_desc;
    item_desc.date = item_info[0];
    item_desc.time = item_info[1];
    item_desc.sn_code = item_info[3];

    item_desc.data_path = data_path;

    test_data_desc.append(item_desc);
  }
}

void AgingTestDataManager::parseDataFile(QString name, int offset, int size,
                                         QFile& file_handle) {
  int data_count = size / sizeof(double);

  file_handle.seek(offset);
  QDataStream data_stream(&file_handle);
  data_stream.setByteOrder(QDataStream::LittleEndian);

  if (name == "time") {
    parseWaveData(m_TimeData, data_stream, data_count);
  } else if (name == "ADC1") {
    parseWaveData(m_ADC1Data, data_stream, data_count);
  } else if (name == "ADC2") {
    parseWaveData(m_ADC2Data, data_stream, data_count);
  } else if (name == "recv_pwm") {
    parseWaveData(m_RecvPWMData, data_stream, data_count);
  } else if (name == "comm_pwm") {
    parseWaveData(m_CommPWMData, data_stream, data_count);
  } else if (name == "phase_voltage") {
    parseWaveData(m_PhaseVoltageData, data_stream, data_count);
  } else if (name == "current_res") {
    parseWaveData(m_CurrentResData, data_stream, data_count);
  } else if (name == "current_acs") {
    parseWaveData(m_CurrentAcsData, data_stream, data_count);
  } else if (name == "bus_current") {
    parseWaveData(m_BusCurrentData, data_stream, data_count);
  } else if (name == "voltage") {
    parseWaveData(m_VoltageData, data_stream, data_count);
  } else if (name == "v_modulation") {
    parseWaveData(m_VModulationData, data_stream, data_count);
  } else if (name == "mos_temp") {
    parseWaveData(m_MosTempData, data_stream, data_count);
  } else if (name == "mos2_temp") {
    parseWaveData(m_Mos2TempData, data_stream, data_count);
  } else if (name == "cap_temp") {
    parseWaveData(m_CapTempData, data_stream, data_count);
  } else if (name == "mcu_temp") {
    parseWaveData(m_McuTempData, data_stream, data_count);
  } else if (name == "running_error") {
    parseWaveData(m_RunningErrorData, data_stream, data_count);
  } else if (name == "selfcheck_error") {
    parseWaveData(m_SelfcheckErrorData, data_stream, data_count);
  } else if (name == "selfcheck_value") {
    parseWaveData(m_SelfcheckValueData, data_stream, data_count);
  } else if (name == "selfcheck_phase") {
    parseWaveData(m_SelfcheckPhaseData, data_stream, data_count);
  } else if (name == "motor_temp") {
    parseWaveData(m_MotorTempData, data_stream, data_count);
  } else if (name == "time_10ms") {
    parseWaveData(m_Time10msData, data_stream, data_count);
  } else if (name == "encoder_angle") {
    parseWaveData(m_EncoderAngleData, data_stream, data_count);
  } else if (name == "encoder_temp") {
    parseWaveData(m_EncoderTempData, data_stream, data_count);
  } else if (name == "v5_V") {
    parseWaveData(m_V5VData, data_stream, data_count);
  } else if (name == "v15_V") {
    parseWaveData(m_V15VData, data_stream, data_count);
  }
}

void AgingTestDataManager::parseWaveData(QVector<double>& data,
                                         QDataStream& data_stream, int size) {
  data.clear();
  for (int i = 0; i < size; i++) {
    double value;
    data_stream >> value;
    data.append(value);
  }
}

void AgingTestDataManager::initWave() {
  addWave("ADC1");
  addWave("ADC2");

  addWave("recv_pwm");
  addWave("comm_pwm");

  addWave("phase_voltage");

  addWave("current_res");
  addWave("current_acs");
  addWave("bus_current");

  addWave("voltage");
  addWave("v_modulation");

  addWave("mos_temp");
  addWave("mos2_temp");

  addWave("cap_temp");
  addWave("mcu_temp");

  addWave("running_error");

  addWave("selfcheck_error");
  addWave("selfcheck_value");
  addWave("selfcheck_phase");

  addWave("motor_temp");

  addWave("time_10ms");
  addWave("encoder_angle");
  addWave("encoder_temp");

  addWave("v5_V");
  addWave("v15_V");
}

void AgingTestDataManager::addWave(QString line_name) {
  if (!m_DataMap.contains(line_name)) {
    WaveData* data = new WaveData();
    m_DataMap[line_name] = data;
  }
}

void AgingTestDataManager::syncWave() {
  QMap<QString, WaveData*>::iterator iter;

  for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++) {
    QString name = iter.key();
    WaveData* data = iter.value();
    data->clear();

    if (name == "ADC1") {
      syncWaveData(m_TimeData, m_ADC1Data, data);
    }
    if (name == "ADC2") {
      syncWaveData(m_TimeData, m_ADC2Data, data);
    } else if (name == "recv_pwm") {
      syncWaveData(m_TimeData, m_RecvPWMData, data);
    } else if (name == "comm_pwm") {
      syncWaveData(m_TimeData, m_CommPWMData, data);
    } else if (name == "phase_voltage") {
      syncWaveData(m_TimeData, m_PhaseVoltageData, data);
    } else if (name == "current_res") {
      syncWaveData(m_TimeData, m_CurrentResData, data);
    } else if (name == "current_acs") {
      syncWaveData(m_TimeData, m_CurrentAcsData, data);
    } else if (name == "bus_current") {
      syncWaveData(m_TimeData, m_BusCurrentData, data);
    } else if (name == "voltage") {
      syncWaveData(m_TimeData, m_VoltageData, data);
    } else if (name == "v_modulation") {
      syncWaveData(m_TimeData, m_VModulationData, data);
    } else if (name == "mos_temp") {
      syncWaveData(m_TimeData, m_MosTempData, data);
    } else if (name == "mos2_temp") {
      syncWaveData(m_TimeData, m_Mos2TempData, data);
    } else if (name == "cap_temp") {
      syncWaveData(m_TimeData, m_CapTempData, data);
    } else if (name == "mcu_temp") {
      syncWaveData(m_TimeData, m_McuTempData, data);
    } else if (name == "running_error") {
      syncWaveData(m_TimeData, m_RunningErrorData, data);
    } else if (name == "selfcheck_error") {
      syncWaveData(m_TimeData, m_SelfcheckErrorData, data);
    } else if (name == "selfcheck_value") {
      syncWaveData(m_TimeData, m_SelfcheckValueData, data);
    } else if (name == "selfcheck_phase") {
      syncWaveData(m_TimeData, m_SelfcheckPhaseData, data);
    } else if (name == "motor_temp") {
      syncWaveData(m_TimeData, m_MotorTempData, data);
    } else if (name == "time_10ms") {
      syncWaveData(m_TimeData, m_Time10msData, data);
    } else if (name == "encoder_angle") {
      syncWaveData(m_TimeData, m_EncoderAngleData, data);
    } else if (name == "encoder_temp") {
      syncWaveData(m_TimeData, m_EncoderTempData, data);
    } else if (name == "v5_V") {
      syncWaveData(m_TimeData, m_V5VData, data);
    } else if (name == "v15_V") {
      syncWaveData(m_TimeData, m_V15VData, data);
    }
  }
}

void AgingTestDataManager::syncWaveData(QVector<double>& time_x,
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

int AgingTestDataManager::initAgingTest() {
  CommManager::singleton()->clearDataPacketBuffer();

  if (m_AgingTestCasePoint.size() <= 0) {
    return 1;
  }

  m_AgingTestCount = 0;

  for (int i = 0; i < 4; i++) {
    m_ErrorTestCount[i] = 0;
    m_LastErrorTestPointIndex[i] = -1;
    m_AgingErrorCount[i] = 0;

    m_AgingErrorFlag[i] = 0;
    m_AgingRunError[i] = 0;
  }

  return 0;
}

int AgingTestDataManager::parseAgingTestCase(QString file_path) {
  m_AgingTestCasePoint.clear();

  if (file_path == "") {
    return 1;
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
    return 1;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return 1;
  }

  QTextStream in(&file);
  QStringList test_case_list;
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList line_item = line.split(',');
    if (line_item.size() != 32) {
      return 2;  // 格式不正确
    }
    test_case_list.append(line);
  }

  if (test_case_list.size() < 11) {
    return 2;
  }

  m_AgingTestCaseInfo.clear();

  // Suppoer ESC
  QString support_esc_str = test_case_list[0].split(',')[0];
  m_AgingEsc = support_esc_str.split(':')[1];
  m_AgingTestCaseInfo.append(support_esc_str);

  // Voltage
  QString voltage_v_str = test_case_list[1].split(',')[0];
  m_AgingTestCaseInfo.append(voltage_v_str);

  // PwmAccDecTime
  QString pwm_acc_dec_time_str = test_case_list[2].split(',')[0];
  m_AgingPwmAccDecTimeMs = pwm_acc_dec_time_str.split(':')[1].toInt();
  m_AgingTestCaseInfo.append(pwm_acc_dec_time_str);

  // CheckTimeMs
  QString check_time_str = test_case_list[3].split(',')[0];
  m_AgingCheckTimeMs = check_time_str.split(':')[1].toUInt();
  m_AgingTestCaseInfo.append(check_time_str);

  // MaxErrorCount
  QString max_error_count_str = test_case_list[4].split(',')[0];
  m_AgingMaxErrorCount = max_error_count_str.split(':')[1].toInt();
  m_AgingTestCaseInfo.append(max_error_count_str);

  for (int i = 11; i < test_case_list.size(); i++) {
    AgingTestCasePoint point_value;

    QStringList test_point_str = test_case_list[i].split(',');

    point_value.time = test_point_str[0].toFloat();
    point_value.send_pwm = test_point_str[1].toFloat();
    point_value.pwm_min = test_point_str[2].toFloat();
    point_value.pwm_max = test_point_str[3].toFloat();
    point_value.voltage_min = test_point_str[4].toFloat();
    point_value.voltage_max = test_point_str[5].toFloat();
    point_value.phase_voltage_min = test_point_str[6].toFloat();
    point_value.phase_voltage_max = test_point_str[7].toFloat();
    point_value.current_res_min = test_point_str[8].toFloat();
    point_value.current_res_max = test_point_str[9].toFloat();
    point_value.current_acs_min = test_point_str[10].toFloat();
    point_value.current_acs_max = test_point_str[11].toFloat();
    point_value.bus_current_min = test_point_str[12].toFloat();
    point_value.bus_current_max = test_point_str[13].toFloat();
    point_value.temp_mos_min = test_point_str[14].toFloat();
    point_value.temp_mos_max = test_point_str[15].toFloat();
    point_value.temp_cap_min = test_point_str[16].toFloat();
    point_value.temp_cap_max = test_point_str[17].toFloat();
    point_value.temp_mcu_min = test_point_str[18].toFloat();
    point_value.temp_mcu_max = test_point_str[19].toFloat();
    point_value.temp_encoder_min = test_point_str[20].toFloat();
    point_value.temp_encoder_max = test_point_str[21].toFloat();
    point_value.adc1_min = test_point_str[22].toFloat();
    point_value.adc1_max = test_point_str[23].toFloat();
    point_value.adc2_min = test_point_str[24].toFloat();
    point_value.adc2_max = test_point_str[25].toFloat();

    point_value.temp_motor_min = test_point_str[26].toFloat();
    point_value.temp_motor_max = test_point_str[27].toFloat();

    point_value.v5_min = test_point_str[28].toFloat();
    point_value.v5_max = test_point_str[29].toFloat();

    point_value.v15_min = test_point_str[30].toFloat();
    point_value.v15_max = test_point_str[31].toFloat();

    m_AgingTestCasePoint.append(point_value);
  }

  file.close();

  QFileInfo file_info(file_path);
  m_AgingtTestCaseFileName = file_info.fileName();
  return 0;
}

void AgingTestDataManager::judgeAgingTestData(
    int esc_id, int test_count, int point_index, quint32 test_point_time_ms,
    EscAgingTestDataParam& test_data) {
  if (test_point_time_ms < m_AgingCheckTimeMs) {
    return;
  }

  if (point_index >= m_AgingTestCasePoint.size()) {
    return;
  }

  quint8 esc_index = esc_id - 32;
  quint8 esc_display_index = esc_id - 31;

  m_AgingTestCount = test_count;

  AgingTestCasePoint test_point = m_AgingTestCasePoint[point_index];

  QString error_info = "";
  if (test_data.recv_pwm < test_point.pwm_min ||
      test_data.recv_pwm > test_point.pwm_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0001)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : recv_pwm "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.recv_pwm)
              .arg(test_point.pwm_min)
              .arg(test_point.pwm_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0001;
  }

  if (test_data.comm_pwm < test_point.pwm_min ||
      test_data.comm_pwm > test_point.pwm_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0002)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : comm_pwm "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.comm_pwm)
              .arg(test_point.pwm_min)
              .arg(test_point.pwm_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0002;
  }

  if (test_data.voltage < test_point.voltage_min ||
      test_data.voltage > test_point.voltage_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0004)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : voltage %3 "
                              "not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.voltage)
              .arg(test_point.voltage_min)
              .arg(test_point.voltage_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0004;
  }

  if (test_data.phase_voltage < test_point.phase_voltage_min ||
      test_data.phase_voltage > test_point.phase_voltage_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0008)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                              "phase_voltage %3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.phase_voltage)
              .arg(test_point.phase_voltage_min)
              .arg(test_point.phase_voltage_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0008;
  }

  if (test_data.current_res < test_point.current_res_min ||
      test_data.current_res > test_point.current_res_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0010)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                              "current_res %3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.current_res)
              .arg(test_point.current_res_min)
              .arg(test_point.current_res_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0010;
  }

  if (test_data.current_acs < test_point.current_acs_min ||
      test_data.current_acs > test_point.current_acs_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0020)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                              "current_acs %3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.current_acs)
              .arg(test_point.current_acs_min)
              .arg(test_point.current_acs_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0020;
  }

  if (test_data.bus_current < test_point.bus_current_min ||
      test_data.bus_current > test_point.bus_current_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0040)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                              "bus_current %3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.bus_current)
              .arg(test_point.bus_current_min)
              .arg(test_point.bus_current_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0040;
  }

  if (test_data.mos_temp < test_point.temp_mos_min ||
      test_data.mos_temp > test_point.temp_mos_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0080)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : mos_temp "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.mos_temp)
              .arg(test_point.temp_mos_min)
              .arg(test_point.temp_mos_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0080;
  }

  if (test_data.mos2_temp < test_point.temp_mos_min ||
      test_data.mos2_temp > test_point.temp_mos_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0100)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : mos2_temp "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.mos2_temp)
              .arg(test_point.temp_mos_min)
              .arg(test_point.temp_mos_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0100;
  }

  if (test_data.cap_temp < test_point.temp_cap_min ||
      test_data.cap_temp > test_point.temp_cap_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0100)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : cap_temp "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.cap_temp)
              .arg(test_point.temp_cap_min)
              .arg(test_point.temp_cap_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0100;
  }

  if (test_data.mcu_temp < test_point.temp_mcu_min ||
      test_data.mcu_temp > test_point.temp_mcu_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0200)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : mcu_temp "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.cap_temp)
              .arg(test_point.temp_mcu_min)
              .arg(test_point.temp_mcu_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0200;
  }

  if (test_data.encoder_temp < test_point.temp_encoder_min ||
      test_data.encoder_temp > test_point.temp_encoder_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0400)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                              "encoder_temp %3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.encoder_temp)
              .arg(test_point.temp_encoder_min)
              .arg(test_point.temp_encoder_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0400;
  }

  if (test_data.adc1 < test_point.adc1_min ||
      test_data.adc1 > test_point.adc1_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x0800)) {
      error_info = QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                                       "adc1 %3 not in range(%4, %5)</font>"))
                       .arg(esc_display_index)
                       .arg(point_index)
                       .arg(test_data.adc1)
                       .arg(test_point.adc1_min)
                       .arg(test_point.adc1_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x0800;
  }

  if (test_data.adc2 < test_point.adc2_min ||
      test_data.adc2 > test_point.adc2_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x1000)) {
      error_info = QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                                       "adc2 %3 not in range(%4, %5)</font>"))
                       .arg(esc_display_index)
                       .arg(point_index)
                       .arg(test_data.adc1)
                       .arg(test_point.adc1_min)
                       .arg(test_point.adc1_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x1000;
  }

  if (test_data.motor_temp < test_point.temp_motor_min ||
      test_data.motor_temp > test_point.temp_motor_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x2000)) {
      error_info =
          QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : motor_temp "
                              "%3 not in range(%4, %5)</font>"))
              .arg(esc_display_index)
              .arg(point_index)
              .arg(test_data.motor_temp)
              .arg(test_point.temp_motor_min)
              .arg(test_point.temp_motor_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x2000;
  }

  if (test_data.v5_V < test_point.v5_min ||
      test_data.v5_V > test_point.v5_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x4000)) {
      error_info = QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                                       "v5_V %3 not in range(%4, %5)</font>"))
                       .arg(esc_display_index)
                       .arg(point_index)
                       .arg(test_data.v5_V)
                       .arg(test_point.v5_min)
                       .arg(test_point.v5_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x4000;
  }

  if (test_data.v15_V < test_point.v15_min ||
      test_data.v15_V > test_point.v15_max) {
    if (!(m_AgingErrorFlag[esc_index] & 0x8000)) {
      error_info = QString(QObject::tr("<font color=#e81123>ESC%1 Point %2 : "
                                       "v15_V %3 not in range(%4, %5)</font>"))
                       .arg(esc_display_index)
                       .arg(point_index)
                       .arg(test_data.v15_V)
                       .arg(test_point.v15_min)
                       .arg(test_point.v15_max);
      emit agingTestMessage(error_info);
    }

    m_AgingErrorFlag[esc_index] |= 0x8000;
  }

  // 测试计数变之后,需要从新计数
  if (m_ErrorTestCount[esc_index] != test_count) {
    m_LastErrorTestPointIndex[esc_index] = -1;
    m_ErrorTestCount[esc_index] = test_count;
  }

  // 故障次数计数
  if (m_AgingErrorFlag[esc_index] != 0) {
    // 首次检测到计数
    if (m_LastErrorTestPointIndex[esc_index] != point_index) {
      m_LastErrorTestPointIndex[esc_index] = point_index;

      m_AgingErrorCount[esc_index] = m_AgingErrorCount[esc_index] + 1;  // 计数+1
      m_AgingErrorFlag[esc_index] = 0;  // 重新置位
    }
  }

  // 过流置位
  if (test_data.running_error & 0x0001) {
    m_AgingRunError[esc_index] |= 0x01;
  }

  if (m_AgingErrorCount[esc_index] >= m_AgingMaxErrorCount) {
    m_AgingRunError[esc_index] |= 0x02;
  }

  if (m_AgingRunError[esc_index] != 0) {
    if ((m_AgingRunError[esc_index] & 0x80) == 0) {
      emit agingTestRunError(esc_index);

      m_AgingRunError[esc_index] |= 0x80;
    }
  }
}

QVector<AgingTestCasePoint> AgingTestDataManager::getAgingTestPoint() {
  return m_AgingTestCasePoint;
}

QString AgingTestDataManager::getAgingTestESCName() { return m_AgingEsc; }

QStringList AgingTestDataManager::getAgingTestCaseInfo() {
  return m_AgingTestCaseInfo;
}

int AgingTestDataManager::getAgingPwmAccDecTimeMs() {
  return m_AgingPwmAccDecTimeMs;
}

int AgingTestDataManager::getAgingTestCount() { return m_AgingTestCount; }
