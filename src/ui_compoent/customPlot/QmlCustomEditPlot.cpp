#include "QmlCustomEditPlot.h"

#include "qcustomplot.h"
#include "CustomPlot.h"
#include <QDebug>
#include "Data/RealTimeDataManager.h"
#include "Data/TestDataManager.h"

QmlCustomEditPlot::QmlCustomEditPlot(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_CustomPlot(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);

    connect(this, &QQuickPaintedItem::widthChanged, this,
            &QmlCustomEditPlot::updateCustomPlotSize);
    connect(this, &QQuickPaintedItem::heightChanged, this,
            &QmlCustomEditPlot::updateCustomPlotSize);
}

QmlCustomEditPlot::~QmlCustomEditPlot() {
    delete m_CustomPlot;
    m_CustomPlot = nullptr;
}

void QmlCustomEditPlot::initCustomPlot() {
    m_MaxPointNum = 10;
    m_Point = -1;

    m_XData.clear();
    m_XData.append(0);
    m_XData.append(100);

    m_YData.clear();
    m_YData.append(0);
    m_YData.append(100);

    m_CustomPlot = new QCustomPlot();

    connect(m_CustomPlot, &QCustomPlot::afterReplot, this,
            &QmlCustomEditPlot::onCustomReplot);

    m_CustomPlot->replot();

    m_CustomPlot->xAxis->setRange(-10, 110);
    m_CustomPlot->xAxis->ticker()->setTickCount(12);
    m_CustomPlot->yAxis->setRange(-10, 110);
    m_CustomPlot->axisRect()->setupFullAxesBox();
    m_CustomPlot->yAxis->ticker()->setTickCount(12);

    m_CustomPlot->xAxis->setLabel("x Axis");
    m_CustomPlot->yAxis->setLabel("y Axis");

    m_CustomPlot->addGraph();
    m_CustomPlot->graph()->setData(m_XData, m_YData);
    m_CustomPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(Qt::red, 3), QBrush(Qt::white), 4));
}

void QmlCustomEditPlot::setLable(QString x_label, QString y_label)
{
    m_CustomPlot->xAxis->setLabel(x_label);
    m_CustomPlot->yAxis->setLabel(y_label);
}



void QmlCustomEditPlot::setParamID(int param_id)
{
    m_XData.clear();
    m_YData.clear();

    m_ParamID = param_id;

    Parameter* custom_speed  = ParamManager::singleton()->getParamPtr(param_id);

    if(custom_speed == NULL)
        return;

    int valid_len = custom_speed->getArrayValue(0);

    if(valid_len == 0)
    {
        m_XData.append(0);
        m_XData.append(100);
        m_YData.append(0);
        m_YData.append(100);
    }
    else
    {
        for(int i = 0; i < valid_len; i++)
        {
            double x = custom_speed->getArrayValue(i*2+1) / 10.0f;
            double y = custom_speed->getArrayValue(i*2+2) / 10.0f;
            m_XData.append(x);
            m_YData.append(y);
        }
    }

    m_CustomPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QPen(Qt::red, 3), QBrush(Qt::white), 4));

    m_CustomPlot->graph()->setData(m_XData, m_YData);
    m_CustomPlot->replot();
}


/*
void QmlCustomEditPlot::setCurveType(int curve_type)
{
    m_CurveType = curve_type;
    m_XData.clear();
    m_YData.clear();

    switch (m_CurveType) {
    case CurvePlotType::NormalLinearSpeed:
        m_XData.append(0);
        m_XData.append(100);
        m_YData.append(0);
        m_YData.append(100);
        m_CustomPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, QPen(Qt::red, 3), QBrush(Qt::white), 4));
        break;
    case CurvePlotType::NormalLinearThrust:
        for(int i = 0; i <= 100; i++)
        {
            double x = i;
            double y = qSqrt(i/100.0f)*100;
            m_XData.append(x);
            m_YData.append(y);
        }
        m_CustomPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, QPen(Qt::red, 3), QBrush(Qt::white), 4));
        break;
    case CurvePlotType::NormalCoefComp:
    {
        Parameter* normal_ppm_start = ParamManager::singleton()->getParamPtr("normal_pwm_start");
        Parameter* normal_ppm_end = ParamManager::singleton()->getParamPtr("normal_pwm_end");
        Parameter* normal_coef = ParamManager::singleton()->getParamPtr("normal_pwm_curve_comp_coef");
        Parameter* motor_max_rpm = ParamManager::singleton()->getParamPtr("motor_max_rpm");

        int ppm_start = normal_ppm_start->getValue()/10;
        int ppm_end = normal_ppm_end->getValue()/10;
        int coef = normal_coef->getValue();
        int max_rpm = motor_max_rpm->getValue();
        double ppm_range = qAbs(ppm_end - ppm_start);

        for(int i = ppm_start; i <= ppm_end; i++)
        {
            double pwm = i;
            double ppm_value = qAbs(i - ppm_start);

            double rpm = qAbs(i - ppm_start) / ppm_range * max_rpm;
            double comp_rpm = rpm;

            if(coef != 0)
            {
                comp_rpm = rpm * (ppm_range + coef) / (ppm_value + coef);
            }

            double x = ppm_value / ppm_range;
            double y = comp_rpm / max_rpm;

            m_XData.append(x);
            m_YData.append(y);
        }
        m_CustomPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, QPen(Qt::red, 3), QBrush(Qt::white), 4));
    }
        break;
    case CurvePlotType::NormalCustom:
    {
        Parameter* normal_custom_speed  = ParamManager::singleton()->getParamPtr("normal_custom_speed_curve");

    }
    default:
        break;
    }

    m_CustomPlot->graph()->setData(m_XData, m_YData);
    m_CustomPlot->replot();
}
*/

void QmlCustomEditPlot::acceptCurveData()
{
    QString value_str = "";

    int value_dim = m_XData.size();

    value_str.append(QString::number(value_dim));

    for(int i = 0; i < value_dim; i++)
    {
        value_str.append(",");
        int x_value = m_XData[i]*10;
        value_str.append(QString::number(x_value));

        value_str.append(",");
        int y_value = m_YData[i]*10;
        value_str.append(QString::number(y_value));
    }

    ParamManager::singleton()->setParamValueString(m_ParamID, value_str);
}

void QmlCustomEditPlot::resaclePlot()
{
    m_CustomPlot->rescaleAxes();
    m_CustomPlot->replot();
}


void QmlCustomEditPlot::paint(QPainter *painter) {
    if (m_CustomPlot) {
        QPixmap picture(boundingRect().size().toSize());
        QCPPainter qcpPainter(&picture);

        m_CustomPlot->toPainter(&qcpPainter);

        painter->drawPixmap(QPoint(), picture);
    }
}

void QmlCustomEditPlot::mousePressEvent(QMouseEvent *event)
{
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();

    float x_val = m_CustomPlot->xAxis->pixelToCoord(x_pos);
    float y_val = m_CustomPlot->yAxis->pixelToCoord(y_pos);

    m_Point = -1;
    for (int i = 0; i < m_XData.size(); i++) {
        if (Utility::FloatEqual(x_val,  m_XData.at(i), 1)
                && Utility::FloatEqual(y_val,  m_YData.at(i), 1) )
        {
            m_Point = i;
            break;
        }
    }

    if (event->button() == Qt::RightButton)
    {
        if (m_Point != -1)
        {
            m_XData.remove(m_Point);
            m_YData.remove(m_Point);
            m_Point = -1;
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        //未能找到点,则新增点
        if (m_Point == -1 && m_XData.size() < m_MaxPointNum)
        {
            for (int i = 0; i < m_XData.size() - 1; i++)
            {
                if (Utility::FloatGreaterEqual(x_val, m_XData.at(i))
                        && Utility::FloatLessEqual(x_val, m_XData.at(i + 1)))
                {
                    if (Utility::FloatLess(y_val, m_YData.at(i)))
                    {
                        y_val = m_YData.at(i);
                    }

                    if (Utility::FloatGreater(y_val, m_YData.at(i+1)))
                    {
                        y_val = m_YData.at(i+1);
                    }

                    if (Utility::FloatGreater(y_val, m_YData.at(m_YData.size() - 1)))
                    {
                        y_val = m_YData.at(m_YData.size() - 1);
                    }

                    m_XData.insert(i + 1, x_val);
                    m_YData.insert(i + 1, y_val);
                    m_Point = i + 1;
                    break;
                }
            }
        }
    }

    m_CustomPlot->graph()->setData(m_XData, m_YData);
    m_CustomPlot->replot();
}

void QmlCustomEditPlot::mouseReleaseEvent(QMouseEvent *event)
{
    m_Point = -1;
}

void QmlCustomEditPlot::mouseMoveEvent(QMouseEvent *event)
{    
    if (m_Point == -1)
        return;

    m_CustomPlot->setInteractions(QCP::iRangeZoom);

    int x_pos = event->pos().x();
    int y_pos = event->pos().y();

    float x_val = m_CustomPlot->xAxis->pixelToCoord(x_pos);
    float y_val = m_CustomPlot->yAxis->pixelToCoord(y_pos);

    float x_min = 0;
    float x_max = 100;

    float y_min = 0;
    float y_max = 100;

    if(m_Point == 0)
    {
        x_min = 0;
        x_max = 0;
    }
    else if(m_Point == m_XData.size() - 1)
    {
        x_min = 100;
        x_max = 100;
    }
    else
    {
        x_min =  m_XData.at(m_Point - 1);
        x_max = m_XData.at(m_Point + 1);
    }

    if (Utility::FloatLess(x_val, x_min, 0.1f))
    {
        x_val = x_min;
    }

    if (Utility::FloatGreater(x_val, x_max, 0.1f))
    {
        x_val = x_max;
    }


    if(m_Point == 0)
    {
        y_min = 0;
        y_max = m_YData.at(m_Point + 1);
    }
    else if(m_Point == m_YData.size() - 1)
    {
        y_min = m_YData.at(m_Point - 1);
        y_max = 100;
    }
    else
    {
        y_min =  m_YData.at(m_Point - 1);
        y_max = m_YData.at(m_Point + 1);
    }

    if (Utility::FloatLess(y_val, y_min, 0.1f))
    {
        y_val = y_min;
    }

    if (Utility::FloatGreater(y_val, y_max, 0.1f))
    {
        y_val = y_max;
    }


    for (int i = m_Point + 1; i < m_YData.size() - 1; i++)
    {
        if (Utility::FloatGreater(y_val, m_YData.at(i)))
        {
            m_YData[i] = y_val;
        }
    }

    m_XData[m_Point] = x_val;
    m_YData[m_Point] = y_val;

    m_CustomPlot->graph()->setData(m_XData, m_YData);
    m_CustomPlot->replot();
}

void QmlCustomEditPlot::graphClicked(QCPAbstractPlottable *plottable) {
    qDebug() << Q_FUNC_INFO
             << QString("Clicked on graph '%1 ").arg(plottable->name());
}

void QmlCustomEditPlot::updateCustomPlotSize() {
    if (m_CustomPlot) {
        m_CustomPlot->setGeometry(0, 0, (int)width(), (int)height());
        m_CustomPlot->setViewport(QRect(0, 0, (int)width(), (int)height()));
    }
}

void QmlCustomEditPlot::onCustomReplot() {
    update();
}

