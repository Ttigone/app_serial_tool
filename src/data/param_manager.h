#pragma once

#include <QMap>
#include <QString>
#include <QVector>
#include <QObject>
#include <QMutex>
#include "DataType.h"
#include "Parameter.h"
#include <QVariant>
#include "TestCasePoint.h"
#include "EscErrorInfo.h"

class QQmlEngine;
class QJSEngine;

class ParamManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(ParamManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static ParamManager* singleton();

  ParamManager();
  ~ParamManager();

  bool loadXml(QString fileName);

  void initParam();

  Q_INVOKABLE EscRealtimeDataParam* getEscRealtimeDataParam();
  Q_INVOKABLE EscInfoParam getEscInfoParam();
  EscInfoParam* getEscInfoParamPtr();

  ConfigInfoParam* getConfigInfoParam();

  Q_INVOKABLE EncoderInfoParam getEncoderInfoParam();
  EncoderInfoParam* getEncoderInfoParamPtr();

  LogFileInfoParam* getLogFileInfoParamPtr();

  AgingTestCmdParam* getAgingTestCmdParamPrt();

  EscAgingTestDataParam* getEscAgingTestDataParamPtr(int esc_index);

  Q_INVOKABLE EscAgingTestDataParam getEscAgingTestDataParam(int esc_index);

  Q_INVOKABLE quint8 getEscID();
  Q_INVOKABLE void setEscID(quint8 esc_id);

  Q_INVOKABLE void setEscList(QVector<quint8> esc_list);

  Q_INVOKABLE QList<int> getParamList();

  Q_INVOKABLE Parameter* getParamPtr(int param_id);
  Q_INVOKABLE Parameter* getParamPtr(QString param_id);

  Q_INVOKABLE QVariantMap getParam(int param_id);
  Q_INVOKABLE QVariantMap getParam(QString param_name);

  Q_INVOKABLE QVariantMap getParamOverview(int param_id);

  Q_INVOKABLE QVariantMap getGroupParams(QString group_id);

  Q_INVOKABLE QVariantMap getGroupParams2(QString group_id);

  Q_INVOKABLE void setParamByte(int param_id, CustomByteArray& data_byte);
  Q_INVOKABLE void getParamByte(int param_id, CustomByteArray& data_byte);

  Q_INVOKABLE void setParamValueString(int param_id, QString value_str);
  Q_INVOKABLE void setParamValue(int param_id, int value);

  Q_INVOKABLE int enableTestCase(QString path);
  Q_INVOKABLE QVariantMap getTestCasePoint(int index);
  Q_INVOKABLE bool getTestCaseFlag();
  Q_INVOKABLE QString getTestCaseFileName();

  Q_INVOKABLE void resseTestCase();

  Q_INVOKABLE int processTestCase(int esc_id, int time_interval);
  Q_INVOKABLE QString getTestCaseErrorInfo();

  Q_INVOKABLE void resetFreqTest();
  Q_INVOKABLE void processFreqTest(int esc_id, int time_interval,
                                   int wave_type);

  Q_INVOKABLE QString getSelfCheckErrorInfo(quint16 self_error);
  Q_INVOKABLE QString getRunningErrorInfo(quint16 running_error);

  void emitParamChanged(int param_id);

  void initParamMap();

 signals:
  void paramChanged(int param_id);

 private:
  quint8 m_EscDeviceID;
  QVector<quint8> m_EscDeviceIDList;

  EscRealtimeDataParam* m_EscRealtimeDataParam;

  EscInfoParam* m_EscInfoParam;
  ConfigInfoParam* m_ConfigInfoParam;  // 写参数状态

  EncoderInfoParam* m_EncoderInfoParam;

  LogFileInfoParam* m_LogFileInfoParam;

  AgingTestCmdParam* m_AgingTestCmdParam;

  QVector<EscAgingTestDataParam*> m_EscAgingTestDataParamList;

  QMap<int, Parameter> m_ParamMap;

  QVector<TestCasePoint> m_TestCasePoint;
  QString m_TestCaseFileName;
  bool m_TestCaseFlag;
  int m_TestIndex;

  qint64 m_TestTimeStart;
  qint64 m_TestTimeCurrent;

  int m_TestCurrentTimeUse;
  int m_TestSendPPM;
  int m_TestStartSendPPM;
  int m_TestEndSendPPM;
  int m_TestPPMInterval;

  int m_FreqScanTimeUse;
  float m_FreqScanFreq;
  float m_FreqScanEndFreq;
  float m_FreqScanStep;
  int m_FreqScanFreqCount;
  int m_FreqScanFreqMaxCount;
  int m_FreqScanMinPwm;
  int m_FreqScanMaxPwm;
  int m_FreqScanMiddlePwm;
  int m_FreqScanPwmRange;

  EscErrorInfo m_EscErrorInfo;
  QString m_ErrorInfo;
  quint16 m_ErrorFlag;
};
