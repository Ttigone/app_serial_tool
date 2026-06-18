#pragma once

#include <QMap>
#include <QString>
#include "Utils/Utility.h"
#include <QtMath>
#include "CustomPlot/qcustomplot.h"
#include "Utils/Config.h"

#define REALTIME_DATA_INTERVAL_MS 20

class WaveData {
 public:
  WaveData() : m_DataBufferPointer(new QCPGraphDataContainer) {
    m_BufferTimeLength =
        60 * Config::singleton()->getStorageDuration();  // 缓存10min
  }

  ~WaveData() {
    m_DataBufferPointer.clear();  // 删除指针引用
  }

  inline void appendPoint(QCPGraphData sample) {
    m_DataBufferPointer->add(sample);

    //        bool found_range;
    //        QCPRange range = m_DataBufferPointer->keyRange(found_range);
    //        if (!found_range)
    //        {
    //            return;
    //        }

    //        float start_time = range.lower;
    //        float end_time = range.upper;
    //        float time_length = end_time - start_time;

    //        //每次清除60s的缓存数据
    //        if (Utility::FloatGreater(time_length, m_BufferTimeLength + 120))
    //        {
    //            float start_time_limit = end_time - m_BufferTimeLength + 120;
    //            m_DataBufferPointer->removeBefore(start_time_limit);

    //            qDebug() << "appendPoint remove :" << start_time_limit;
    //        }
  }

  inline void appendPoint2(QCPGraphData sample) {
    m_DataBufferPointer->add(sample);
  }

  void clear() { m_DataBufferPointer->clear(); }

  QSharedPointer<QCPGraphDataContainer> m_DataBufferPointer;
  float m_BufferTimeLength;  // 存储时长
};
