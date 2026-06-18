#pragma once

#include <QtQuick>
class CustomPlot;
class QCPAbstractPlottable;

class QmlCustomPlot : public QQuickPaintedItem {
  Q_OBJECT

public:
  QmlCustomPlot(QQuickItem *parent = 0);
  virtual ~QmlCustomPlot();

  void paint(QPainter *painter);

  Q_INVOKABLE void initCustomPlot();

  Q_INVOKABLE void setLegendVisiable(bool visible);

  Q_INVOKABLE void addRealWave(QString wave_id, QString wave_title);
  Q_INVOKABLE void addHistoryWave(QString wave_id, QString wave_title);

  Q_INVOKABLE void addAgingTestWave(QString wave_id, QString wave_title);
  Q_INVOKABLE void addAgingHistoryWave(QString wave_id, QString wave_title);

  Q_INVOKABLE void addLogWave(QString wave_id, QString wave_title);


  Q_INVOKABLE void deleteWave(QString wave_id);
  Q_INVOKABLE void clearWave();

  Q_INVOKABLE void setWaveVisiable(QString wave_id, bool visible);

  Q_INVOKABLE void setColor(QString wave_id, QColor color);

  Q_INVOKABLE void setXLabel(QString title);

  Q_INVOKABLE void resaclePlot();

  Q_INVOKABLE void showDataPoint(bool show);
  Q_INVOKABLE void showMouseXY(bool show);
  Q_INVOKABLE void showMarkerAB(bool show);

  Q_INVOKABLE void updatePlot();

  Q_INVOKABLE void updateCustomPlotSize();

protected:
  void routeMouseEvents(QMouseEvent *event);
  void routeWheelEvents(QWheelEvent *event);

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);

private:
  CustomPlot *m_CustomPlot;
  int m_TimeAxisLength;
  float m_EndTimeAxis;

private slots:
  void graphClicked(QCPAbstractPlottable *plottable);
  void onCustomReplot();
};

