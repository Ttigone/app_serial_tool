#include "QmlCustomPlot.h"

#include "qcustomplot.h"
#include "CustomPlot.h"
#include <QDebug>
#include "Data/RealTimeDataManager.h"
#include "Data/TestDataManager.h"
#include "Data/LogDataManager.h"
#include "Data/AgingTestDataManager.h"

QmlCustomPlot::QmlCustomPlot(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_CustomPlot(nullptr)
{


  setFlag(QQuickItem::ItemHasContents, true);
  setAcceptedMouseButtons(Qt::AllButtons);

  connect(this, &QQuickPaintedItem::widthChanged, this,
          &QmlCustomPlot::updateCustomPlotSize);
  connect(this, &QQuickPaintedItem::heightChanged, this,
          &QmlCustomPlot::updateCustomPlotSize);
}

QmlCustomPlot::~QmlCustomPlot() {
  delete m_CustomPlot;
  m_CustomPlot = nullptr;
}

void QmlCustomPlot::initCustomPlot() {
    m_CustomPlot = new CustomPlot();

  //updateCustomPlotSize();
  connect(m_CustomPlot, &QCustomPlot::afterReplot, this,
          &QmlCustomPlot::onCustomReplot);

  m_TimeAxisLength = 30;
  m_EndTimeAxis = 0;

  m_CustomPlot->replot();

}

void QmlCustomPlot::setLegendVisiable(bool visible)
{
    m_CustomPlot->setLegendVisiable(visible);
}

void QmlCustomPlot::addRealWave(QString wave_id, QString wave_title)
{
    QSharedPointer<QCPGraphDataContainer> data;

    data = RealTimeDataManager::singleton()->getData(wave_id);

    m_CustomPlot->addWave(wave_id, wave_title);
    m_CustomPlot->setData(wave_id, data);
}

void QmlCustomPlot::addHistoryWave(QString wave_id, QString wave_title)
{
    QSharedPointer<QCPGraphDataContainer> data;

    data = TestDataManager::singleton()->getWaveData(wave_id);

    m_CustomPlot->addWave(wave_id, wave_title);
    m_CustomPlot->setData(wave_id, data);
}

void QmlCustomPlot::addAgingTestWave(QString wave_id, QString wave_title)
{
    QSharedPointer<QCPGraphDataContainer> data;

    data = RealTimeDataManager::singleton()->getAgingTestData(wave_id);

    m_CustomPlot->addWave(wave_id, wave_title);
    m_CustomPlot->setData(wave_id, data);
}

void QmlCustomPlot::addAgingHistoryWave(QString wave_id, QString wave_title)
{
    QSharedPointer<QCPGraphDataContainer> data;

    data = AgingTestDataManager::singleton()->getWaveData(wave_id);

    m_CustomPlot->addWave(wave_id, wave_title);
    m_CustomPlot->setData(wave_id, data);
}


void QmlCustomPlot::addLogWave(QString wave_id, QString wave_title)
{
    QSharedPointer<QCPGraphDataContainer> data;

    data = LogDataManager::singleton()->getWaveData(wave_id);

    m_CustomPlot->addWave(wave_id, wave_title);
    m_CustomPlot->setData(wave_id, data);
}


void QmlCustomPlot::deleteWave(QString wave_id)
{
    m_CustomPlot->deleteWave(wave_id);
}

void QmlCustomPlot::clearWave()
{
    m_CustomPlot->clearWave();

}

void QmlCustomPlot::setWaveVisiable(QString wave_id, bool visible)
{
    m_CustomPlot->setWaveVisiable(wave_id, visible);
}

void QmlCustomPlot::setColor(QString wave_id, QColor color)
{
    m_CustomPlot->setColor(wave_id, color);
}

void QmlCustomPlot::setXLabel(QString title)
{
    m_CustomPlot->setXLabel(title);
}

void QmlCustomPlot::resaclePlot()
{
    m_CustomPlot->rescaleAxes();
    m_CustomPlot->rescaleY();
    m_CustomPlot->replot();
}

void QmlCustomPlot::showDataPoint(bool show)
{
    m_CustomPlot->showDataPoint(show);
}

void QmlCustomPlot::showMouseXY(bool show)
{
    m_CustomPlot->showMouseXY(show);

}
void QmlCustomPlot::showMarkerAB(bool show)
{
    m_CustomPlot->showMarkerAB(show);
}

void QmlCustomPlot::updatePlot()
{
    QSharedPointer<QCPGraphDataContainer> data = m_CustomPlot->graph(0)->data();

    bool found_range;
    QCPRange range = data->keyRange(found_range);

    if (found_range)
    {
        m_EndTimeAxis = range.upper;
    }

    if (m_EndTimeAxis < m_TimeAxisLength)
    {
        m_CustomPlot->xAxis->setRange(0, m_TimeAxisLength, Qt::AlignLeft);
    }
    else
    {
        m_CustomPlot->xAxis->setRange(m_EndTimeAxis, m_TimeAxisLength, Qt::AlignRight);
    }

    m_CustomPlot->rescaleY();


    m_CustomPlot->replot(QCustomPlot::rpQueuedReplot);
}


void QmlCustomPlot::paint(QPainter *painter) {
  if (m_CustomPlot) {
    QPixmap picture(boundingRect().size().toSize());
    QCPPainter qcpPainter(&picture);

    m_CustomPlot->toPainter(&qcpPainter);

    painter->drawPixmap(QPoint(), picture);
  }
}

void QmlCustomPlot::mousePressEvent(QMouseEvent *event) {
  routeMouseEvents(event);
}

void QmlCustomPlot::mouseReleaseEvent(QMouseEvent *event) {
  routeMouseEvents(event);
}

void QmlCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
  routeMouseEvents(event);
}

void QmlCustomPlot::mouseDoubleClickEvent(QMouseEvent *event) {
  routeMouseEvents(event);
}

void QmlCustomPlot::wheelEvent(QWheelEvent *event)
{
    routeWheelEvents(event);
}

void QmlCustomPlot::graphClicked(QCPAbstractPlottable *plottable) {
  qDebug() << Q_FUNC_INFO
           << QString("Clicked on graph '%1 ").arg(plottable->name());
}

void QmlCustomPlot::routeMouseEvents(QMouseEvent *event) {
  if (m_CustomPlot) {
    QMouseEvent *newEvent =
        new QMouseEvent(event->type(), event->localPos(), event->button(),
                        event->buttons(), event->modifiers());
    QCoreApplication::postEvent(m_CustomPlot, newEvent);
  }
}

void QmlCustomPlot::routeWheelEvents(QWheelEvent *event) {
  if (m_CustomPlot) {
    QWheelEvent *newEvent = new QWheelEvent(
        event->pos(), event->globalPos(), event->pixelDelta(),
        event->angleDelta(), event->buttons(), event->modifiers(),
        event->phase(), event->inverted());
    QCoreApplication::postEvent(m_CustomPlot, newEvent);
  }
}

void QmlCustomPlot::updateCustomPlotSize() {
  if (m_CustomPlot) {
    m_CustomPlot->setGeometry(0, 0, (int)width(), (int)height());
    m_CustomPlot->setViewport(QRect(0, 0, (int)width(), (int)height()));
  }
}

void QmlCustomPlot::onCustomReplot() {
  update();
}

