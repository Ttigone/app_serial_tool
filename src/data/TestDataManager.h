#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include <QFile>
#include "ParamManager.h"
#include "WaveData.h"

class QQmlEngine;
class QJSEngine;

class TestDataDesc {
 public:
  TestDataDesc();

  QString date;
  QString time;
  QString sn_code;

  QString desc_path;
  QString data_path;
};

class TestDataManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(TestDataManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static TestDataManager* singleton();

  TestDataManager();
  ~TestDataManager();

  Q_INVOKABLE void setDataSaveFlag(bool flag);

  Q_INVOKABLE void saveData(QString esc_sn, QString log_time, bool last_time);
  Q_INVOKABLE void clearHistoryData();

  Q_INVOKABLE QVariantMap getTestDataDesc();

  Q_INVOKABLE QString getLastDirPath();

  Q_INVOKABLE QVariantMap getDirTestDataDesc(QString dir_path);

  Q_INVOKABLE void parseData(QString desc_path, QString data_path);

  Q_INVOKABLE QSharedPointer<QCPGraphDataContainer> getWaveData(
      QString wave_id);

  Q_INVOKABLE void saveCsvFile(QString file_path);
  Q_INVOKABLE void saveTestSummaryCsvFile(QString file_path);

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
                        QVector<TestDataDesc>& test_data_desc);

  void scanTestSummaryInfo(QString dir_path, QVector<QString>& test_summary);

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

  QVector<double> m_Time10msData;

  QVector<double> m_CommPWMData;
  QVector<double> m_RecvPWMData;
  QVector<double> m_SpeedData;
  QVector<double> m_BusCurrentData;
  QVector<double> m_CurrentData;
  QVector<double> m_VModulationData;
  QVector<double> m_VoltageData;
  QVector<double> m_MosTempData;
  QVector<double> m_CapTempData;
  QVector<double> m_McuTempData;
  QVector<double> m_MotorTempData;
  QVector<double> m_RunningErrorData;
  QVector<double> m_SelfcheckErrorData;

  QVector<double> m_SpeedKmHData;
};
