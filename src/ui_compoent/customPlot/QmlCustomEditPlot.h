#pragma once

#include <QtQuick>
#include <QObject>
#include "qcustomplot.h"

class CustomPlot;
class QCPAbstractPlottable;

class CurvePlotType {
    Q_GADGET
public:
    enum CurveType {
        NormalLinearSpeed = 0,
        NormalLinearThrust,
        NormalCoefComp,
        NormalCustom,

        PostiveLinearSpeed,
        PostiveLinearThrust,
        PostiveCoef,
        PostiveCustom,

        NegativeLinearSpeed,
        NegativeLinearThrust,
        NegativeCoef,
        NegativeCustom,
    };
    Q_ENUM(CurveType)
};


class QmlCustomEditPlot : public QQuickPaintedItem {
    Q_OBJECT
public:
    QmlCustomEditPlot(QQuickItem *parent = 0);
    virtual ~QmlCustomEditPlot();

    void paint(QPainter *painter);

    Q_INVOKABLE void initCustomPlot();

    Q_INVOKABLE void setLable(QString x_label, QString y_label);

    Q_INVOKABLE void setParamID(int param_id);

    //Q_INVOKABLE void setCurveType(int curve_type);
    Q_INVOKABLE void acceptCurveData();

    Q_INVOKABLE void resaclePlot();

    Q_INVOKABLE void updateCustomPlotSize();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
private:
    QCustomPlot *m_CustomPlot;

//    int m_CurveType;

    int m_ParamID;
    QVector<double> m_XData;
    QVector<double> m_YData;

    int m_MaxPointNum;

    int m_Point;

private slots:
    void graphClicked(QCPAbstractPlottable *plottable);
    void onCustomReplot();
};

