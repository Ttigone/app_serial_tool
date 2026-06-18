#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include <QFile>
#include "ParamManager.h"
#include "WaveData.h"

class QQmlEngine;
class QJSEngine;

typedef enum {
  FIELD_INT8,
  FIELD_UINT8,
  FIELD_INT16,
  FIELD_UINT16,
  FIELD_INT32,
  FIELD_UINT32,
} LogFieldType;

class LogDataField {
 public:
  LogDataField();

  QString field_name;
  int field_type;
  int field_scale;
};

class LogDataManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(LogDataManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static LogDataManager* singleton();

  LogDataManager();
  ~LogDataManager();

  Q_INVOKABLE int parseLogFile(QString log_file);
  Q_INVOKABLE QSharedPointer<QCPGraphDataContainer> getWaveData(
      QString wave_id);

  Q_INVOKABLE QVariantMap getFieldList();

  Q_INVOKABLE void saveCsvFile(QString file_path);

 private:
  int parseLogHeader(QFile& file_handle);

  int parseLogData(QFile& file_handle);

  void initWave();
  void addWave(QString data_id);

  QMap<QString, WaveData*> m_DataMap;

  QVector<LogDataField> m_LogDataField;
  QVariantMap m_LogDataFiledList;
  int m_LogFiledSize;

  quint16 m_LogType;  // 0， esc, 1, sample
  QString m_LogVersion;
};
