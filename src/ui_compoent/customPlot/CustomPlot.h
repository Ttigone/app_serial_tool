#pragma once

#include <QPoint>
#include "qcustomplot.h"
#include "CustomPlotTracer.h"

class QRubberBand;
class QMouseEvent;
class QWidget;

class CustomPlot : public QCustomPlot
{
    Q_OBJECT

public:
    CustomPlot(QWidget* parent = Q_NULLPTR);

    void addWave(QString wave_id, QString wave_title, QCPAxis::AxisType  y_type = QCPAxis::atLeft);
    void deleteWave(QString wave_id);
    void clearWave();

    void setLegendVisiable(bool visible);

    void setWaveVisiable(QString wave_id, bool visible);

    void setColor(QString wave_id, QColor color);

    void setData(QString wave_id, QSharedPointer<QCPGraphDataContainer> data);
    void setData(QString wave_id, QVector<double>& keys, QVector<double>& values);

    void setTittle(QString title, QCPAxis::AxisType  y_type = QCPAxis::atLeft);
    void setXLabel(QString title);

    void showDataPoint(bool show);

    void showMouseXY(bool show);
    bool isShowTracer();

    void showMarkerAB(bool show);

    void rescaleY(QCPAxis::AxisType  y_type = QCPAxis::atLeft);

private slots:
    void selectionChanged();
    void beforeReplotProcess();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void initCursor();

    QString  getCursorText();
private:
    QCPItemRect* m_ZoomRectItem;
    QRect m_ZoomRect;
    bool m_ZoomRectFlag;

    QPoint mOrigin;

    QCPTextElement* m_Title;

    bool m_ShowTracer;//是否显示追踪器（鼠标在图中移动，显示对应的值）
    CustomPlotTracer* m_XTracer;//x轴
    CustomPlotTracer* m_YTracer;//y轴
    CustomPlotTraceLine* m_XYTraceLine;//直线

    bool m_ShowCursor; 

    CustomPlotTraceLine* m_XCursorA;//x轴直线1
    CustomPlotTracer* m_XCursorTracerA;//x轴坐标A

    CustomPlotTraceLine* m_XCursorB;//x轴直线2
    CustomPlotTracer* m_XCursorTracerB;//x轴坐标B

    CustomPlotTraceLine* m_YCursorA;//y轴直线A
    CustomPlotTracer* m_YCursorTracerA;//y轴坐标A

    CustomPlotTraceLine* m_YCursorB;//y轴直线B
    CustomPlotTracer* m_YCursorTracerB;//y轴坐标B

    CustomPlotTraceLine* m_SelectedCursorLine;
    CustomPlotTracer* m_SelectedTracer;

    QCPItemText* m_CursorText;//关于坐标轴差值计算文本

    QMap<QString, int> m_GraphIndexList;

    bool m_yAxis2ShowFlag;
};
