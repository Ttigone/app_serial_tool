#include "Config.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>
#include "Data/ParamManager.h"
#include <QDateTime>
#include <QLocale>

QObject* Config::qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine){
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    static Config* _singleton=nullptr;
    if(!_singleton)
        _singleton = new Config;
    return _singleton;
}

Config* Config::singleton(){
    return qobject_cast<Config*>(qmlSingleton(nullptr,nullptr));
}


Config::Config(QObject* parent)
    : QObject(parent)
{
    m_Language = "zh_cn";

    initPath();

    m_StorageDuration = 30;

    m_EscScanStartID = 32;
    m_EscScanEndID = 35;

    m_ScanFreqStartFreq = 1.0;
    m_ScanFreqEndFreq = 5.0;
    m_ScanFreqStep = 1.0;
    m_ScanFreqMinPwm = 1300;
    m_ScanFreqMaxPwm = 1600;
    m_ScanFreqCount = 2;

    m_SupportS200Update = 0;

    m_AgingTestEnable = 0;
    m_LogingDataEnable = 0;


    m_RTUpdatePeriodMs = 20;

    m_AppTitleSuffix = "";

    m_FavoriteParams = QStringList();

    parseConfig();
    parseBleName();

    qDebug() <<  "m_RootPath" << m_RootPath;
    qDebug() <<  "m_ConfigPath" << m_ConfigPath;

    qDebug() <<  "m_FirmwarePath" << m_FirmwarePath;
    qDebug() <<  "m_EscTestDataPath" << m_EscTestDataPath;
    qDebug() <<  "m_EscConfigPath" << m_EscConfigPath;
    qDebug() <<  "m_AgingTestDataPath" << m_AgingTestDataPath;


    qDebug() <<  "m_ExportCsvPath" << m_ExportCsvPath;

    qDebug() <<  "m_TestCasePath" << m_TestCasePath;

    qDebug() <<  "m_RTUpdatePeriodMs" << m_RTUpdatePeriodMs;




}

Config::~Config()
{

}

void Config::initPath()
{
    const QStringList dowmload_location = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    //const QStringList dowmload_location = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

    QString document_path = qApp->applicationDirPath();

    qDebug()<< "applicationDirPath" <<  document_path;
#ifdef Android_Platform
    if(!dowmload_location.isEmpty())
    {
        document_path = dowmload_location.front();
    }

    QString root_path = document_path;
    QDir dir(document_path);
    while(dir.isReadable())
    {
        root_path = dir.path();
        if(dir.cdUp() == false)
            break;
    }

    if(root_path.contains("/storage/emulated/0"))
        root_path = "/storage/emulated/0";

    root_path = "file://" + root_path;
    m_RootPath = root_path;
    m_ConfigPath = document_path;
    m_FirmwarePath = root_path;
    m_EscConfigPath = document_path;
    m_EscTestDataPath = document_path + "/TestCaseData";
    m_ExportCsvPath = document_path + "/TestCaseData";
#else
    m_RootPath = document_path;
    m_ConfigPath = document_path + "/config";
    m_FirmwarePath = document_path;
    m_EscConfigPath = document_path + "/config";
    m_EscTestDataPath = document_path + "/TestCaseData";
    m_AgingTestDataPath = document_path + "/AgingTestData";
    m_ExportCsvPath = document_path + "/TestCaseData";
    m_TestCasePath = document_path;

    QDir config_dir;
    config_dir.mkpath(m_ConfigPath);
#endif
}

void Config::parseConfig()
{
    QString file_path = m_ConfigPath + "/app_config.json";
    QFile file(file_path);
    if (!file.exists())
    {
        QJsonObject jsonObject;
        jsonObject.insert("language", m_Language);
        jsonObject.insert("firmware_path", m_FirmwarePath);
        jsonObject.insert("esc_config_path", m_EscConfigPath);
        jsonObject.insert("test_case_path", m_TestCasePath);
        jsonObject.insert("export_csv_path", m_ExportCsvPath);



        jsonObject.insert("rt_storage_duration", m_StorageDuration);

        jsonObject.insert("esc_scan_start_id", m_EscScanStartID);
        jsonObject.insert("esc_scan_end_id", m_EscScanEndID);


        jsonObject.insert("scan_freq_start_freq", QString::number(m_ScanFreqStartFreq,'f',1));
        jsonObject.insert("scan_freq_end_freq", QString::number(m_ScanFreqEndFreq,'f',1));
        jsonObject.insert("scan_freq_step", QString::number(m_ScanFreqStep,'f',1));
        jsonObject.insert("scan_freq_count", m_ScanFreqCount);

        jsonObject.insert("scan_freq_min_pwm", m_ScanFreqMinPwm);
        jsonObject.insert("scan_freq_max_pwm", m_ScanFreqMaxPwm);

        jsonObject.insert("support_200_update", m_SupportS200Update);


        jsonObject.insert("aging_test_enable", m_AgingTestEnable);
        jsonObject.insert("loging_data_enable", m_LogingDataEnable);
        jsonObject.insert("app_title_suffix", m_AppTitleSuffix);

        jsonObject.insert("rt_update_period_ms", m_RTUpdatePeriodMs);




        // 使用QJsonDocument设置该json对象
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);

        // 将json以文本形式写入文件并关闭文件。
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.write(jsonDoc.toJson());
        file.close();
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QString config_str = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(config_str.toUtf8(), &parseJsonErr);
    if (parseJsonErr.error == QJsonParseError::NoError)
    {
        if (document.isObject())
        {
            QJsonObject obj = document.object();

            if (obj.contains("language"))
            {
                QJsonValue item_value = obj.take("language");
                if (item_value.isString())
                {
                    m_Language = item_value.toString();
                }
            }

            if (obj.contains("firmware_path"))
            {
                QJsonValue item_value = obj.take("firmware_path");
                if (item_value.isString())
                {
                    m_FirmwarePath = item_value.toString();
                }
            }

            if (obj.contains("esc_config_path"))
            {
                QJsonValue item_value = obj.take("esc_config_path");
                if (item_value.isString())
                {
                    m_EscConfigPath = item_value.toString();
                }
            }

            if (obj.contains("export_csv_path"))
            {
                QJsonValue item_value = obj.take("export_csv_path");
                if (item_value.isString())
                {
                    m_TestCasePath = item_value.toString();
                }
            }

            if (obj.contains("test_case_path"))
            {
                QJsonValue item_value = obj.take("test_case_path");
                if (item_value.isString())
                {
                    m_ExportCsvPath = item_value.toString();
                }
            }


            if (obj.contains("rt_storage_duration"))
            {
                QJsonValue item_value = obj.take("rt_storage_duration");

                m_StorageDuration = item_value.toInt();


                if (m_StorageDuration < 1 || m_StorageDuration > 120)
                {
                    m_StorageDuration = 30;
                }
            }


            if (obj.contains("esc_scan_start_id"))
            {
                QJsonValue item_value = obj.take("esc_scan_start_id");

                m_EscScanStartID = item_value.toInt();


                if (m_EscScanStartID < 1 || m_EscScanStartID > 128)
                {
                    m_EscScanStartID = 32;
                }
            }

            if (obj.contains("esc_scan_end_id"))
            {
                QJsonValue item_value = obj.take("esc_scan_end_id");
                m_EscScanEndID = item_value.toInt();
            }

            if (obj.contains("scan_freq_start_freq"))
            {
                QJsonValue item_value = obj.take("scan_freq_start_freq");
                m_ScanFreqStartFreq = item_value.toString().toFloat();
            }

            if (obj.contains("scan_freq_end_freq"))
            {
                QJsonValue item_value = obj.take("scan_freq_end_freq");
                m_ScanFreqEndFreq = item_value.toString().toFloat();
            }

            if (obj.contains("scan_freq_step"))
            {
                QJsonValue item_value = obj.take("scan_freq_step");
                m_ScanFreqStep = item_value.toString().toFloat();
            }

            if (obj.contains("scan_freq_count"))
            {
                QJsonValue item_value = obj.take("scan_freq_count");
                m_ScanFreqCount = item_value.toInt();
            }

            if (obj.contains("scan_freq_min_pwm"))
            {
                QJsonValue item_value = obj.take("scan_freq_min_pwm");
                m_ScanFreqMinPwm = item_value.toInt();
            }

            if (obj.contains("scan_freq_max_pwm"))
            {
                QJsonValue item_value = obj.take("scan_freq_max_pwm");
                m_ScanFreqMaxPwm = item_value.toInt();
            }

            if (obj.contains("support_200_update"))
            {
                QJsonValue item_value = obj.take("support_200_update");
                m_SupportS200Update = item_value.toInt();
            }

            if (obj.contains("aging_test_enable"))
            {
                QJsonValue item_value = obj.take("aging_test_enable");
                m_AgingTestEnable = item_value.toInt();
            }

            if (obj.contains("loging_data_enable"))
            {
                QJsonValue item_value = obj.take("loging_data_enable");
                m_LogingDataEnable = item_value.toInt();
            }

            if (obj.contains("app_title_suffix"))
            {
                QJsonValue item_value = obj.take("app_title_suffix");
                m_AppTitleSuffix = item_value.toString();
            }

            if (obj.contains("rt_update_period_ms"))
            {
                QJsonValue item_value = obj.take("rt_update_period_ms");
                m_RTUpdatePeriodMs = item_value.toInt();
            }

            if (obj.contains("favorite_params"))
            {
                QJsonValue item_value = obj.take("favorite_params");
                if (item_value.isArray())
                {
                    m_FavoriteParams.clear();
                    QJsonArray array = item_value.toArray();
                    for (const QJsonValue& value : array)
                    {
                        if (value.isString())
                        {
                            m_FavoriteParams.append(value.toString());
                        }
                    }
                }
            }
        }
    }
}

void Config::saveConfig()
{
    QJsonObject jsonObject;
    jsonObject.insert("language", m_Language);
    jsonObject.insert("firmware_path", m_FirmwarePath);
    jsonObject.insert("esc_config_path", m_EscConfigPath);
    jsonObject.insert("test_case_path", m_TestCasePath);
    jsonObject.insert("export_csv_path", m_ExportCsvPath);

    jsonObject.insert("rt_storage_duration", m_StorageDuration);

    jsonObject.insert("esc_scan_start_id", m_EscScanStartID);
    jsonObject.insert("esc_scan_end_id", m_EscScanEndID);

    jsonObject.insert("scan_freq_start_freq", QString::number(m_ScanFreqStartFreq,'f',1));
    jsonObject.insert("scan_freq_end_freq", QString::number(m_ScanFreqEndFreq,'f',1));
    jsonObject.insert("scan_freq_step", QString::number(m_ScanFreqStep,'f',1));
    jsonObject.insert("scan_freq_count", m_ScanFreqCount);
    jsonObject.insert("scan_freq_min_pwm", m_ScanFreqMinPwm);
    jsonObject.insert("scan_freq_max_pwm", m_ScanFreqMaxPwm);

    jsonObject.insert("support_200_update", m_SupportS200Update);

    jsonObject.insert("aging_test_enable", m_AgingTestEnable);
    jsonObject.insert("loging_data_enable", m_LogingDataEnable);
    jsonObject.insert("app_title_suffix", m_AppTitleSuffix);

    jsonObject.insert("rt_update_period_ms", m_RTUpdatePeriodMs);

    QJsonArray favoriteParamsArray;
    for (const QString& param : m_FavoriteParams) {
        favoriteParamsArray.append(param);
    }
    jsonObject.insert("favorite_params", favoriteParamsArray);

    // 使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

    // 将json以文本形式写入文件并关闭文件。
    QString file_path = m_ConfigPath + "/app_config.json";
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
}

QString Config::getRootPath()
{
    return m_RootPath;
}

QString Config::getTestDataPath()
{
    return m_EscTestDataPath;
}

QString Config::getAgingTestDataPath()
{
    return m_AgingTestDataPath;
}

void Config::saveFavoriteParams(QStringList favoriteParams)
{
    m_FavoriteParams = favoriteParams;
    saveConfig();
}

QStringList Config::getFavoriteParams()
{
    return m_FavoriteParams;
}

void Config::saveLanguage(QString language)
{
    m_Language = language;
    saveConfig();
}

QString Config::getLanguage()
{
    return m_Language;
}

void Config::saveFirmwarePath(QString dir_path)
{
    m_FirmwarePath = dir_path;
    saveConfig();
}

QString Config::getFirmwarePath()
{
    return m_FirmwarePath;
}

void Config::setEscConfigPath(QString dir_path)
{
    m_EscConfigPath = dir_path;
    saveConfig();
}

QString Config::getEscConfigPath()
{
    return m_EscConfigPath;
}

bool Config::parseEscConfig(QString file_path)
{
#ifdef Android_Platform
    if (file_path.startsWith("file:/")) {
        file_path.remove(0, 6);
    }
#else
    if (file_path.startsWith("file:///")) {
        file_path.remove(0, 8);
    }
    else if(file_path.startsWith("file:")) {
        file_path.remove(0, 5);
    }
#endif

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QString config_str = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(config_str.toUtf8(), &parseJsonErr);
    if (parseJsonErr.error == QJsonParseError::NoError)
    {
        if (document.isObject())
        {
            QJsonObject root_obj = document.object();

            if (root_obj.contains("esc_param"))
            {
                QJsonObject param_obj = root_obj.value("esc_param").toObject();

                QList<int> param_id_list = ParamManager::singleton()->getParamList();
                for(int i = 0; i < param_id_list.size(); i++)
                {
                    int param_id = param_id_list[i];
                    Parameter* param = ParamManager::singleton()->getParamPtr(param_id);
                    param->resetData();
                    QString key_str = param->m_ParamName;

                    if (param_obj.contains(key_str) && param_obj[key_str].isString())
                    {
                        QString data_str = param_obj[key_str].toString();

                        ParamManager::singleton()->setParamValueString(param_id, data_str);
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool Config::saveEscConfig(QString file_path)
{
#ifdef Android_Platform
    if (file_path.startsWith("file:/")) {
        file_path.remove(0, 6);
    }
#else
    if (file_path.startsWith("file:///")) {
        file_path.remove(0, 8);
    }
    else if(file_path.startsWith("file:")) {
        file_path.remove(0, 5);
    }
#endif

    QList<int> param_id_list = ParamManager::singleton()->getParamList();

    QJsonObject param_json_object;

    for(int i = 0; i < param_id_list.size(); i++)
    {
        Parameter* param = ParamManager::singleton()->getParamPtr(param_id_list[i]);
        QString key_str = param->m_ParamName;

        QString value_str = "";

        if(param->m_DisplayType == DisplayDataType::DISPLAY_TYPE_STRING)
        {
            for(int i = 0; i < param->m_ValueArrayDim; i++)
            {
                if(param->m_ValueArray[i] == 0)
                    break;
                value_str.append(param->m_ValueArray[i]);
            }
        }
        else if(param->m_DisplayType == DisplayDataType::DISPLAY_TYPE_INT_ARRAY)
        {
            for(int i = 0; i < param->m_ValueArrayDim; i++)
            {
                value_str.append(QString::number(param->m_ValueArray[i]));

                if(i != (param->m_ValueArrayDim - 1))
                    value_str.append(",");
            }
        }
        else
        {
            value_str = QString::number(param->m_ValueData);
        }

        param_json_object[key_str] = value_str;
    }

    QJsonObject jsonObject;
    jsonObject["esc_param"] = param_json_object;

    // 使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

    // 将json以文本形式写入文件并关闭文件
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();

    return true;
}

void Config::parseBleName()
{
    QString file_path = m_ConfigPath + "/ble_config.json";

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QString config_str = file.readAll();
    file.close();


    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(config_str.toUtf8(), &parseJsonErr);

    QJsonArray jsonArrays =  document.array();

    for(int i = 0; i < jsonArrays.count(); i++)
    {
        QJsonValue valueArrayObject = jsonArrays.at(i);
        if (valueArrayObject.type() != QJsonValue::Object)
        {
            continue;
        }

        QJsonObject jsArrayValue = valueArrayObject.toObject();

        QString ble_addr  = jsArrayValue.value("addr").toString();
        QString ble_name  = jsArrayValue.value("name").toString();
        if(!m_BleNameList.contains(ble_addr))
        {
            m_BleNameList.insert(ble_addr, ble_name);
        }
    }

}

void Config::saveBleName()
{
    QString file_path = m_EscConfigPath + "/ble_config.json";

    QJsonArray jsonArrays;

    QMap<QString,QString>::iterator iter;

    for(iter = m_BleNameList.begin(); iter != m_BleNameList.end(); iter++)
    {
        QJsonObject ble_item;
        ble_item["addr"] = iter.key();
        ble_item["name"] = iter.value();

        jsonArrays.append(QJsonValue(ble_item));
    }

    // 使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArrays);

    // 将json以文本形式写入文件并关闭文件
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
}

void Config::setBleName(QString addr, QString name)
{
    if(m_BleNameList.contains(addr))
    {
        m_BleNameList[addr] = name;
    }
    else
    {
        m_BleNameList[addr] = name;
    }

    saveBleName();
}

QString Config::getBleName(QString addr, QString name)
{
    if(m_BleNameList.contains(addr))
    {
        return m_BleNameList[addr];
    }

    return name;
}

void Config::saveTestCaseConfigPath(QString dir_path)
{
    m_TestCasePath = dir_path;
    saveConfig();
}

QString Config::getTesCaseConfigPath()
{
    return m_TestCasePath;
}

void Config::setExportCsvPath(QString dir_path)
{
    m_ExportCsvPath = dir_path;
    saveConfig();
}
QString Config::getExportCsvPath()
{
    return m_ExportCsvPath;
}


void Config::saveStorageDuration(int storage_duration)
{
    m_StorageDuration = storage_duration;
    saveConfig();
}

int Config::getStorageDuration()
{
    return m_StorageDuration;
}

void Config::saveEscScan(int start_id, int end_id)
{
    m_EscScanStartID = start_id;
    m_EscScanEndID = end_id;
    saveConfig();
}

int Config::getEscScanStartID()
{
    return m_EscScanStartID;
}

int Config::getEscScanEndID()
{
    return m_EscScanEndID;
}

void Config::saveScanFreq(float start_freq, float end_freq, float freq_step, float scan_count,  int min_pwm, int max_pwm)
{
    m_ScanFreqStartFreq = start_freq;
    m_ScanFreqEndFreq = end_freq;
    m_ScanFreqStep = freq_step;
    m_ScanFreqCount = scan_count;

    m_ScanFreqMinPwm = min_pwm;
    m_ScanFreqMaxPwm = max_pwm;

    saveConfig();
}

float Config::getScanFreqStartFreq()
{
    return m_ScanFreqStartFreq;
}

float Config::getScanFreqEndFreq()
{
    return m_ScanFreqEndFreq;
}

float Config::getScanFreqStep()
{
    return m_ScanFreqStep;
}

int Config::getScanFreqCount()
{
    return m_ScanFreqCount;
}

int Config::getScanFreqMinPwm()
{
    return m_ScanFreqMinPwm;
}

int Config::getScanFreqMaxPwm()
{
    return m_ScanFreqMaxPwm;
}

int Config::getSupportS200Update()
{
    return m_SupportS200Update;
}

QString Config::getBuildDateTime()
{
    QString date_time = QStringLiteral("%1 %2").arg(__DATE__).arg(__TIME__);

    QDateTime dateTime = QLocale("en_US").toDateTime(date_time.simplified(), "MMM d yyyy hh:mm:ss");
    QString build_time = dateTime.toString("yyyy-MM-dd hh:mm:ss") + m_AppTitleSuffix;
    return build_time;
}

bool Config::getAgingTestEnable()
{
    return m_AgingTestEnable > 0;
}

bool Config::getLogingDataEnable()
{
    return m_LogingDataEnable > 0;
}

int Config::getRTUpdatePeriodMs()
{
    return m_RTUpdatePeriodMs;
}
