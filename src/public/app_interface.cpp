#include "AppInterface.h"
#include "Comm/Command.h"

QObject* AppInterface::qmlSingleton(QQmlEngine* engine,
                                    QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static AppInterface* _singleton = nullptr;
  if (!_singleton) _singleton = new AppInterface;
  return _singleton;
}

AppInterface* AppInterface::singleton() {
  return qobject_cast<AppInterface*>(qmlSingleton(nullptr, nullptr));
}

AppInterface::AppInterface(QObject* parent) : QObject(parent) {
#ifdef HAS_POSITION

  m_GpsPosSource = QGeoPositionInfoSource::createDefaultSource(this);

  if (m_GpsPosSource) {
    connect(m_GpsPosSource, &QGeoPositionInfoSource::positionUpdated, this,
            &AppInterface::gpsSpeedUpdated);
    m_GpsPosSource->setUpdateInterval(20);
    m_GpsPosSource->startUpdates();
  }
#endif

  m_Speed_km_h = 0;
}

AppInterface::~AppInterface() {}

void AppInterface::emitMessageDialog(const QString& title, const QString& msg,
                                     bool richText) {
  emit messageDialog(title, msg, richText);
}

double AppInterface::getSpeedKmH() { return m_Speed_km_h; }

#ifdef HAS_POSITION
void AppInterface::gpsSpeedUpdated(const QGeoPositionInfo& info) {
  // 地面速度
  if (info.isValid() && info.hasAttribute(QGeoPositionInfo::GroundSpeed)) {
    m_Speed_km_h = info.attribute(QGeoPositionInfo::GroundSpeed) * 3.6;
  }
}
#endif
