#include "CustomPlot.h"
#include "Utils/Utility.h"
#include <QRubberBand>

CustomPlot::CustomPlot(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    m_ZoomRectItem = new QCPItemRect(this);
    m_ZoomRectItem->setVisible(false);
    m_ZoomRectFlag = false;

    xAxis->setRange(0, 30);
    yAxis->setRange(-100, 100);
    //yAxis2->setRange(-1000, 1000);

    axisRect()->setupFullAxesBox();
    //axisRect()->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);

    axisRect()->setAutoMargins(QCP::MarginSide::msBottom | QCP::MarginSide::msTop | QCP::MarginSide::msRight| QCP::MarginSide::msLeft);
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);

    xAxis->setLabel("x Axis");
    yAxis->setLabel("y Axis");

    //axisRect()->setRangeZoomFactor(1, 1.1);
    legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    legend->setBrush(QBrush(QColor(255, 255, 255, 200)));

    //x轴同步
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));

    //y轴同步
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));

    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(this, SIGNAL(beforeReplot()), this, SLOT(beforeReplotProcess()));

    m_yAxis2ShowFlag = false;


//    mRubberBand->setGeometry(QRect(QPoint(0, 0), QSize(1000, 1000)));
//    mRubberBand->show();


//    selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//设置选框的样式：虚线
//    selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
//    setSelectionRectMode(QCP::SelectionRectMode::srmZoom);

    initCursor();
}

void CustomPlot::initCursor()
{
    m_ShowCursor = false;

    m_XCursorA = new CustomPlotTraceLine(this, CustomPlotTraceLine::VerticalLine);
    m_XCursorTracerA = new CustomPlotTracer(this, CustomPlotTracer::XTextTracer);
    m_XCursorTracerA->setText("X-A");

    m_XCursorB = new CustomPlotTraceLine(this, CustomPlotTraceLine::VerticalLine);
    m_XCursorTracerB = new CustomPlotTracer(this, CustomPlotTracer::XTextTracer);
    m_XCursorTracerB->setText("X-B");

    m_YCursorA = new CustomPlotTraceLine(this, CustomPlotTraceLine::HorizonLine);
    m_YCursorTracerA = new CustomPlotTracer(this, CustomPlotTracer::YTextTracer);
    m_YCursorTracerA->setText("Y-A");

    m_YCursorB = new CustomPlotTraceLine(this, CustomPlotTraceLine::HorizonLine);
    m_YCursorTracerB = new CustomPlotTracer(this, CustomPlotTracer::YTextTracer);
    m_YCursorTracerB->setText("Y-B");

    m_CursorText = new QCPItemText(this);//构造一个文本
    m_CursorText->position->setType(QCPItemPosition::ptAxisRectRatio);//设置文本坐标解析方式
    m_CursorText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);//设置位置在矩形区域的位置
    m_CursorText->position->setCoords(0.99, 0.01); // 设置位置，注意第三行代码的枚举类型和这儿的值解析方式有关系
    m_CursorText->setTextAlignment(Qt::AlignLeft);//设置文本在矩形区域的位置
    m_CursorText->setFont(QFont(font().family(), 9));//设置文本的字体
    m_CursorText->setColor(Qt::red);//字体颜色
    m_CursorText->setPen(QPen(Qt::black)); //矩形框颜色
    m_CursorText->setPadding(QMargins(2, 2, 2, 2));//设置文本所在矩形的margins*/
    m_CursorText->setText(getCursorText());//文本描述

    m_CursorText->setVisible(false);

    m_XCursorA->setVisible(false);
    m_XCursorTracerA->setVisible(false);
    m_XCursorB->setVisible(false);
    m_XCursorTracerB->setVisible(false);

    m_YCursorA->setVisible(false);
    m_YCursorTracerA->setVisible(false);
    m_YCursorB->setVisible(false);
    m_YCursorTracerB->setVisible(false);

    m_SelectedCursorLine = NULL;
    m_SelectedTracer = NULL;


    m_XTracer = new CustomPlotTracer(this, CustomPlotTracer::XAxisTracer);//x轴
    m_YTracer = new CustomPlotTracer(this, CustomPlotTracer::YAxisTracer);//y轴
    m_XYTraceLine = new CustomPlotTraceLine(this, CustomPlotTraceLine::Both);//直线

    m_ShowTracer = false;

    m_XTracer->setVisible(m_ShowTracer);
    m_YTracer->setVisible(m_ShowTracer);
    m_XYTraceLine->setVisible(m_ShowTracer);


}

void CustomPlot::setTittle(QString title, QCPAxis::AxisType  y_type)
{
    if (y_type == QCPAxis::atRight)
    {
        yAxis2->setLabel(title);
    }
    else
    {
        yAxis->setLabel(title);
    }
}

void CustomPlot::setXLabel(QString title)
{
    xAxis->setLabel(title);
}


void CustomPlot::showMouseXY(bool show)
{
    m_ShowTracer = show;

    m_XTracer->setVisible(m_ShowTracer);
    m_YTracer->setVisible(m_ShowTracer);
    m_XYTraceLine->setVisible(m_ShowTracer);

    this->replot();//曲线重绘
}

bool CustomPlot::isShowTracer()
{
    return m_ShowTracer;
}

void CustomPlot::showMarkerAB(bool show)
{
    m_ShowCursor = show;

    m_CursorText->setVisible(m_ShowCursor);

    m_XCursorA->setVisible(m_ShowCursor);
    m_XCursorTracerA->setVisible(m_ShowCursor);
    m_XCursorB->setVisible(m_ShowCursor);
    m_XCursorTracerB->setVisible(m_ShowCursor);

    m_YCursorA->setVisible(m_ShowCursor);
    m_YCursorTracerA->setVisible(m_ShowCursor);
    m_YCursorB->setVisible(m_ShowCursor);
    m_YCursorTracerB->setVisible(m_ShowCursor);

    if (m_ShowCursor)
    {

        QRect x_rect = this->xAxis->axisRect()->rect();
        QRect y_rect = this->yAxis->axisRect()->rect();
        float x_cursor_a =x_rect.x() + x_rect.width() * 0.1;
        float x_cursor_b =x_rect.x() + x_rect.width() * 0.9;


        m_XCursorA->updatePosition(x_cursor_a, 0);
        m_XCursorTracerA->updatePosition(x_cursor_a, 0);

        m_XCursorB->updatePosition(x_cursor_b, 0);
        m_XCursorTracerB->updatePosition(x_cursor_b, 0);

        float y_cursor_a = y_rect.y() + y_rect.height() * 0.9;
        float y_cursor_b = y_rect.y() + y_rect.height() * 0.1;

        m_YCursorA->updatePosition(0, y_cursor_a);
        m_YCursorTracerA->updatePosition(0, y_cursor_a);

        m_YCursorB->updatePosition(0, y_cursor_b);
        m_YCursorTracerB->updatePosition(0, y_cursor_b);

        m_CursorText->setText(getCursorText());//文本描述
    }


    this->replot();//曲线重绘
}

void CustomPlot::addWave(QString wave_id, QString wave_title, QCPAxis::AxisType  y_type)
{
    if (!m_GraphIndexList.contains(wave_id))
    {
        int graph_index = graphCount();

        QCPAxis* wave_y_axis = yAxis;
        if (y_type == QCPAxis::atRight)
        {
            wave_y_axis = yAxis2;
            m_yAxis2ShowFlag = true;
        }

        addGraph(xAxis, wave_y_axis);

        graph(graph_index)->setName(wave_title);

        m_GraphIndexList[wave_id] = graph_index;
    }
}

void CustomPlot::deleteWave(QString wave_id)
{
    if (m_GraphIndexList.contains(wave_id))
    {
        int graph_index = m_GraphIndexList[wave_id];
        removeGraph(graph_index);
        m_GraphIndexList.remove(wave_id);

        QMap<QString, int>::iterator iter;
        for (iter = m_GraphIndexList.begin(); iter != m_GraphIndexList.end(); iter++)
        {
            if (iter.value() > graph_index)
            {
                iter.value() = iter.value() - 1;
            }
        }
    }
}

void CustomPlot::clearWave()
{
    m_GraphIndexList.clear();

    clearGraphs();
}

void CustomPlot::setLegendVisiable(bool visible)
{
    legend->setVisible(visible);
}

void CustomPlot::setWaveVisiable(QString wave_id, bool visible)
{
    if (m_GraphIndexList.contains(wave_id))
    {
        int graph_index = m_GraphIndexList[wave_id];
        graph(graph_index)->setVisible(visible);
    }
}


void CustomPlot::setColor(QString wave_id,QColor color)
{
    if (m_GraphIndexList.contains(wave_id))
    {
        QPen pen(color);
        int graph_index = m_GraphIndexList[wave_id];
        graph(graph_index)->setPen(pen);
    }
}

void CustomPlot::setData(QString wave_id, QSharedPointer<QCPGraphDataContainer> data)
{
    if (m_GraphIndexList.contains(wave_id))
    {
        int graph_index = m_GraphIndexList[wave_id];
        graph(graph_index)->setData(data);
    }
}

void CustomPlot::setData(QString wave_id, QVector<double>& keys, QVector<double>& values)
{
    if (m_GraphIndexList.contains(wave_id))
    {
        int graph_index = m_GraphIndexList[wave_id];
        graph(graph_index)->setData(keys, values);
    }
}

void CustomPlot::showDataPoint(bool show)
{
    QCPGraph::LineStyle line_style = QCPGraph::LineStyle::lsLine;

    QCPScatterStyle scatter_style;
    if (show)
    {
        line_style = QCPGraph::LineStyle::lsLine;
        scatter_style.setShape(QCPScatterStyle::ssCross);
        scatter_style.setSize(3);
    }
    else
    {
        line_style = QCPGraph::LineStyle::lsLine;
        scatter_style.setShape(QCPScatterStyle::ssNone);
    }

    for (int i = 0; i < graphCount(); i++)
    {
        graph(i)->setLineStyle(line_style);

        QPen pen = graph(i)->pen();
        scatter_style.setPen(pen);
        graph(i)->setScatterStyle(scatter_style);
    }

    replot();
}

void CustomPlot::rescaleY(QCPAxis::AxisType  y_type)
{
    QCPAxis* wave_y_axis = yAxis;
    if (y_type == QCPAxis::atRight)
    {
        wave_y_axis = yAxis2;
    }

    QList<QCPAbstractPlottable*> p = wave_y_axis->plottables();
    QCPRange newRange;
    bool haveRange = false;
    for (int i = 0; i < p.size(); ++i)
    {
        if (!p.at(i)->realVisibility())
            continue;

        QCPRange plottableRange;
        bool currentFoundRange;
        QCP::SignDomain signDomain = QCP::sdBoth;

        plottableRange = p.at(i)->getValueRange(currentFoundRange, signDomain);

        if (currentFoundRange)
        {
            if (!haveRange)
                newRange = plottableRange;
            else
                newRange.expand(plottableRange);
            haveRange = true;
        }
    }
    newRange.lower = newRange.lower - (newRange.size()+10) *0.05;
    newRange.upper = newRange.upper + (newRange.size() + 10) * 0.05;

    wave_y_axis->setRange(newRange);
}

void CustomPlot::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        mOrigin = event->pos();

        m_ZoomRect.setTopLeft(event->pos());
        double x = xAxis->pixelToCoord(event->pos().x());
        double y = yAxis->pixelToCoord(event->pos().y());

        m_ZoomRectItem->topLeft->setCoords(x, y);
        m_ZoomRectItem->bottomRight->setCoords(x,y);
        m_ZoomRectItem->setVisible(true);

        m_ZoomRectFlag = true;

        this->replot();//曲线重绘

    }
    else
    {

        if (m_ShowTracer && event->button() == Qt::LeftButton)
        {
            //当前鼠标位置（像素坐标）
            int x_pos = event->pos().x();
            int y_pos = event->pos().y();

            QRect rect = axisRect()->rect();
            if (rect.contains(x_pos, y_pos))
            {
                m_XTracer->setVisible(true);
                m_YTracer->setVisible(true);
                m_XYTraceLine->setVisible(true);

                m_XTracer->updatePosition(x_pos, y_pos);
                m_YTracer->updatePosition(x_pos, y_pos);
                m_XYTraceLine->updatePosition(x_pos, y_pos);
            }
            else
            {
                m_XTracer->setVisible(false);
                m_YTracer->setVisible(false);
                m_XYTraceLine->setVisible(false);
            }

            this->replot();//曲线重绘
        }


        if (m_ShowCursor)
        {
            //当前鼠标位置（像素坐标）
            int x_pos = event->pos().x();
            int y_pos = event->pos().y();

            if (Utility::FloatEqual(x_pos, m_XCursorA->getPixelValueX(), 3))
            {
                m_SelectedTracer = m_XCursorTracerA;
                m_SelectedCursorLine = m_XCursorA;
            }
            else if (Utility::FloatEqual(x_pos, m_XCursorB->getPixelValueX(), 3))
            {
                m_SelectedTracer = m_XCursorTracerB;
                m_SelectedCursorLine = m_XCursorB;
            }
            else if (Utility::FloatEqual(y_pos, m_YCursorA->getPixelValueY(), 3))
            {
                m_SelectedTracer = m_YCursorTracerA;
                m_SelectedCursorLine = m_YCursorA;
            }
            else if (Utility::FloatEqual(y_pos, m_YCursorB->getPixelValueY(), 3))
            {
                m_SelectedTracer = m_YCursorTracerB;
                m_SelectedCursorLine = m_YCursorB;
            }

            if (m_SelectedCursorLine != NULL || m_SelectedTracer != NULL)
            {
                setInteraction(QCP::iRangeDrag, false);
            }
        }

        if (m_SelectedCursorLine == NULL)
        {
            if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
                axisRect()->setRangeDrag(xAxis->orientation());
            else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
            {
                axisRect()->setRangeDragAxes(xAxis, yAxis);
                axisRect()->setRangeDrag(yAxis->orientation());
            }
            else if (yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
            {
                axisRect()->setRangeDragAxes(xAxis, yAxis2);
                axisRect()->setRangeDrag(yAxis2->orientation());
            }
            else
            {
                QList< QCPAxis*>  horizontal;
                horizontal.append(xAxis);
                horizontal.append(xAxis2);

                QList< QCPAxis*>  vertical;
                vertical.append(yAxis);
                vertical.append(yAxis2);

                axisRect()->setRangeDragAxes(horizontal, vertical);
                axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
            }
        }
    }

    QCustomPlot::mousePressEvent(event);
}

void CustomPlot::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::RightButton)
    {
        if(m_ZoomRectFlag)
        {
            m_ZoomRect.setBottomRight(event->pos());
            double x = xAxis->pixelToCoord(event->pos().x());
            double y = yAxis->pixelToCoord(event->pos().y());

            m_ZoomRectItem->bottomRight->setCoords(x,y);

            this->replot();//曲线重绘
        }
    }
    else
    {

        if (m_ShowCursor)
        {
            //当前鼠标位置（像素坐标）
            int x_pos = event->pos().x();
            int y_pos = event->pos().y();

            QRect rect = axisRect()->rect();
            if (rect.contains(x_pos, y_pos))
            {
                //像素坐标转成实际的x,y轴的坐标
                if (m_SelectedCursorLine != NULL && m_SelectedTracer != NULL)
                {
                    if (m_SelectedCursorLine->GetType() == CustomPlotTraceLine::VerticalLine)
                    {
                        m_SelectedCursorLine->updatePosition(x_pos, 0);
                        m_SelectedTracer->updatePosition(x_pos, 0);
                    }
                    else
                    {
                        m_SelectedCursorLine->updatePosition(0, y_pos);
                        m_SelectedTracer->updatePosition(0, y_pos);
                    }
                    m_CursorText->setText(getCursorText());//文本描述
                    this->replot();//曲线重绘
                }
            }
        }
    }

    QCustomPlot::mouseMoveEvent(event);
}

void CustomPlot::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_ZoomRectFlag)
    {
        //qDebug() << m_ZoomRect.width() << m_ZoomRect.height();
        if (qAbs(m_ZoomRect.width()) > 10 && qAbs(m_ZoomRect.height()) > 10)
        {
            int xp1, yp1, xp2, yp2;
            m_ZoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
            double x1 = xAxis->pixelToCoord(xp1);
            double x2 = xAxis->pixelToCoord(xp2);
            double y1 = yAxis->pixelToCoord(yp1);
            double y2 = yAxis->pixelToCoord(yp2);

            xAxis->setRange(x1, x2);
            yAxis->setRange(y1, y2);

            if (m_yAxis2ShowFlag)
            {
                double y3 = yAxis2->pixelToCoord(yp1);
                double y4 = yAxis2->pixelToCoord(yp2);
                yAxis2->setRange(y3, y4);
            }

            m_ZoomRectItem->setVisible(false);

            m_ZoomRectFlag = false;

            replot();
        }
    }

    if (m_SelectedCursorLine != NULL || m_SelectedTracer!= NULL)
    {
        setInteraction(QCP::iRangeDrag, true);
        m_SelectedCursorLine = NULL;
        m_SelectedTracer = NULL;
    }

    QCustomPlot::mouseReleaseEvent(event);
}

void CustomPlot::wheelEvent(QWheelEvent* event)
{
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        axisRect()->setRangeZoom(xAxis->orientation());
    }
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        axisRect()->setRangeZoomAxes(xAxis, yAxis);
        axisRect()->setRangeZoom(yAxis->orientation());
    }
    else if (yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
    {
        axisRect()->setRangeZoomAxes(xAxis, yAxis2);
        axisRect()->setRangeZoom(yAxis2->orientation());
    }
    else
    {
        QList< QCPAxis*>  horizontal;
        horizontal.append(xAxis);
        horizontal.append(xAxis2);

        QList< QCPAxis*>  vertical;
        vertical.append(yAxis);
        vertical.append(yAxis2);

        axisRect()->setRangeZoomAxes(horizontal, vertical);
        axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }

    QCustomPlot::wheelEvent(event);
}

void CustomPlot::selectionChanged()
{
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis) || xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
        xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) || yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    if (yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i = 0; i < graphCount(); ++i)
    {
        QCPGraph* tmp_graph = graph(i);
        QCPPlottableLegendItem* item = legend->itemWithPlottable(tmp_graph);
        if (item->selected() || tmp_graph->selected())
        {
            item->setSelected(true);
            tmp_graph->setSelection(QCPDataSelection(tmp_graph->data()->dataRange()));
        }
    }
}

void CustomPlot::beforeReplotProcess()
{
    if (m_ShowTracer)
    {
        //当前鼠标位置（像素坐标）
        int x_pos = m_XYTraceLine->getPixelValueX();
        int y_pos = m_XYTraceLine->getPixelValueY();

        QRect rect = axisRect()->rect();
        if (rect.contains(x_pos, y_pos))
        {
            m_XTracer->setVisible(true);
            m_YTracer->setVisible(true);
            m_XYTraceLine->setVisible(true);

            m_XTracer->updatePosition(x_pos, y_pos);
            m_YTracer->updatePosition(x_pos, y_pos);
            m_XYTraceLine->updatePosition(x_pos, y_pos);
        }
        else
        {
            m_XTracer->setVisible(false);
            m_YTracer->setVisible(false);
            m_XYTraceLine->setVisible(false);
        }
    }

    if (m_ShowCursor)
    {
        m_CursorText->setText(getCursorText());//文本描述
    }
}

QString  CustomPlot::getCursorText()
{
    if (!m_ShowCursor)
        return "";
    QString cursor_text;
    float x_a = m_XCursorA->getPositionX();
    float x_b = m_XCursorB->getPositionX();
    float y_a = m_YCursorA->getPositionY();
    float y_b = m_YCursorB->getPositionY();

    cursor_text = "Xa:" + QString::number(x_a, 'f', 3) + "\n";
    cursor_text += "Xb:" + QString::number(x_b, 'f', 3) + "\n";
    cursor_text += "Ya:" + QString::number(y_a, 'f', 3) + "\n";
    cursor_text += "Yb:" + QString::number(y_b, 'f', 3) + "\n";
    cursor_text += "Xb - Xa:" + QString::number(x_b - x_a, 'f', 3) + "\n";
    cursor_text += "Yb - Ya:" + QString::number(y_b - y_a, 'f', 3);

    if (m_yAxis2ShowFlag)
    {
        float y2_a = m_YCursorA->getPositionY2();
        float y2_b = m_YCursorB->getPositionY2();

        cursor_text = " Xa:" + QString::number(x_a, 'f', 3) + "\n";
        cursor_text += "Xb:" + QString::number(x_b, 'f', 3) + "\n";
        cursor_text += "Y1a:" + QString::number(y_a, 'f', 3) + "\n";
        cursor_text += "Y1b:" + QString::number(y_b, 'f', 3) + "\n";
        cursor_text += "Y2a:" + QString::number(y2_a, 'f', 3) + "\n";
        cursor_text += "Y2b:" + QString::number(y2_b, 'f', 3) + "\n";
        cursor_text += "Xb - Xa:" + QString::number(x_b - x_a, 'f', 3) + "\n";
        cursor_text += "Y1b - Y1a:" + QString::number(y_b - y_a, 'f', 3) + "\n";
        cursor_text += "Y2b - Y2a:" + QString::number(y2_b - y2_a, 'f', 3);
    }
    return cursor_text;
}
