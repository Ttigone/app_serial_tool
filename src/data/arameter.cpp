#include "Parameter.h"

#include "ParamManager.h"
#include "Utils/Utility.h"
#include "Utils/Linguist.h"

Parameter::Parameter(QObject* parent) : QObject(parent) { reset(); }

Parameter::Parameter(const Parameter& other) {
  if (this == &other) {
    return;
  }

  this->m_GroupName = other.m_GroupName;
  this->m_ParamID = other.m_ParamID;
  this->m_ParamName = other.m_ParamName;
  this->m_DisplayName = other.m_DisplayName;
  this->m_DisplayNameCN = other.m_DisplayNameCN;
  this->m_ShortDescInfo = other.m_ShortDescInfo;
  this->m_ShortDescInfoCN = other.m_ShortDescInfoCN;

  this->m_Suffix = other.m_Suffix;

  this->m_ValueData = other.m_ValueData;
  this->m_MinValue = other.m_MinValue;
  this->m_MaxValue = other.m_MaxValue;
  this->m_StepValue = other.m_StepValue;
  this->m_Scale = other.m_Scale;
  this->m_Decimals = other.m_Decimals;

  this->m_DisplayType = other.m_DisplayType;
  this->m_DataType = other.m_DataType;
  this->m_Attr = other.m_Attr;

  this->m_EnumItemList = other.m_EnumItemList;
  this->m_EnumItemCNList = other.m_EnumItemCNList;

  this->m_ValueArrayDim = other.m_ValueArrayDim;

  memcpy(this->m_ValueArray, other.m_ValueArray, sizeof(m_ValueArray));
}

Parameter& Parameter::operator=(const Parameter& other) {
  if (this == &other) {
    return *this;
  }

  this->m_GroupName = other.m_GroupName;
  this->m_ParamID = other.m_ParamID;
  this->m_ParamName = other.m_ParamName;
  this->m_DisplayName = other.m_DisplayName;
  this->m_DisplayNameCN = other.m_DisplayNameCN;
  this->m_ShortDescInfo = other.m_ShortDescInfo;
  this->m_ShortDescInfoCN = other.m_ShortDescInfoCN;

  this->m_Suffix = other.m_Suffix;

  this->m_ValueData = other.m_ValueData;
  this->m_MinValue = other.m_MinValue;
  this->m_MaxValue = other.m_MaxValue;
  this->m_StepValue = other.m_StepValue;
  this->m_Scale = other.m_Scale;
  this->m_Decimals = other.m_Decimals;

  this->m_DisplayType = other.m_DisplayType;
  this->m_DataType = other.m_DataType;
  this->m_Attr = other.m_Attr;

  this->m_EnumItemList = other.m_EnumItemList;
  this->m_EnumItemCNList = other.m_EnumItemCNList;

  memcpy(this->m_ValueArray, other.m_ValueArray, sizeof(m_ValueArray));
  this->m_ValueArrayDim = other.m_ValueArrayDim;
  return *this;
}

void Parameter::reset() {
  m_GroupName = "None";
  m_ParamID = 0;
  m_ParamName = "None";
  m_DisplayName = "None";
  m_DisplayNameCN = "None";

  m_ShortDescInfo = "";

  m_ShortDescInfoCN = "";
  m_Suffix = "";

  m_Scale = 1;
  m_Decimals = 0;

  m_ValueData = 0;
  m_MinValue = 0;
  m_MaxValue = 100;
  m_StepValue = 1;

  m_DisplayType = DisplayDataType::DISPLAY_TYPE_INT;
  m_DataType = ParamDataType::DATA_UINT16;
  m_Attr = ParamAttrType::ATTR_RW;

  m_EnumItemList.clear();
  m_EnumItemCNList.clear();

  memset(m_ValueArray, 0, sizeof(m_ValueArray));
  m_ValueArrayDim = 0;
}

void Parameter::resetData() {
  m_ValueData = 0;
  memset(m_ValueArray, 0, sizeof(m_ValueArray));
}

QString Parameter::getGroupName() { return m_GroupName; }

int Parameter::getParamID() { return m_ParamID; }

QString Parameter::getParamName() { return m_ParamName; }

QString Parameter::getParamDisplayName() {
  if (Linguist::singleton()->getCurrentLanguage() == Language::zh_cn) {
    return m_DisplayNameCN;
  } else {
    return m_DisplayName;
  }
}

QString Parameter::getSuffix() { return m_Suffix; }

QString Parameter::getShortDescInfo() { return m_ShortDescInfo; }

void Parameter::setValue(int value) { m_ValueData = correctData(value); }

void Parameter::setArrayValue(int index, int value) {
  if (index < m_ValueArrayDim) {
    m_ValueArray[index] = correctData(value);
  }
}

int Parameter::getArrayValue(int index) {
  if (index < m_ValueArrayDim) {
    return m_ValueArray[index];
  }

  return 0;
}

int Parameter::getValue() { return m_ValueData; }

void Parameter::setMinValue(int value) {
  m_MinValue = value;

  emit minValueChanged(m_MinValue);
}

int Parameter::getMinValue() { return m_MinValue; }

void Parameter::setMaxValue(int value) {
  m_MaxValue = value;

  emit maxValueChanged(m_MaxValue);
}

int Parameter::getMaxValue() { return m_MaxValue; }

int Parameter::getStep() { return m_StepValue; }

int Parameter::getScale() { return m_Scale; }

int Parameter::getDecimals() { return m_Decimals; }

void Parameter::setValueString(QString value) {
  int value_int = value.toInt() * m_Scale;
  m_ValueData = value_int;
}

QString Parameter::getValueString() {
  float value_int = (float)m_ValueData / m_Scale;

  return QString::number(value_int, 'f', m_Decimals);
}

int Parameter::correctData(int value) {
  if (value > m_MaxValue) {
    value = m_MaxValue;
  }

  if (value < m_MinValue) {
    value = m_MinValue;
  }

  return value;
}
