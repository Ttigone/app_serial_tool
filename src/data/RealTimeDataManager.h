#pragma once

#include <QMap>
#include <QString>
#include "WaveData.h"

class QQmlEngine;
class QJSEngine;

#include <QObject>

class RealTimeDataManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(RealTimeDataManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static RealTimeDataManager* singleton();

  RealTimeDataManager();
  ~RealTimeDataManager();

  Q_INVOKABLE void initRealTimeWave();

  Q_INVOKABLE void resetWave();

  Q_INVOKABLE void addWave(QString line_name);
  Q_INVOKABLE void clearData();

  Q_INVOKABLE QSharedPointer<QCPGraphDataContainer> getData(QString wave_id);

  Q_INVOKABLE void initAgingTestWave();
  Q_INVOKABLE void resetAgingTestWave();

  Q_INVOKABLE void addAgingTestWave(QString line_name);
  Q_INVOKABLE void clearAgingTestData();

  Q_INVOKABLE QSharedPointer<QCPGraphDataContainer> getAgingTestData(
      QString wave_id);

  //    Q_INVOKABLE void updateData(QString line_name, QAbstractSeries *series);
  //    Q_INVOKABLE void rescaleData(QString line_name, QAbstractSeries
  //    *series);

  Q_INVOKABLE void addDataPoint();
  Q_INVOKABLE void addEncoderPoint();

  Q_INVOKABLE void addAgingTestPoint(quint8 esc_id, quint16 test_count,
                                     quint8 test_point_index,
                                     quint32 test_point_time_ms,
                                     EscAgingTestDataParam& test_data);

 public:
  qint64 m_TimeStart;
  qint64 m_AgingTestTimeStart;

  QMap<QString, WaveData*> m_DataMap;

  QMap<QString, WaveData*> m_AgingDataMap;
};
