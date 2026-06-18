#pragma once
#include <QObject>
#include <QString>
#include <QMap>
#include "Data/TestCasePoint.h"

class QQmlEngine;
class QJSEngine;

class Config : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(Config)

public:
    static QObject* qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Config* singleton();

    explicit Config(QObject* parent = nullptr);
    ~Config();

    Q_INVOKABLE void initPath();
    Q_INVOKABLE void parseConfig();
    Q_INVOKABLE void saveConfig();

    Q_INVOKABLE QString getRootPath();
    Q_INVOKABLE QString getTestDataPath();
    Q_INVOKABLE QString getAgingTestDataPath();

    Q_INVOKABLE void saveLanguage(QString language);
    Q_INVOKABLE QString getLanguage();

    Q_INVOKABLE void saveFirmwarePath(QString dir_path);
    Q_INVOKABLE QString getFirmwarePath();


    Q_INVOKABLE void setEscConfigPath(QString dir_path);
    Q_INVOKABLE QString getEscConfigPath();

    Q_INVOKABLE void setExportCsvPath(QString dir_path);
    Q_INVOKABLE QString getExportCsvPath();

    Q_INVOKABLE bool parseEscConfig(QString file_path);
    Q_INVOKABLE bool saveEscConfig(QString file_path);

    Q_INVOKABLE void parseBleName();
    Q_INVOKABLE void saveBleName();

    Q_INVOKABLE void setBleName(QString addr, QString name);

    Q_INVOKABLE QString getBleName(QString addr, QString name);

    Q_INVOKABLE void saveTestCaseConfigPath(QString dir_path);
    Q_INVOKABLE QString getTesCaseConfigPath();

    Q_INVOKABLE void saveStorageDuration(int storage_duration);
    Q_INVOKABLE int getStorageDuration();

    Q_INVOKABLE void saveEscScan(int start_id, int end_id);
    Q_INVOKABLE int getEscScanStartID();
    Q_INVOKABLE int getEscScanEndID();

    Q_INVOKABLE void saveScanFreq(float start_freq, float end_freq, float freq_step, float scan_count,  int min_pwm, int max_pwm);

    Q_INVOKABLE float getScanFreqStartFreq();
    Q_INVOKABLE float getScanFreqEndFreq();
    Q_INVOKABLE float getScanFreqStep();
    Q_INVOKABLE int getScanFreqCount();

    Q_INVOKABLE int getScanFreqMinPwm();
    Q_INVOKABLE int getScanFreqMaxPwm();


    Q_INVOKABLE int getSupportS200Update();

    Q_INVOKABLE QString getBuildDateTime();

    Q_INVOKABLE bool getAgingTestEnable();
    Q_INVOKABLE bool getLogingDataEnable();


    Q_INVOKABLE int getRTUpdatePeriodMs();

  Q_INVOKABLE void saveFavoriteParams(QStringList favoriteParams);
  Q_INVOKABLE QStringList getFavoriteParams();

private:
    QString m_Language;

    QString m_RootPath;
    QString m_ConfigPath;
    QString m_FirmwarePath;
    QString m_EscConfigPath;
    QString m_EscTestDataPath;
    QString m_AgingTestDataPath;

    QString m_ExportCsvPath;

    QString m_TestCasePath;

    int m_StorageDuration;

    int m_EscScanStartID;
    int m_EscScanEndID;

    float m_ScanFreqStartFreq;
    float m_ScanFreqEndFreq;
    float m_ScanFreqStep;
    int m_ScanFreqCount;
  int m_ScanFreqMinPwm;
  int m_ScanFreqMaxPwm;

  int m_SupportS200Update;

  int m_RTUpdatePeriodMs;

  int m_AgingTestEnable;
  int m_LogingDataEnable;
  QString m_AppTitleSuffix;

  QMap<QString, QString> m_BleNameList;
  QStringList m_FavoriteParams;
};

