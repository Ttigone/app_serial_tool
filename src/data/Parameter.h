#pragma once

#include <QObject>
#include <QImage>
#include <QVariant>

#include "Utils/CustomByteArray.h"

typedef enum {
  DISPLAY_TYPE_INT,
  DISPLAY_TYPE_FLOAT,
  DISPLAY_TYPE_ENUM,
  DISPLAY_TYPE_STRING,
  DISPLAY_TYPE_INT_ARRAY,
} DisplayDataType;

typedef enum {
  DATA_UINT16,
  DATA_INT16,
  DATA_INT32,
  DATA_UINT32,
  DATA_INT8_ARRAY,
  DATA_INT16_ARRAY
} ParamDataType;

typedef enum {
  ATTR_RW,
  ATTR_RO,
} ParamAttrType;

class Parameter : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString groupName READ getGroupName CONSTANT)

  Q_PROPERTY(QString paramID READ getParamID CONSTANT)
  Q_PROPERTY(QString paramName READ getParamName CONSTANT)
  Q_PROPERTY(QString suffix READ getSuffix CONSTANT)
  Q_PROPERTY(QString shortDescInfo READ getShortDescInfo CONSTANT)

  Q_PROPERTY(int value READ getValue WRITE setValue NOTIFY valueChanged)
  Q_PROPERTY(
      int minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
  Q_PROPERTY(
      int maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)
  Q_PROPERTY(int stepValue READ getStep CONSTANT)
  Q_PROPERTY(int scale READ getScale CONSTANT)
  Q_PROPERTY(int decimals READ getDecimals CONSTANT)

 public:
  explicit Parameter(QObject* parent = nullptr);

  Parameter(const Parameter& other);

  Parameter& operator=(const Parameter& other);

  void reset();
  void resetData();

  QString getGroupName();
  int getParamID();
  QString getParamName();
  QString getParamDisplayName();
  QString getSuffix();
  QString getShortDescInfo();

  void setValue(int value);
  int getValue();

  void setArrayValue(int index, int value);
  int getArrayValue(int index);

  void setValueString(QString value);
  QString getValueString();

  void setMinValue(int value);
  int getMinValue();

  void setMaxValue(int value);
  int getMaxValue();

  int getStep();
  int getScale();
  int getDecimals();

  int correctData(int value);

  QString m_GroupName;
  int m_ParamID;
  QString m_ParamName;
  QString m_DisplayName;
  QString m_DisplayNameCN;
  QString m_ShortDescInfo;
  QString m_ShortDescInfoCN;

  QString m_Suffix;

  int m_ValueData;
  int m_MinValue;
  int m_MaxValue;
  int m_StepValue;

  int m_Scale;
  int m_Decimals;

  DisplayDataType m_DisplayType;
  ParamDataType m_DataType;

  ParamAttrType m_Attr;

  QVector<QString> m_EnumItemList;
  QVector<QString> m_EnumItemCNList;

  int m_ValueArray[30];
  int m_ValueArrayDim;
 signals:
  void valueChanged(int value);
  void minValueChanged(int min_value);
  void maxValueChanged(int max_value);
};
