#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include <QFile>
#include "ParamManager.h"
#include "WaveData.h"

class QQmlEngine;
class QJSEngine;

struct AgingTestCasePoint {
 public:
  float time;
  float send_pwm;

  float pwm_min;
  float pwm_max;

  float voltage_min;
  float voltage_max;

  float phase_voltage_min;
  float phase_voltage_max;

  float current_res_min;
  float current_res_max;

  float current_acs_min;
  float current_acs_max;

  float bus_current_min;
  float bus_current_max;

  float temp_mos_min;
  float temp_mos_max;

  float temp_cap_min;
  float temp_cap_max;

  float temp_mcu_min;
  float temp_mcu_max;

  float temp_encoder_min;
  float temp_encoder_max;

  float adc1_min;
  float adc1_max;

  float adc2_min;
  float adc2_max;

  float temp_motor_min;
  float temp_motor_max;

  float v5_min;
  float v5_max;

  float v15_min;
  float v15_max;
};

class AgingTestDataDesc {
 public:
  AgingTestDataDesc();

  QString date;
  QString time;
  QString sn_code;
  QString data_path;
};

class AgingTestDataManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(AgingTestDataManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static AgingTestDataManager* singleton();

  AgingTestDataManager();
  ~AgingTestDataManager();

  Q_INVOKABLE void setDataSaveFlag(bool flag);

  Q_INVOKABLE void saveData(quint8 esc_id, QString esc_sn, QString log_time,
                            bool last_time);

  Q_INVOKABLE void clearHistoryData();

  Q_INVOKABLE QVariantMap getDirTestDataDesc(QString dir_path);

  Q_INVOKABLE void parseData(QString data_path);

  Q_INVOKABLE QSharedPointer<QCPGraphDataContainer> getWaveData(
      QString wave_id);

  Q_INVOKABLE void saveCsvFile(QString file_path);

  Q_INVOKABLE int initAgingTest();

  Q_INVOKABLE int parseAgingTestCase(QString file_path);

  Q_INVOKABLE void judgeAgingTestData(int esc_id, int test_count,
                                      int point_index,
                                      quint32 test_point_time_ms,
                                      EscAgingTestDataParam& test_data);

  Q_INVOKABLE QVector<AgingTestCasePoint> getAgingTestPoint();

  Q_INVOKABLE QString getAgingTestESCName();

  Q_INVOKABLE QStringList getAgingTestCaseInfo();

  Q_INVOKABLE int getAgingPwmAccDecTimeMs();

  Q_INVOKABLE int getAgingTestCount();

 signals:
  void agingTestMessage(QString message);
  void agingTestRunError(int esc_index);

 private:
  void saveWaveTitle(QFile& file_handle, QString wave_title, int wave_offset,
                     int wave_size);
  void saveWaveTime(QFile& file_handle,
                    QSharedPointer<QCPGraphDataContainer> data,
                    double end_time);
  void saveWaveData(QFile& file_handle,
                    QSharedPointer<QCPGraphDataContainer> data, double end_time,
                    bool last_time);

  void scanTestDataDesc(QString dir_path,
                        QVector<AgingTestDataDesc>& test_data_desc);

  void parseDataFile(QString name, int offset, int size, QFile& file_handle);

  void parseWaveData(QVector<double>& data, QDataStream& data_stream, int size);

  void initWave();
  void addWave(QString line_name);
  void syncWave();

  void syncWaveData(QVector<double>& time_x, QVector<double>& value_y,
                    WaveData* wave_data);

  bool m_DataSaveFlag;
  int m_DataSaveIndex;
  float m_BufferTimeLength;  // 存储时长

  QMap<QString, WaveData*> m_DataMap;

  QVector<double> m_TimeData;

  QVector<double> m_ADC1Data;
  QVector<double> m_ADC2Data;

  QVector<double> m_RecvPWMData;
  QVector<double> m_CommPWMData;

  QVector<double> m_PhaseVoltageData;

  QVector<double> m_CurrentResData;
  QVector<double> m_CurrentAcsData;
  QVector<double> m_BusCurrentData;

  QVector<double> m_VoltageData;
  QVector<double> m_VModulationData;

  QVector<double> m_MosTempData;
  QVector<double> m_Mos2TempData;

  QVector<double> m_CapTempData;
  QVector<double> m_McuTempData;

  QVector<double> m_RunningErrorData;
  QVector<double> m_SelfcheckErrorData;
  QVector<double> m_SelfcheckValueData;
  QVector<double> m_SelfcheckPhaseData;

  QVector<double> m_MotorTempData;

  QVector<double> m_Time10msData;
  QVector<double> m_EncoderAngleData;
  QVector<double> m_EncoderTempData;
  QVector<double> m_V5VData;
  QVector<double> m_V15VData;

  QStringList m_AgingTestCaseInfo;
  QString m_AgingEsc;
  int m_AgingPwmAccDecTimeMs;
  quint32 m_AgingCheckTimeMs;

  QVector<AgingTestCasePoint> m_AgingTestCasePoint;
  QString m_AgingtTestCaseFileName;

  int m_AgingMaxErrorCount;

  int m_AgingTestCount;

  int m_ErrorTestCount[4];
  int m_LastErrorTestPointIndex[4];
  int m_AgingErrorCount[4];

  quint32 m_AgingErrorFlag[4];
  quint32 m_AgingRunError[4];
};
