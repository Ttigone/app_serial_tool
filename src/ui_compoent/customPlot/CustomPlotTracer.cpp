#include "CustomPlotTracer.h"
#include <QWidget>

CustomPlotTracer::CustomPlotTracer(QCustomPlot *_plot, TracerType _type, QObject *parent)
    : QObject(parent),
      m_plot(_plot),
      m_type(_type)
{
    m_visible = true;
    m_tracer = Q_NULLPTR;// 跟踪的点
    m_label = Q_NULLPTR;// 显示的数值
    m_arrow = Q_NULLPTR;// 箭头
    if (m_plot)
    {
        QColor clrDefault(Qt::darkGreen);
        QBrush brushDefault(Qt::NoBrush);
        QPen penDefault(clrDefault);
        //        penDefault.setBrush(brushDefault);
        penDefault.setWidthF(0.5);

        m_tracer = new QCPItemTracer(m_plot);
        m_tracer->setStyle(QCPItemTracer::tsCircle);
        m_tracer->setPen(penDefault);
        m_tracer->setBrush(brushDefault);

        m_label = new QCPItemText(m_plot);
        m_label->setLayer("overlay");
        m_label->setClipToAxisRect(false);
        m_label->setPadding(QMargins(0, 0, 0, 0));
        m_label->setBrush(brushDefault);
        m_label->setPen(penDefault);
        m_label->position->setParentAnchor(m_tracer->position);
        QFont f = m_label->font();
        f.setPointSize(8);
        m_label->setFont(f);
        m_label->setColor(clrDefault);
        m_label->setText("");
        //m_label->setDrawTextRect(false);

        m_arrow = new QCPItemLine(m_plot);
        QPen  arrowPen(clrDefault, 1);
        m_arrow->setPen(penDefault);
        m_arrow->setLayer("overlay");
        m_arrow->setClipToAxisRect(false);
        m_arrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状


        switch (m_type)
        {
        case XAxisTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptAbsolute);
            m_tracer->position->setTypeY(QCPItemPosition::ptAbsolute);
            m_tracer->setSize(3);
            m_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(0, 20);//偏移量
            break;
        }
        case XTextTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptAbsolute);
            m_tracer->position->setTypeY(QCPItemPosition::ptAbsolute);
            m_tracer->setSize(3);
            m_label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(0, 20);//偏移量
            break;
        }
        case YAxisTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptAbsolute);
            m_tracer->position->setTypeY(QCPItemPosition::ptAbsolute);
            m_tracer->setSize(3);
            m_label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(-20, 0);//偏移量
            break;
        }
        case YTextTracer:
        {
            m_tracer->position->setTypeX(QCPItemPosition::ptAbsolute);
            m_tracer->position->setTypeY(QCPItemPosition::ptAbsolute);
            m_tracer->setSize(3);
            m_label->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            m_arrow->end->setParentAnchor(m_tracer->position);
            m_arrow->start->setParentAnchor(m_arrow->end);
            m_arrow->start->setCoords(-20, 0);//偏移量
            break;
        }
        default:
            break;
        }
        setVisible(false);
    }
}

CustomPlotTracer::~CustomPlotTracer()
{
    if(m_plot)
    {
        if (m_tracer)
            m_plot->removeItem(m_tracer);
        if (m_label)
            m_plot->removeItem(m_label);
        if (m_arrow)
            m_plot->removeItem(m_arrow);
    }
}

void CustomPlotTracer::setPen(const QPen &pen)
{
    if(m_tracer)
        m_tracer->setPen(pen);
    if(m_arrow)
        m_arrow->setPen(pen);
}

void CustomPlotTracer::setBrush(const QBrush &brush)
{
    if(m_tracer)
        m_tracer->setBrush(brush);
}

void CustomPlotTracer::setLabelPen(const QPen &pen)
{
    if(m_label)
    {
        m_label->setPen(pen);
        m_label->setBrush(Qt::NoBrush);
        m_label->setColor(pen.color());
    }
}

void CustomPlotTracer::setText(const QString &text)
{
    if(m_label)
        m_label->setText(text);
}

void CustomPlotTracer::setVisible(bool vis)
{
    m_visible = vis;
    if(m_tracer)
        m_tracer->setVisible(m_visible);
    if(m_label)
        m_label->setVisible(m_visible);
    if(m_arrow)
        m_arrow->setVisible(m_visible);
}

void CustomPlotTracer::updatePosition(double xValue, double yValue)
{
    if (!m_visible)
    {
        setVisible(true);
        m_visible = true;
    }

    QRect x_rect = m_plot->xAxis->axisRect()->rect();
    QRect y_rect = m_plot->yAxis->axisRect()->rect();


    switch (m_type)
    {
    case XAxisTracer:
    {
        m_tracer->position->setCoords(xValue, y_rect.bottom());
        m_label->position->setCoords(0, 15);
        m_arrow->start->setCoords(0, 15);
        m_arrow->end->setCoords(0, 0);        
        setText(QString::number(m_plot->xAxis->pixelToCoord(xValue), 'f',1));
        break;
    }
    case YAxisTracer:
    {
        m_tracer->position->setCoords(x_rect.left(), yValue);
        m_label->position->setCoords(-20, 0);
        setText(QString::number(m_plot->yAxis->pixelToCoord(yValue), 'f', 1));
        break;
    }
    case XTextTracer:
    {
        m_tracer->position->setCoords(xValue, y_rect.bottom());
        m_label->position->setCoords(0, 15);
        m_arrow->start->setCoords(0, 15);
        m_arrow->end->setCoords(0, 0);
        break;
    }
    case YTextTracer:
    {
        m_tracer->position->setCoords(x_rect.left(), yValue);
        m_label->position->setCoords(-20, 0);
        break;
    }
    default:
        break;
    }
}

CustomPlotTraceLine::CustomPlotTraceLine(QCustomPlot *_plot, LineType _type, QObject *parent)
    : QObject(parent),
      m_Type(_type),
      m_plot(_plot)
{
    m_LineV = Q_NULLPTR;
    m_LineH = Q_NULLPTR;
    initLine();
}

CustomPlotTraceLine::~CustomPlotTraceLine()
{
    if(m_plot)
    {
        if (m_LineV)
            m_plot->removeItem(m_LineV);
        if (m_LineH)
            m_plot->removeItem(m_LineH);
    }
}

void CustomPlotTraceLine::initLine()
{
    if(m_plot)
    {
        QPen linesPen(Qt::darkGreen, 1, Qt::DashLine);

        if(VerticalLine == m_Type || Both == m_Type)
        {
            m_LineV = new QCPItemStraightLine(m_plot);//垂直线
            m_LineV->setLayer("overlay");
            m_LineV->setPen(linesPen);
            m_LineV->setClipToAxisRect(true);
            m_LineV->point1->setType(QCPItemPosition::ptAbsolute);
            m_LineV->point2->setType(QCPItemPosition::ptAbsolute);
        }

        if(HorizonLine == m_Type || Both == m_Type)
        {
            m_LineH = new QCPItemStraightLine(m_plot);//水平线
            m_LineH->setLayer("overlay");
            m_LineH->setPen(linesPen);
            m_LineH->setClipToAxisRect(true);
            m_LineH->point1->setType(QCPItemPosition::ptAbsolute);
            m_LineH->point2->setType(QCPItemPosition::ptAbsolute);
        }
    }
}

void CustomPlotTraceLine::updatePosition(double xValue, double yValue)
{
    if(VerticalLine == m_Type || Both == m_Type)
    {
        if(m_LineV)
        {
            QRect y_rect = m_plot->yAxis->axisRect()->rect();

            m_LineV->point1->setCoords(xValue, y_rect.bottom());
            m_LineV->point2->setCoords(xValue, y_rect.top());
        }
    }

    if(HorizonLine == m_Type || Both == m_Type)
    {
        if(m_LineH)
        {
            QRect x_rect = m_plot->xAxis->axisRect()->rect();
            m_LineH->point1->setCoords(x_rect.left(), yValue);
            m_LineH->point2->setCoords(x_rect.right(), yValue);
        }
    }

    m_ValueX = xValue;
    m_ValueY = yValue;
}


