#pragma once

#include <QObject>
#include <QString>
#include <QVector>

typedef struct ErrorInfoStruct {
  quint32 flag_bit;
  QString error_message;
  QString error_analysis;
} ErrorInfo;

class EscErrorInfo : public QObject {
  Q_OBJECT
 public:
  EscErrorInfo();
  ~EscErrorInfo();
  void initErrorInfo();

  Q_INVOKABLE QString getSelfCheckErrorInfo(quint16 self_error);
  Q_INVOKABLE QString getRunningErrorInfo(quint16 running_error);

 private:
  QVector<ErrorInfo> m_SelfCheckErrorInfo;
  QVector<ErrorInfo> m_RunningErrorInfo;
};
