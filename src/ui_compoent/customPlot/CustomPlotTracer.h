#ifndef MYTRACER_H
#define MYTRACER_H

#include <QObject>
#include "qcustomplot.h"

class CustomPlotTracer : public QObject
{
	Q_OBJECT

public:
	enum TracerType
	{
        XAxisTracer,//依附在x轴上显示x值
        YAxisTracer,//依附在y轴上显示y值
        XTextTracer,  //依附在x轴上显示字符串
        YTextTracer //依附在x轴上显示字符串
	};
    explicit CustomPlotTracer(QCustomPlot *_plot, TracerType _type, QObject *parent = Q_NULLPTR);
    ~CustomPlotTracer();
	void setPen(const QPen &pen);
	void setBrush(const QBrush &brush);
	void setText(const QString &text);
	void setLabelPen(const QPen &pen);
	void updatePosition(double xValue, double yValue);
    void setVisible(bool m_visible);

protected:
    bool m_visible;//是否可见
    TracerType m_type;//类型
    QCustomPlot *m_plot;//图表
    QCPItemTracer *m_tracer;//跟踪的点
    QCPItemText *m_label;//显示的数值
    QCPItemLine *m_arrow;//箭头
};

class CustomPlotTraceLine : public QObject
{
public:
    enum LineType
    {
        VerticalLine,//垂直线
        HorizonLine, //水平线
        Both//同时显示水平和垂直线
    };
    explicit CustomPlotTraceLine(QCustomPlot *_plot, LineType _type = VerticalLine, QObject *parent = Q_NULLPTR);
    ~CustomPlotTraceLine();
    void initLine();
    void updatePosition(double xValue, double yValue);

    void setVisible(bool vis)
    {
        if(m_LineV)
            m_LineV->setVisible(vis);
        if(m_LineH)
            m_LineH->setVisible(vis);
    }

    float getPixelValueX()
    {
        return m_ValueX;
    }

    float getPixelValueY()
    {
        return m_ValueY;
    }

    LineType GetType() 
    {
        return m_Type; 
    }

    double getPositionX() 
    {
        return m_plot->xAxis->pixelToCoord(m_ValueX);
    }

    double getPositionY()
    {
        return m_plot->yAxis->pixelToCoord(m_ValueY);
    }

    double getPositionY2()
    {
        return m_plot->yAxis2->pixelToCoord(m_ValueY);
    }

protected:
    bool m_Visible;//是否可见
    LineType m_Type;//类型
    QCustomPlot *m_plot;//图表
    QCPItemStraightLine *m_LineV; //垂直线
    QCPItemStraightLine *m_LineH; //水平线

    float m_ValueX;
    float m_ValueY;
};

#endif // MYTRACER_H
