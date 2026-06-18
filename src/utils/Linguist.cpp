#include "Linguist.h"
#include <qcoreapplication.h>
#include <QMetaEnum>
#include <QDir>
#include <QDebug>
Linguist::Linguist()
{
    m_trans = new QTranslator;
}

Linguist* Linguist::singleton()
{
    static Linguist intstance;
    return &intstance;
}

void Linguist::changeLanguage(QString lan)
{
    if (lan == this->m_CurrentLanguage || lan == Language::undefined)
        return;
    bool ret = false;

    QMetaEnum meta_enum = QMetaEnum::fromType<Language::LanguageDefine>();
    int lan_value = meta_enum.keysToValue(lan.toStdString().c_str());

#ifdef Android_Platform
    QString dir = "assets:/trans/";
#else
    QString dir = qApp->applicationDirPath();
    dir += "/assets/trans/";
#endif
    QString file = dir + "zh_CN.qm";
    switch (lan_value)
    {
    case Language::zh_cn:
        if (nullptr != m_trans)
        {
            qApp->removeTranslator(m_trans);
        }
        ret = m_trans->load(dir + "zh_CN.qm");
        if (ret)
        {
            qApp->installTranslator(m_trans);
        }

        break;
    case Language::en_us:
        if (nullptr != m_trans)
        {
            qApp->removeTranslator(m_trans);
        }
        ret = m_trans->load(dir + "en_US.qm");
        if (ret)
        {
            qApp->installTranslator(m_trans);
        }
        break;
    default:
        break;
    }
    if (ret)
    {
        this->m_CurrentLanguage = lan_value;
        // 发出语言被切换的信号
        //emit LanguageChaned();
    }
}

int Linguist::getCurrentLanguage()
{
    return m_CurrentLanguage;
}
