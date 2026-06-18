#pragma once

#include <QObject>

#include "Comm/CommManager.h"

#ifdef HAS_POSITION
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#endif

class QQmlEngine;
class QJSEngine;

class AppInterface : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(AppInterface)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static AppInterface* singleton();

  explicit AppInterface(QObject* parent = nullptr);
  ~AppInterface();

  Q_INVOKABLE void emitMessageDialog(const QString& title, const QString& msg,
                                     bool richText = false);

  Q_INVOKABLE double getSpeedKmH();

 signals:
  void messageDialog(const QString& title, const QString& msg, bool richText);

 private slots:

#ifdef HAS_POSITION
  void gpsSpeedUpdated(const QGeoPositionInfo& info);
#endif
 private:
#ifdef HAS_POSITION
  QGeoPositionInfoSource* m_GpsPosSource;
  QGeoPositionInfo m_GpsPos;

#endif

  double m_Speed_km_h;
};
