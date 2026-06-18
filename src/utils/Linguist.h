#pragma once
/*
语言家，配置当前系统的语言
*/

#include <QObject>
#include <qtranslator.h>

class Language : public QObject
{
    Q_OBJECT
public:
    Language() {}

    enum LanguageDefine
    {
        zh_cn,
        en_us,
        undefined
    };
    Q_ENUM(LanguageDefine);
};

class Linguist :public QObject
{
    Q_OBJECT
public:
    static  Linguist* singleton();

    void changeLanguage(QString lan);

    int getCurrentLanguage();

private:
    Linguist();
    QTranslator* m_trans;

    int m_CurrentLanguage = Language::undefined;
};
